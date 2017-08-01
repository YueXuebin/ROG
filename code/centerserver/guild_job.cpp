#include "centerserver_pch.h"
#include "guild_job.h"
#include "dbdriver.h"
#include "player_job.h"
#include "player.h"
#include "player_manager.h"
#include "player_net_handler.h"
#include "centerserver.h"
#include "gate_handler.h"
#include "game_handler.h"
#include "player_config.h"
#include "guild_manager.h"
#include "guild_config.h"
#include "json_helper.h"
#include "guild_science_config.h"

IMPLEMENT_SINGLETON(GuildJob)


bool SortGuildLevel(const GUILD_BRIEF_INFO& a, const GUILD_BRIEF_INFO& b)
{
    return a.level < b.level;
}
// ==================================================
// 检查公会数据
// ==================================================
void GuildJob::CheckGuildExist(const std::string& guild_name, uint64 playerID)
{
    char name[GUILD_NAME_LEN*2+1] = {0};

    m_dbExecutor->EscapeString(name, guild_name.c_str(), guild_name.length());

    m_dbExecutor->Query(this, &GuildJob::CBCheckGuildExist, playerID, guild_name, 
        "SELECT `guild_id` FROM `guild_info` WHERE `guild_name`='%s'", name);
}

// ==================================================
// 创建公会
// ==================================================
void GuildJob::CreateGuild(Player* player, const std::string& guildName)
{
    uint64 guildID = 0;

    if(!player) return;

    guildID = GuildManager::Instance()->GenGuildID();

    // 存储过程开始
    m_dbExecutor->StartTransaction();

    // 保存角色信息（公会名字有唯一索引保证原子性，所以不会重名）
    if(!AddGuildInfo(guildID, guildName, 1, 1, 0))
    {
        goto _ERROR;
    }

    // 添加公会成员
    if(!AddGuildMember(guildID, player->GetPlayerID(), GUILD_OFFICE_CHAIRMAN))
    {
        goto _ERROR;
    }

    // 更新角色的公会ID
    if(!SetPlayerGuildID(player->GetPlayerID(), guildID))
    {
        goto _ERROR;
    }

    // 提交
    if (!m_dbExecutor->Commit())
    {
        goto _ERROR;
    }

    // 存储过程结束
    m_dbExecutor->EndTransaction();

    player->SetGuildID(guildID);                    // 设置公会ID

    // 通知GameServer公会结构
    GuildManager::Instance()->CreateGuildInfo(guildID, guildName, player);

    // 保存到公会排行榜尾部
    if(!SaveGuildRanking(guildID))
    {
        goto _ERROR;
    }

    // 发送公会创建成功的消息，用来结算创建公会的费用
    GuildManager::Instance()->SendCreateGuildAck(player);

    // 通知角色的公会ID
    GuildManager::Instance()->SendGuildID(player);

    // 公会日志
    GuildManager::Instance()->GuildLog(guildID, GUILD_LOG_CREATE, player->GetPlayerID());
    return;

_ERROR:
    // 存储过程结束
    m_dbExecutor->Rollback();
    m_dbExecutor->EndTransaction();

    player->NotifyErrorCode(ERR_FAILED);
}


/// 删除公会
void GuildJob::DestroyGuild(Player* player)
{
    uint64 guildID = 0;
    GUILD_INFO* guildInfo = 0;
    GUILD_MEMBER* playerInfo = NULL;
    GUILD_NOTICE_ACK noticeMsg;

    if(!player)
    {
        return;
    }

    // 获得公会ID
    guildID = player->GetGuildID();
    if(!guildID)
    {
        return;
    }

    // 检查是否为公会长
    if(!GuildManager::Instance()->IsMaster(player))
    {
        return;
    }

    playerInfo = GuildManager::Instance()->GetGuildMember(guildID, player->GetPlayerID());
    if(!playerInfo)
    {
        return;
    }

    // 存储过程开始
    m_dbExecutor->StartTransaction();

    // 删除角色信息
    if(!DelGuildInfo(guildID))
    {
        goto _ERROR;
    }

    // 删除公会成员
    if(!DelGuildMember(guildID))
    {
        goto _ERROR;
    }

    // 更新角色的公会ID
    if(!ResetMemberGuildID(guildID))
    {
        goto _ERROR;
    }

    // 提交
    if(!m_dbExecutor->Commit())
    {
        goto _ERROR;
    }

    // 存储过程结束
    m_dbExecutor->EndTransaction();

    // NOTICE
    guildInfo = GuildManager::Instance()->GetGuildInfo(guildID);
    if (guildInfo)
    {
        noticeMsg.guild_notice_type = GUILD_NOTICE_TYPE_DISMISS;
        noticeMsg.master_name = playerInfo->name;
        noticeMsg.master_office = playerInfo->office;
        noticeMsg.master_vip = playerInfo->vip_info;
        GuildManager::Instance()->SendToAllGuildMember(guildInfo, MSG_GUILD_NOTICE_ACK, &noticeMsg);
    }

    GuildManager::Instance()->DelGuildInfo(guildID);

    // 公会解散日志
    GuildManager::Instance()->GuildLog(guildID, GUILD_LOG_DESTORY, player->GetPlayerID());

    return;

_ERROR:
    // 存储过程结束
    m_dbExecutor->Rollback();
    m_dbExecutor->EndTransaction();

    player->NotifyErrorCode(ERR_FAILED);
}

// ==================================================
// 添加公会
// ==================================================
bool GuildJob::AddGuildInfo(uint64 guildID, const std::string& guildName, int32 memberCount, int32 level, int32 battle_power)
{
    DBRecorder recorder;
    uint32 crusadeStartTime = 0;
    char name[GUILD_NAME_LEN*2+1] = {0};

    m_dbExecutor->EscapeString(name, guildName.c_str(), guildName.length());

    crusadeStartTime = GuildConfig::Instance()->GetCrusadeStartTime(1);

    // guild addition array
    Json::FastWriter writer;
    Json::Value additions;
    Json::Value guild_addition;
    std::string additions_txt;
    char guild_additions_sqlTxt[MAX_SQLTXT_LEN] = {0};

    additions.clear();

    const GuildAdditionVert& guildAdditionVert = GuildScienceConfig::Instance()->GetGuildAdditions();
    for(GuildAdditionVert::const_iterator guildAddtionIt = guildAdditionVert.begin(); guildAddtionIt != guildAdditionVert.end(); ++guildAddtionIt)
    {
        guild_addition.clear();
        GUILD_ADDITION default_addition;
        default_addition.id = guildAddtionIt->id;
        write_GUILD_ADDITION(default_addition, guild_addition);
        additions["additions"].append(guild_addition);
    }

    if(additions.isNull())
        additions_txt = "{}";
    else
        additions_txt = writer.write(additions);

    m_dbExecutor->EscapeString(guild_additions_sqlTxt, additions_txt.c_str(), additions_txt.length());

    recorder = m_dbExecutor->Query("INSERT INTO `guild_info` SET `guild_id`=%llu, guild_name='%s', member_count=%d, level=%d, additions='%s', "
        "battle_power=%d, bulletin='', guild_boss_level=1, guild_boss_star_time=%u", 
        guildID, name, memberCount, level, guild_additions_sqlTxt, battle_power, crusadeStartTime);

    if(!recorder.HasExecut())
    {
        return false;
    }

    return true;
}

// ==================================================
// 删除公会
// ==================================================
bool GuildJob::DelGuildInfo(uint64 guildID)
{
    DBRecorder recorder;

    recorder = m_dbExecutor->Query("DELETE FROM `guild_info` WHERE `guild_id`=%llu", guildID);

    if(!recorder.HasExecut())
    {
        return false;
    }

    return true;
}

// ==================================================
// 更新角色的公会ID
// ==================================================
bool GuildJob::SetPlayerGuildID(uint64 playerID, uint64 guildID)
{
    DBRecorder recorder;

    recorder = m_dbExecutor->Query("UPDATE `player` SET `guild_id`=%llu WHERE `player_id`=%llu", guildID, playerID);

    if(!recorder.HasExecut())
    {
        return false;
    }

    return true;
}

// ==================================================
// 保存到公会排行榜尾部
// ==================================================
bool GuildJob::SaveGuildRanking(uint64 guildID)
{
    DBRecorder recorder;
    char guild_name[GUILD_NAME_LEN*2+1] = {0};

    GUILD_INFO* guildInfo = GuildManager::Instance()->GetGuildInfo(guildID);
    if(!guildInfo)
    {
        return false;
    }

    m_dbExecutor->EscapeString(guild_name, guildInfo->guild_name.c_str(), guildInfo->guild_name.length());

    recorder = m_dbExecutor->Query("INSERT INTO guild_ranking SELECT COUNT(rank)+1,%llu,'%s',%d,%d,%d FROM guild_ranking", guildID, guild_name, guildInfo->level, guildInfo->member_list.size(), guildInfo->battle_power);

    if(!recorder.HasExecut())
    {
        return false;
    }

    return true;
}

// ==================================================
// 重置公会成员公会ID
// ==================================================
bool GuildJob::ResetMemberGuildID(uint64 guildID)
{
    DBRecorder recorder;
    uint64 zeroID = 0;

    recorder = m_dbExecutor->Query("UPDATE `player` SET `guild_id`=%llu WHERE `guild_id`=%llu", zeroID, guildID);

    if(!recorder.HasExecut())
    {
        return false;
    }

    return true;
}

// ==================================================
// 设置公会标记
// ==================================================
void GuildJob::SetGuildFlag(uint64 guildID, uint32 flag)
{
    m_dbExecutor->Query(this, &GuildJob::CBSetGuildFlag, guildID, flag, 
        "UPDATE `guild_info` SET `flag`=%u WHERE `guild_id`=%llu", flag, guildID);
}

// ==================================================
// 设置公会的等级和经验
// ==================================================
void GuildJob::SetGuildLeveAndExp(uint64 guildID, int32 addLevel, int32 guildExp)
{
    DBRecorder recorder;

   // !!!TODO 由于异步性可能会出现问题，但应该问题不大（经验可能会比预期的少）
    recorder = m_dbExecutor->Query("UPDATE `guild_info` SET `level`= `level` + %u, `exp`=%d WHERE `guild_id`=%llu", addLevel, guildExp, guildID);
}

// ==================================================
// 设置公会的加成
// ==================================================
void GuildJob::SetGuildAddition(uint64 guildID, std::vector<GUILD_ADDITION> addition_list)
{
    // guild addition array
    Json::FastWriter writer;
    Json::Value additions;
    Json::Value guild_addition;
    std::string additions_txt;
    char guild_additions_sqlTxt[MAX_SQLTXT_LEN] = {0};

    additions.clear();
    for(std::vector<GUILD_ADDITION>::iterator additionIt = addition_list.begin(); additionIt != addition_list.end(); ++additionIt)
    {
        guild_addition.clear();
        write_GUILD_ADDITION(*additionIt, guild_addition);
        additions["additions"].append(guild_addition);
    }

    if(additions.isNull())
        additions_txt = "{}";
    else
        additions_txt = writer.write(additions);

    m_dbExecutor->EscapeString(guild_additions_sqlTxt, additions_txt.c_str(), additions_txt.length());

    DBRecorder recorder;

    recorder = m_dbExecutor->Query("UPDATE `guild_info` SET additions='%s' WHERE `guild_id`=%llu", guild_additions_sqlTxt, guildID);
}

// ==================================================
// 设置工会科技加成的玩家信息（游戏币和魔石加成次数）
// ==================================================
//void GuildJob::SetGuildPlayerAddition(uint64 playerID, std::vector<GUILD_PLAYER_ADDITION> addition_list)
//{
//    // guild player addition array
//    Json::FastWriter writer;
//    Json::Value additions;
//    Json::Value guild_addition;
//    std::string additions_txt;
//    char guild_additions_sqlTxt[MAX_SQLTXT_LEN] = {0};
//
//    additions.clear();
//    for(std::vector<GUILD_PLAYER_ADDITION>::iterator additionIt = addition_list.begin(); additionIt != addition_list.end(); ++additionIt)
//    {
//        guild_addition.clear();
//        write_GUILD_PLAYER_ADDITION(*additionIt, guild_addition);
//        additions["additions"].append(guild_addition);
//    }
//
//    if(additions.isNull())
//        additions_txt = "{}";
//    else
//        additions_txt = writer.write(additions);
//
//    m_dbExecutor->EscapeString(guild_additions_sqlTxt, additions_txt.c_str(), additions_txt.length());
//
//     DBRecorder recorder;
//
//    recorder = m_dbExecutor->Query("UPDATE `guild_member` SET additions='%s' WHERE `player_id`=%llu", guild_additions_sqlTxt, playerID);
//}

/// 设置公会BOSS的创建和死亡时间
void GuildJob::SetGuildBossTime(uint64 guildID, uint32 startTime, uint32 createTime, uint32 deadTime)
{
    DBRecorder recorder;

    recorder = m_dbExecutor->Query("UPDATE `guild_info` SET `guild_boss_star_time`=%u, "
        "`guild_boss_create_time`=%u, `guild_boss_dead_time`=%u WHERE `guild_id`=%llu", startTime, createTime, deadTime, guildID);
}

/// 设置公会BOSS的等级和经验
void GuildJob::SetGuildBossLeveAndExp(uint64 guildID, int32 bossLevel, int32 bossExp)
{
    DBRecorder recorder;

    // 同步操作，保证原子性
    recorder = m_dbExecutor->Query("UPDATE `guild_info` SET `guild_boss_level`=%d, `guild_boss_exp`=%d WHERE `guild_id`=%llu", bossLevel, bossExp, guildID);
}

// ==================================================
// 设置公会战斗力
// ==================================================
void GuildJob::SetMemberCountAndBattlpower(uint64 guildID, int32 member_count, int32 battlePower)
{
    m_dbExecutor->Query(this, &GuildJob::CBSetMemberCountAndBattlePower, guildID, battlePower, 
        "UPDATE `guild_info` SET `member_count`=%d, `battle_power`=%u WHERE `guild_id`=%llu", member_count, battlePower, guildID);
}

// ==================================================
// 添加公会成员
// ==================================================
bool GuildJob::AddGuildMember(uint64 guildID, uint64 playerID, int32 officeID)
{
    DBRecorder recorder;

    recorder = m_dbExecutor->Query("INSERT INTO `guild_member` SET `player_id`=%llu, `guild_id`=%llu, `office_id`=%d, `additions`='{}', `contribute_value`=0", playerID, guildID, officeID);

    if(!recorder.HasExecut())
        return false;

    return true;
}

// ==================================================
// 删除公会成员
// ==================================================
bool GuildJob::DelGuildMember(uint64 guildID)
{
    DBRecorder recorder;

    recorder = m_dbExecutor->Query("DELETE FROM `guild_member` WHERE `guild_id`=%llu", guildID);

    if(!recorder.HasExecut())
    {
        return false;
    }

    return true;
}

/// 删除公会成员
bool GuildJob::DelGuildMember(uint64 guildID, uint64 playerID)
{
    DBRecorder recorder;

    recorder = m_dbExecutor->Query("DELETE FROM `guild_member` WHERE `player_id`=%llu AND `guild_id`=%llu", playerID, guildID);

    if(!recorder.HasExecut())
    {
        return false;
    }

    return true;
}

// ==================================================
// 申请加入公会请求列表
// ==================================================
void GuildJob::GetGuildApplyList(uint64 playerID, uint64 guildID)
{
    m_dbExecutor->Query(this, &GuildJob::CBGetGuildApplyList, playerID,
        "SELECT `player_id`, `player_name`, `player_level`, `player_career` FROM `guild_apply` WHERE `guild_id`=%llu LIMIT %d", guildID, GUILD_APPLY_LIST_MAX_COUNT);
}

// ==================================================
// 获得公会列表
// ==================================================
void GuildJob::GetGuildList(uint64 playerID, int32 page)
{
    m_dbExecutor->Query(this, &GuildJob::CBGuildInfoCount, playerID, page, 
        "SELECT COUNT(`guild_id`) FROM `guild_info`");
}

// ==================================================
// 设置公会公告
// ==================================================
void GuildJob::SetGuildBulletin(uint64 guildID, const std::string& bulletin)
{
    char buffer[GUILD_BULLETIN_LEN*2+1] = {0};

    m_dbExecutor->EscapeString(buffer, bulletin.c_str(), bulletin.length());

    m_dbExecutor->Query(this, &GuildJob::CBSetGuildBulletin, guildID, 
        "UPDATE `guild_info` SET `bulletin`='%s' WHERE `guild_id`=%llu", buffer, guildID);
}

// ==================================================
// 加载公会信息
// ==================================================
void GuildJob::LoadGuildInfoForMgr(uint64 guildID)
{
    m_dbExecutor->Query(this, &GuildJob::CBLoadGuildInfoForMgr, 
        "SELECT `guild_id`, `guild_name`, `level`, `exp`, `additions`, `flag`, `battle_power`, "
        "`bulletin`, `guild_boss_level`, `guild_boss_exp`, "
        "`guild_boss_star_time`, `guild_boss_create_time`, `guild_boss_dead_time` "
        "FROM `guild_info` WHERE `guild_id`=%llu", guildID);
}

// ==================================================
// 加载公会成员信息
// ==================================================
void GuildJob::LoadGuildMemberForMgr(uint64 guildID, GUILD_INFO& guildInfo)
{
    m_dbExecutor->Query(this, &GuildJob::CBLoadGuildMemberForMgr, guildInfo, 
        "SELECT T1.player_id, `player_name`, `sex`, `career`, `level`, `battle_power`, UNIX_TIMESTAMP(`logout_time`), `vip_info`, `growth_point`, T2.office_id, T2.contribute_value "
        "FROM player T1 INNER JOIN guild_member T2 ON T1.player_id=T2.player_id "
        "WHERE T2.guild_id=%llu ", guildID);
}

// ==================================================
// 角色加入公会前检查公会
// ==================================================
void GuildJob::CheckPlayerJoinGuild(uint64 applyPlayerID, uint64 invitePlayerID, uint64 guildID)
{
    m_dbExecutor->Query(this, &GuildJob::CBCheckPlayerJoinGuild, applyPlayerID, invitePlayerID, guildID, 
        "SELECT T1.`player_name`, T1.`sex`, T1.`career`, T1.`level`, T1.`battle_power`, UNIX_TIMESTAMP(T1.`logout_time`), T1.`guild_id`, T2.`member_count`, T2.`level` FROM "
        "(SELECT `player_name`, `sex`, `career`, `level`, `battle_power`, `logout_time`, `guild_id` FROM `player` WHERE `player_id`=%llu) AS T1, "
        "(SELECT IFNULL(`member_count`, 0) as `member_count`, IFNULL(`level`, 0) as `level` FROM `guild_info` WHERE `guild_id`=%llu) AS T2", applyPlayerID, guildID);
}

// ==================================================
// 角色离开公会前检查公会ID
// ==================================================
void GuildJob::CheckPlayerLeaveGuild(uint64 playerID, int32 office, uint64 guildID, uint64 delMemberID)
{
    m_dbExecutor->Query(this, &GuildJob::CBCheckPlayerLeaveGuild, playerID, office, guildID, delMemberID, 
        "SELECT `office_id` FROM `guild_member` WHERE `player_id`=%llu AND `guild_id`=%llu", delMemberID, guildID);
}

// ==================================================
// 将角色放入公会中
// ==================================================
void GuildJob::PlayerJoinGuild(uint64 playerID, uint64 guildID, GUILD_MEMBER& guildMember)
{
    Player* player = 0;
    player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);

    guildMember.player_id_h = U64ID_HIGH(playerID);         // 角色ID
    guildMember.player_id_l = U64ID_LOW(playerID);
    guildMember.office = GUILD_OFFICE_MEMBER;                    // 补充公会成员职位
    if(player)
    {
        guildMember.is_online = GUILD_MEMBER_ONLINE_FLAG;
    }
    else
    {
        guildMember.is_online = GUILD_MEMBER_OFFLINE_FLAG;
    }

    GUILD_INFO* guildInfo = 0;
    GUILD_NOTICE_ACK noticeMsg;

    // 存储过程开始
    m_dbExecutor->StartTransaction();

    // 添加公会成员
    if(!AddGuildMember(guildID, playerID, GUILD_OFFICE_MEMBER))
    {
        goto _ERROR;
    }

    // 更新角色的公会ID
    if(!SetPlayerGuildID(playerID, guildID))
    {
        goto _ERROR;
    }

    // 提交
    if (!m_dbExecutor->Commit())
    {
        goto _ERROR;
    }

    // 添加战斗力
    if(player)
    {
        guildMember.battle_power = 0;
    }

    // 公会添加成员
    GuildManager::Instance()->AddGuildMember(guildID, guildMember);

    // 如果角色在线
    if(player)
    {
        guildMember.gatesvr_id = player->GetCentGateID();        // 网关服务器的ID
        player->SetGuildID(guildID);                            // 设置角色的公会ID
        GuildManager::Instance()->SendGuildID(player);          // 通知角色的公会ID
        GuildManager::Instance()->SendGuildInfoToGate(player);  // 通知公会信息

        // 检查公会信息是否已加载，防止玩家加入的是一个全都离线的公会
        if(player->GetGuildID() && !GuildManager::Instance()->GetGuildInfo(player->GetGuildID()))
        {
            GuildJob::Instance()->LoadGuildInfoForMgr(player->GetGuildID());
        }
        //给客户端提示作用
        ERROR_CODE senderrcode;
        senderrcode.errcode = ERR_SUCCEED;
        player->SendToGate(MSG_APPLY_JOIN_GUILD_REQ_ACK, &senderrcode);
    }

    // 公会加入日志
    GuildManager::Instance()->GuildLog(guildID, GUILD_LOG_ADD_MEMBER, playerID);

    // NOTICE
    guildInfo = GuildManager::Instance()->GetGuildInfo(guildID);
    if(guildInfo)
    {
        noticeMsg.guild_notice_type = GUILD_NOTICE_TYPE_ADD;
        noticeMsg.player_name = guildMember.name;
        noticeMsg.player_vip = guildMember.vip_info;
        GuildManager::Instance()->SendToAllGuildMember(guildInfo, MSG_GUILD_NOTICE_ACK, &noticeMsg);
    }

    return;

_ERROR:
    // 存储过程结束
    m_dbExecutor->Rollback();
    m_dbExecutor->EndTransaction();

    //player->NotifyErrorCode(ERR_FAILED);
}

// ==================================================
// 角色离开公会
// ==================================================
void GuildJob::PlayerLeaveGuild(uint64 delMemberID, uint64 guildID, uint64 playerID, int32 office)
{
    Player* player = 0;
    GUILD_INFO* guildInfo = 0;
    GUILD_NOTICE_ACK noticeMsg;
    GUILD_MEMBER* delMemberInfo = GuildManager::Instance()->GetGuildMember(guildID, delMemberID);
    GUILD_MEMBER* playerInfo = GuildManager::Instance()->GetGuildMember(guildID, playerID);

    if (!delMemberInfo)
    {
        goto _ERROR;
    }

    if (!playerInfo)
    {
        goto _ERROR;
    }

    // 存储过程开始
    m_dbExecutor->StartTransaction();

    // 删除公会成员
    if(!DelGuildMember(guildID, delMemberID))
    {
        goto _ERROR;
    }

    // 更新角色的公会ID
    if(!SetPlayerGuildID(delMemberID, 0))
    {
        goto _ERROR;
    }

    // 提交
    if (!m_dbExecutor->Commit())
    {
        goto _ERROR;
    }

    // 如果角色在线
    player = PlayerManager::Instance()->GetPlayerByPlayerID(delMemberID);
    if(player)
    {
        player->SetGuildID(0);                          // 设置角色的公会ID
        GuildManager::Instance()->SendGuildID(player);  // 通知角色的公会ID
    }

    // NOTICE
    guildInfo = GuildManager::Instance()->GetGuildInfo(guildID);
    if (guildInfo)
    {
        if (delMemberID == playerID)
        {
            noticeMsg.guild_notice_type = GUILD_NOTICE_TYPE_QUIT;
        }
        else
        {
            noticeMsg.guild_notice_type = GUILD_NOTICE_TYPE_EXPEL;
            noticeMsg.master_name = playerInfo->name;
            noticeMsg.master_office = playerInfo->office;
            noticeMsg.master_vip = playerInfo->vip_info;
        }
        noticeMsg.player_name = delMemberInfo->name;
        noticeMsg.player_vip = delMemberInfo->vip_info;
        GuildManager::Instance()->SendToAllGuildMember(guildInfo, MSG_GUILD_NOTICE_ACK, &noticeMsg);
    }

    GuildManager::Instance()->DelGuildMember(guildID, delMemberID);

    // 公会减员日志
    GuildManager::Instance()->GuildLog(guildID, GUILD_LOG_REMOVE_MEMBER, delMemberID);

    return;

_ERROR:
    // 存储过程结束
    m_dbExecutor->Rollback();
    m_dbExecutor->EndTransaction();

    //player->NotifyErrorCode(ERR_FAILED);
}

// ==================================================
// 申请加入公会
// ==================================================
void GuildJob::AddGuildApply(uint64 guildID, uint64 playerID, const std::string& playerName, int32 level, int32 career)
{
    char name[PLAYER_NAME_LEN*2+1] = {0};

    m_dbExecutor->EscapeString(name, playerName.c_str(), playerName.length());

    m_dbExecutor->Query(this, &GuildJob::CBAddGuildApply, guildID, 
        "INSERT INTO `guild_apply` SET `guild_id`=%llu, `player_id`=%llu, `player_name`='%s', `player_level`=%d, `player_career`=%d", 
        guildID, playerID, name, level, career);
}

// ==================================================
// 删除加入公会申请
// ==================================================
void GuildJob::DelGuildApply(uint64 guildID, uint64 playerID)
{
    m_dbExecutor->Query(this, &GuildJob::CBDelGuildApply, guildID, 
        "DELETE FROM `guild_apply` WHERE `guild_id`=%llu AND `player_id`=%llu", guildID, playerID);
}

// ==================================================
// 加入公会检查自动加入标记
// ==================================================
void GuildJob::JoinGuildCheckFlag(uint64 guildID, uint64 playerID)
{
    m_dbExecutor->Query(this, &GuildJob::CBJoinGuildCheckFlag, guildID, playerID, 
        "SELECT `flag`, `level`, `member_count` FROM guild_info WHERE `guild_id`=%llu", guildID);
}

// ==================================================
/// 设置公会成员职位
// ==================================================
void GuildJob::SetGuildMemberOffice(uint64 guildID, uint64 playerID, int32 office)
{
    m_dbExecutor->Query(this, &GuildJob::CBSetGuildMemberOffice, guildID, playerID, office, 
        "UPDATE `guild_member` SET `office_id`=%d WHERE `guild_id`=%llu AND `player_id`=%llu", office, guildID, playerID);
}

void GuildJob::SetGuildMemberOfficeNoLock(uint64 guildID, uint64 playerID, int32 office)
{
    m_dbExecutor->Query(this, &GuildJob::CBSetGuildMemberOfficeNoLock, guildID, playerID, office, 
        "UPDATE `guild_member` SET `office_id`=%d WHERE `guild_id`=%llu AND `player_id`=%llu", office, guildID, playerID);
}

// ==================================================
/// 贡献点加入玩家总贡献里
// ==================================================
void GuildJob::AddGuildPointToContributeValue(uint64 guildID, uint64 playerID, int32 guildPoint)
{
    m_dbExecutor->Query(this, &GuildJob::CBAddGuildPointToContributeValue, guildID, playerID, guildPoint, 
        "UPDATE `guild_member` SET `contribute_value`=`contribute_value`+%d WHERE `guild_id`=%llu AND `player_id`=%llu", guildPoint, guildID, playerID);
}

// ==================================================
// 根据公会名字查询ID
// ==================================================
void GuildJob::CBCheckGuildExist(DBRecorder& res, uint64 playerID, std::string guild_name)
{
    Player* player = 0;
    uint64 guild_id = 0;

    player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    if(!player) return;

    if(res.GetRowNum() > 0)
    {
        res.GetRecord(guild_id);
    }

    // 检查是否已加入公会
    if(guild_id)
    {
        player->NotifyErrorCode(ERR_GUILD_DUPLICATE_GUILD_NAME);
    }
    else
    {
        CreateGuild(player, guild_name);
    }
}

// ==================================================
// 设置公会标记
// ==================================================
void GuildJob::CBSetGuildFlag(DBRecorder& res, uint64 guildID, uint32 guildFlag)
{
    if(!res.HasExecut())
    {
        return;
    }

    GuildManager::Instance()->SetGuildFlag(guildID, guildFlag);
}

// ==================================================
// 设置公会等级和经验
// ==================================================
//void GuildJob::CBSetGuildLeveAndExp(DBRecorder& res, uint64 guildID, int32 guildLevel, int32 guildExp)
//{
//    if(!res.HasExecut())
//    {
//        return;
//    }
//}

// ==================================================
// 设置公会战斗力
// ==================================================
void GuildJob::CBSetMemberCountAndBattlePower(DBRecorder& res, uint64 guildID, int32 battlePower)
{
    
}

// ==================================================
// 查询公会列表回调
// ==================================================
void GuildJob::CBGetGuildList(DBRecorder& res, uint64 playerID, int32 page, int32 count)
{
    GUILD_LIST_ACK sendMsg;
    GUILD_BRIEF_INFO guildBriefInfo;
    Player* player = 0;
    uint64 guildID = 0;
    uint64 applyPlyerID = 0;

    player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    if(!player) return;

    sendMsg.curr_page = page;
    sendMsg.total_page = count;

    for(uint32 i=0; i < res.GetRowNum(); ++i)
    {
        guildBriefInfo.Clear();

        CnAssert(res.GetCol() == 0);

        res.GetRecord(guildID);
        guildBriefInfo.guild_id_h = U64ID_HIGH(guildID);
        guildBriefInfo.guild_id_l = U64ID_LOW(guildID);

        res.GetRecord(guildBriefInfo.guild_name);
        res.GetRecord(guildBriefInfo.level);
        res.GetRecord(guildBriefInfo.member_count);
        res.GetRecord(guildBriefInfo.battle_power);


        // 用来判定此用户是否已申请过加入这个公会
        res.GetRecord(applyPlyerID, false);
        if(applyPlyerID > 0)
        {
            guildBriefInfo.flag = 1;
        }
        sendMsg.guild_list.push_back(guildBriefInfo);

        std::sort(sendMsg.guild_list.begin(), sendMsg.guild_list.end(), SortGuildLevel);
    }

    player->SendToGate(MSG_GUILD_LIST_ACK, &sendMsg);
}

// ==================================================
// 获得加入公会的请求列表
// ==================================================
void GuildJob::CBGetGuildApplyList(DBRecorder& res, uint64 playerID)
{
    GUILD_APPLY_INFO applyInfo;
    GUILD_APPLY_LIST applyList;
    uint64 applyPlayerID = 0;
    Player* player = 0;

    player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    if(!player) return;

    applyList.Clear();
    for(uint32 i=0; i < res.GetRowNum(); ++i)
    {
        applyInfo.Clear();

        res.GetRecord(applyPlayerID);
        applyInfo.player_id_h = U64ID_HIGH(applyPlayerID);
        applyInfo.player_id_l = U64ID_LOW(applyPlayerID);
        res.GetRecord(applyInfo.player_name);
        res.GetRecord(applyInfo.player_level);
        res.GetRecord(applyInfo.player_career);

        applyList.apply_list.push_back(applyInfo);
    }

    player->SendToGate(MSG_GUILD_APPLY_LIST_ACK, &applyList);
}

// ==================================================
// 获得公会列表总数回调
// ==================================================
void GuildJob::CBGuildInfoCount(DBRecorder& res, uint64 playerID, int32 page)
{
    int64 count = 0;
    int32 begin_num = 0;

    if(res.GetRowNum() > 0)
    {
        res.GetRecord(count);
    }

    if(count < 1)
    {
        count = 0;
        begin_num = 0;
    }
    else
    {
        begin_num = page * GUILD_LIST_PAGE_COUNT;

        if(begin_num > (int32)count)
        {
            if(count > GUILD_LIST_PAGE_COUNT)
            {
                begin_num = (int32)count - GUILD_LIST_PAGE_COUNT;
            }
            else
            {
                begin_num = 0;
            }
        }
        page = begin_num / GUILD_LIST_PAGE_COUNT;
        count = (count / GUILD_LIST_PAGE_COUNT) + 1;
    }

    m_dbExecutor->Query(this, &GuildJob::CBGetGuildList, playerID, page, (int32)count, 
        "SELECT T1.`guild_id`, T1.`guild_name`, T1.`level`, T1.`member_count`, T1.`battle_power`, IFNULL(T2.`player_id`, 0) "
        "FROM (SELECT `guild_id`, `guild_name`, `level`, `member_count`, `battle_power` FROM "
        "`guild_info` ORDER BY `battle_power` DESC LIMIT %d, %d) AS T1 "
        "LEFT JOIN ( SELECT `guild_id`, `player_id` FROM `guild_apply` WHERE `player_id`=%llu"
        ") AS T2 ON T1.`guild_id` = T2.`guild_id`", begin_num, GUILD_LIST_PAGE_COUNT, playerID);
}

// ==================================================
// 设置公会公告
// ==================================================
void GuildJob::CBSetGuildBulletin(DBRecorder& res, uint64 guildID)
{
    
}

// ==================================================
// 加载公会信息
// ==================================================
void GuildJob::CBLoadGuildInfoForMgr(DBRecorder& res)
{
    GUILD_INFO guildInfo;
    uint64 guildID = 0;

    Json::Reader reader;
    Json::Value additions_json;
    std::string additions;

    guildInfo.Clear();

    if(res.GetRowNum() < 1) return;

    guildInfo.opt_flag = OPT_INIT_FLAG;

    res.GetRecord(guildID);
    guildInfo.guild_id_h = U64ID_HIGH(guildID);
    guildInfo.guild_id_l = U64ID_LOW(guildID);

    res.GetRecord(guildInfo.guild_name);
    res.GetRecord(guildInfo.level);
    res.GetRecord(guildInfo.exp);
    res.GetRecord(additions);

    if (!reader.parse(additions, additions_json, false))
    {
        CnError("Parse guild additions failed\n");
        return;
    }
    
    uint32 adds_count = additions_json["additions"].size();
    for (uint32 i = 0; i < adds_count; i++)
    {
        Json::Value add_json = additions_json["additions"][i];
        GUILD_ADDITION guildAddition;
        read_GUILD_ADDITION(guildAddition, add_json);
        guildInfo.addition_list.push_back(guildAddition);
    }

    res.GetRecord(guildInfo.flag);
    res.GetRecord(guildInfo.battle_power);
    res.GetRecord(guildInfo.bulletin);
    res.GetRecord(guildInfo.guild_boss_level);
    res.GetRecord(guildInfo.guild_boss_exp);
    res.GetRecord(guildInfo.guild_boss_start_time);
    res.GetRecord(guildInfo.guild_boss_create_time);
    res.GetRecord(guildInfo.guild_boss_dead_time);

    LoadGuildMemberForMgr(guildID, guildInfo);
}

// ==================================================
// 加载公会成员信息
// ==================================================
void GuildJob::CBLoadGuildMemberForMgr(DBRecorder& res, GUILD_INFO guildInfo)
{
    GUILD_MEMBER member;
    uint64 playerID = 0;
    uint64 guildID = 0;

    Json::Reader reader;
    Json::Value vip_info_json;
    std::string vip_info_str;

    for(uint32 i=0; i<res.GetRowNum(); ++i)
    {
        member.Clear();
        res.GetRecord(playerID);
        member.player_id_h = U64ID_HIGH(playerID);
        member.player_id_l = U64ID_LOW(playerID);
        res.GetRecord(member.name);
        res.GetRecord(member.sex);
        res.GetRecord(member.career);
        res.GetRecord(member.level);
        res.GetRecord(member.battle_power);

        int64 logout_time = 0;
        res.GetRecord(logout_time); // 最后在线时间
        member.last_online = (uint32)logout_time;

        res.GetRecord(vip_info_str);
        vip_info_json.clear();
        if (!reader.parse(vip_info_str, vip_info_json, false))
        {
            CnAssert(false);
        }
        ParseVipInfo(vip_info_json, member.vip_info);
        res.GetRecord(member.vip_info.growth_point_recharge);

        res.GetRecord(member.office);
        res.GetRecord(member.contribute_value);

        guildInfo.member_list.push_back(member);
    }

    GuildManager::Instance()->LoadGuildInfo(&guildInfo);
}

// ==================================================
// 加入公会
// ==================================================
void GuildJob::CBCheckPlayerJoinGuild(DBRecorder& res, uint64 applyPlayerID, uint64 invitePlayerID, uint64 guildID)
{
    GUILD_MEMBER guildMember;
    uint64 playerGuildID = 0;
    int32 guildMemberCount = 0;
    int32 guildLevel = 0;
    Player* player = 0;
    GUILD_INFO* guildInfo = 0;
    const GuildLevelTmpl* guildLevelTmpl = 0;
    uint32 errcode = ERR_FAILED;

    guildMember.Clear();

    // 不存在此玩家（应该不存在）
    if(res.GetRowNum() < 1)
    {
        return;
    }

    res.GetRecord(guildMember.name);        // 名字
    res.GetRecord(guildMember.sex);         // 性别
    res.GetRecord(guildMember.career);      // 职业
    res.GetRecord(guildMember.level);       // 等级
    res.GetRecord(guildMember.battle_power);

    int64 logout_time = 0;
    res.GetRecord(logout_time); // 最后在线时间
    guildMember.last_online = (uint32)logout_time;

    res.GetRecord(playerGuildID);           // 获得玩家的公会ID
    res.GetRecord(guildMemberCount, false); // 公会成员计数
    res.GetRecord(guildLevel, false);       // 公会成员计数

    // 获得角色指针
    player = PlayerManager::Instance()->GetPlayerByPlayerID(invitePlayerID);

    // 检查角色是否已加入公会
    // 这步只是临时检查一下，这个的原子性通过在公会成员表中插入角儿的ID来保证
    if(playerGuildID)
    {
        errcode = ERR_HAVE_GUILD;
        if(applyPlayerID != invitePlayerID)
            errcode = ERR_INVITE_PLAYER_HAVE_GUILD;
        goto _ERR;
    }

    // 如果有在线公会信息，则覆盖从数据库中查找的信息
    guildInfo = GuildManager::Instance()->GetGuildInfo(guildID);
    if(guildInfo)
    {
        guildMemberCount = (int32)guildInfo->member_list.size();
        guildLevel = guildInfo->level;
    }

    // 获得工会模板
    guildLevelTmpl = GuildConfig::Instance()->GetGuildLeveTmpl(guildLevel);
    if(!guildLevelTmpl)
    {
        goto _ERR;
    }

    // 检查公会人数
    if(guildMemberCount >= guildLevelTmpl->max_member)
    {
        errcode = ERR_MAX_GUILD_MEMBERNUM;
        goto _ERR;
    }

    // 角色加入公会
    PlayerJoinGuild(applyPlayerID, guildID, guildMember);
    
    return;

_ERR:
    if(player)
    {
        player->NotifyErrorCode(errcode);
    }
}

// ==================================================
// 离开公会
// ==================================================
void GuildJob::CBCheckPlayerLeaveGuild(DBRecorder& res, uint64 playerID, int32 office, uint64 guildID, uint64 delMemberID)
{
    int32 delOffice = 0;
    Player* player = 0;

    // 玩家不在此公会
    if(res.GetRowNum() < 1)
    {
        return;
    }

    // 获得玩家的公会职位
    res.GetRecord(delOffice);

    // 如果不是自己离职，则需要检查职位
    if(playerID != delMemberID)
    {
        // 检查角色的职位
        if(office >= delOffice)
        {
            return;
        }
    }

    // 角色离开公会
    PlayerLeaveGuild(delMemberID, guildID, playerID, office);
}

// ==================================================
// 添加公会申请
// ==================================================
void GuildJob::CBAddGuildApply(DBRecorder& res, uint64 guildID)
{
    if(!res.HasExecut())
    {
        return;
    }
    GuildManager::Instance()->SendAddApplyNtf(guildID);
}

// ==================================================
// 加入公会检查标记
// ==================================================
void GuildJob::CBJoinGuildCheckFlag(DBRecorder& res, uint64 guildID, uint64 playerID)
{
    const GuildLevelTmpl* guildLevelTmpl = 0;
    Player* player = 0;
    uint32  guild_flag = 0;
    int32   guild_level = 0;
    int32   guild_member_num = 0;

    player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    if(!player)
    {
        return;
    }

    // 这一步的检查可以说明公会存在
    if(res.GetRowNum() < 1)
    {
        return;
    }

    res.GetRecord(guild_flag);
    res.GetRecord(guild_level);
    res.GetRecord(guild_member_num);

    // 获得工会模板
    guildLevelTmpl = GuildConfig::Instance()->GetGuildLeveTmpl(guild_level);
    if(!guildLevelTmpl)
    {
        player->NotifyErrorCode(ERR_FAILED);
        return;
    }

    // 检查公会人数
    if(guild_member_num >= guildLevelTmpl->max_member)
    {
        player->NotifyErrorCode(ERR_MAX_GUILD_MEMBERNUM);
        return;
    }

    // 检查公会是否为自动加入
    if(guild_flag && GUILD_AUTO_JOIN_FLAG)
    {
        CheckPlayerJoinGuild(playerID, playerID, guildID);
    }
    else
    {
        AddGuildApply(guildID, playerID, player->GetName(), player->GetLevel(), player->GetCareer());

        //给客户端提示作用
        ERROR_CODE senderrcode;
        senderrcode.errcode = ERR_SUCCEED;
        player->SendToGate(MSG_APPLY_JOIN_GUILD_REQ_ACK, &senderrcode);
    }
}

// ==================================================
// 删除公会申请
// ==================================================
void GuildJob::CBDelGuildApply(DBRecorder& res, uint64 guildID)
{
    // 刷新公会会长列表数据
    std::vector<GUILD_MEMBER*> guildMemberList = GuildManager::Instance()->GetGuildMemberByOffice(guildID, GUILD_OFFICE_CHAIRMAN);
    if(guildMemberList.size() != 1)
        return;

    std::vector<GUILD_MEMBER*>::iterator guildChairMan = guildMemberList.begin();
    uint64 chairmanID = U64ID((*guildChairMan)->player_id_h, (*guildChairMan)->player_id_l);
    Player* pPlayer = PlayerManager::Instance()->GetPlayerByPlayerID(chairmanID);
    if(!pPlayer)
        return;

    GuildJob::Instance()->GetGuildApplyList(pPlayer->GetPlayerID(), guildID);
}

// ==================================================
// 设置公会职位
// ==================================================
void GuildJob::CBSetGuildMemberOffice(DBRecorder& res, uint64 guildID, uint64 playerID, int32 office)
{
    GUILD_INFO guildInfo;
    Player* player = 0;

    if(!res.HasExecut())
    {
        return;
    }

    GuildManager::Instance()->CBSetMemberOffice(guildID, playerID, office);
}

// ==================================================
/// 设置公会职位无锁
// ==================================================
void GuildJob::CBSetGuildMemberOfficeNoLock(DBRecorder& res, uint64 guildID, uint64 playerID, int32 office)
{
    GUILD_INFO* guildInfo = 0;
    GUILD_NOTICE_ACK noticeMsg;
    GUILD_MEMBER* playerInfo = NULL;

    if(!res.HasExecut())
    {
        return;
    }

    if (office == GUILD_OFFICE_CHAIRMAN)
    {
        playerInfo = GuildManager::Instance()->GetGuildMember(guildID, playerID);
        guildInfo = GuildManager::Instance()->GetGuildInfo(guildID);
        if (guildInfo && playerInfo)
        {
            // NOTICE
            noticeMsg.guild_notice_type = GUILD_NOTICE_TYPE_INHERIT;
            noticeMsg.player_name = playerInfo->name;
            noticeMsg.player_office = playerInfo->office;
            noticeMsg.player_vip = playerInfo->vip_info;
            GuildManager::Instance()->SendToAllGuildMember(guildInfo, MSG_GUILD_NOTICE_ACK, &noticeMsg);
        }
    }

    GuildManager::Instance()->CBSetMemberOfficeNoLock(guildID, playerID, office);
}

// ==================================================
/// 贡献点加入玩家总贡献里
// ==================================================
void GuildJob::CBAddGuildPointToContributeValue(DBRecorder& res, uint64 guildID, uint64 playerID, int32 office)
{
    GUILD_INFO guildInfo;
    Player* player = 0;

    if(!res.HasExecut())
    {
        return;
    }

    GuildManager::Instance()->AddGuildPointToContributeValue(guildID, playerID, office);
}

// ==================================================
// 解析VIP信息
// ==================================================
void GuildJob::ParseVipInfo(Json::Value& json_val, VIP_INFO& vipInfo)
{
    vipInfo.vip_first_ts = toUint32(json_val["vip_first_ts"]);
    vipInfo.vip_ts = toUint32(json_val["vip_ts"]);
    vipInfo.is_vip = toUint32(json_val["is_vip"]);
    vipInfo.vip_card_1_num = toUint32(json_val["vip_card_1_num"]);
    vipInfo.vip_card_2_num = toUint32(json_val["vip_card_2_num"]);
    vipInfo.vip_card_3_num = toUint32(json_val["vip_card_3_num"]);
    vipInfo.is_vip_reward_today = toUint32(json_val["is_vip_reward_today"]);
    vipInfo.vip_gif_flag = toUint32(json_val["vip_gif_flag"]);
    vipInfo.experience_vip_state = toInt32(json_val["experience_vip_state"]);
    vipInfo.growth_point_no_recharge = toInt32(json_val["growth_point_no_recharge"]);
}
