#include "centerserver_pch.h"
#include "player_info_job.h" 
#include "dbdriver.h"
#include "gate_handler.h"
#include "player_manager.h"
#include "player_config.h"
#include "json_helper.h"
#include "player.h"
#include "kunlun_log.h"
#include "dongyi_log.h"
#include "log_writer.h"
#include "config_loader.h"
#include "platform_callbacker.h"



IMPLEMENT_SINGLETON(PlayerInfoJob)

void PlayerInfoJob::CreatePlayerInfo(uint32 gateSvrID, uint64 loginID, uint64 playerID, 
                                     const std::string& uid, const std::string& uname, const std::string& platform, const PROPERTY_INFO& info)
{
    // 存储过程开始
    m_dbExecutor->StartTransaction();

    uint16 errCode = ERR_FAILED;

    // 创建玩家
    if(!CreatePlayer(playerID, uid, uname, platform, info))
    {
        errCode = ERR_HAVE_EXIST_PLAYER_NAME;
        goto _ERROR;
    }

    // 创建玩家默认头像
    if(!CreateHeadPortrait(playerID, info))
    {
        errCode = ERR_HAVE_EXIST_PLAYER_NAME;
        goto _ERROR;
    }
    

    // 创建角色日志
    if(!CreatePlayerLog(loginID, playerID, uname, info))
    {
        errCode = ERR_HAVE_EXIST_PLAYER_NAME;
        goto _ERROR;
    }

    // 创建角色的初始物品
    if(!CreateItem(playerID))
    {
        errCode = ERR_CREATE_PLAYER_FAILED;
        goto _ERROR;
    }

    // 创建角色的初始技能
    if(!CreateSkill(playerID))
    {
        errCode = ERR_CREATE_PLAYER_FAILED;
        goto _ERROR;
    }

    // 创建角色的宠物
    if(!CreatePet(playerID))
    {
        errCode = ERR_CREATE_PLAYER_FAILED;
        goto _ERROR;
    }

    // 创建角色离线信息
    if(!CreateOfflinePlayerInfo(playerID, info))
    {
        CnWarn("create CreateOfflinePlayerInfo failed, account=%s\n", info.account.c_str());
        
        errCode = ERR_CREATE_PLAYER_FAILED;
        goto _ERROR;
    }

    


    // 创建角色的副本信息
    //if(!CreatePlayerDungeonInfo(playerID))
    //{
    //    errCode = ERR_CREATE_PLAYER_FAILED;
    //    goto _ERROR;
    //}
    //// 再创建角色的初始装备
    //if(!CreatePlayerEquip(gateSvrID, loginID, playerID, uname, info))
    //{
    //    errCode = ERR_CREATE_PLAYER_FAILED;
    //    goto _ERROR;
    //}

    //// 创建普通活动
    //if(!CreateDailyActivity(playerID))
    //{
    //    errCode = ERR_CREATE_PLAYER_FAILED;
    //    goto _ERROR;
    //}

    //// 创建在线活动
    //if(!CreateOnlineActivity(playerID))
    //{
    //    errCode = ERR_CREATE_PLAYER_FAILED;
    //    goto _ERROR;
    //}

    // 提交
    if (!m_dbExecutor->Commit())
    {
        goto _ERROR;
    }

    errCode = ERR_SUCCEED;
    // 存储过程结束
    m_dbExecutor->EndTransaction();

    OnCreatePlayerInfoEnd(gateSvrID, loginID, playerID, uid, uname, info);

    return;

_ERROR:
    // 存储过程结束
    m_dbExecutor->Rollback();
    m_dbExecutor->EndTransaction();

    if(errCode != ERR_SUCCEED)
    {
        GateHandler::Instance()->SendCreatePlayerAck(errCode, gateSvrID, loginID);
    }

}

// 创建角色数据完成
bool PlayerInfoJob::OnCreatePlayerInfoEnd(uint32 gateSvrID, uint64 loginID, uint64 playerID, const std::string& uid, const std::string& uname, const PROPERTY_INFO& info)
{
    // 创建角色平台回调
    GetPlatformCallbacker()->OnCreateCharacter(loginID, playerID, uid, uname, info);

    // 创建成功通知
    ERROR_CODE sendMsg;
    sendMsg.errcode = ERR_SUCCEED;
    GateHandler::Instance()->SendCreatePlayerAck(sendMsg.errcode, gateSvrID, loginID);

    return true;
}
// ==================================================
// 创建在线活动
// ==================================================
bool PlayerInfoJob::CreateOnlineActivity(uint64 playerID)
{
    DBRecorder recorder;
    recorder = m_dbExecutor->Query("INSERT INTO `online_activity` VALUES(%llu, '{}', '{}')", playerID);
    if(!recorder.HasExecut())
    {
        return false;
    }
    return true;
}

// ==================================================
// 创建普通活动
// ==================================================
bool PlayerInfoJob::CreateDailyActivity(uint64 playerID)
{
    DBRecorder recorder;
    recorder = m_dbExecutor->Query("INSERT INTO `daily_activity` VALUES(%llu, '{}', '{}', '{}', '{}', '{}', '{}','{}')", playerID);
    if(!recorder.HasExecut())
    {
        return false;
    }

    return true;
}
// ==================================================
// 创建装备
// ==================================================
bool PlayerInfoJob::CreateEquip(uint64 playerID)
{

    DBRecorder recorder;
    recorder =  m_dbExecutor->Query("INSERT INTO `equip` SET `player_id`=%llu, `equip_info`='{}';", playerID);
    if(!recorder.HasExecut())
    {
        return false;
    }


    return true;

//     // ====以下操作必须为同步操作================================================================================================
// 
//     // 如果角色之前有充值，需要同步充值返还，做充值返还
//     if(!RechargeReturn(playerID, uname))
//     {
//         CnWarn("create RechargeReturn failed, account=%s\n", propInfo.account.c_str());
//         return false;
//     }
// 
//     // 创建角色副本信息
//     if(!CreatePlayerDungeonInfo(playerID))
//     {
//         CnWarn("create CreatePlayerDungeonInfo failed, account=%s\n", propInfo.account.c_str());
//         return false;
//     }
// 

// 
//     // 创建离线排行榜信息
//     if(!CreateOfflineRankingInfo(playerID))
//     {
//         CnWarn("create CreateOfflineRankingInfo failed, account=%s\n", propInfo.account.c_str());
//         return false;
//     }
}

// ==================================================
//创建角色日志
// ==================================================
bool PlayerInfoJob::CreatePlayerLog(uint64 loginID, uint64 playerID, const std::string& uname,const PROPERTY_INFO& propInfo)
{
    Player* player = NULL;

    // 创建角色日志
    player = PlayerManager::Instance()->GetPlayerByLoginID(loginID);
    if(player)
    {
        KunlunLog::Instance()->LogActive(player->GetPlatformUID(), playerID, player->GetPlatformUName(), player->GetLoginIP());
    }

    uint32 now = (uint32)Crown::SDNow().GetTimeValue();
    uint32 loginip = 0;
    if(player)
    {
        loginip = player->GetLoginIP();
    }

    DoingGameLog::Instance()->WriteCenterLog(LOG_CREATE_PLAYER, playerID, WriterToLogByCreatePlayer(now, uname, propInfo.name, propInfo.sex, propInfo.career, loginip));

    if(player)
        return true;

    return false;
}

//创建玩家信息
bool PlayerInfoJob::CreatePlayer(uint64 playerID, 
                                 const std::string& uid, const std::string& uname, const std::string& platform, const PROPERTY_INFO& info)
{
    DBRecorder recorder;

    char account[PLAYER_ACCOUNT_LEN*2+1] = {0};
    char name[PLAYER_NAME_LEN*2+1] = {0};

    m_dbExecutor->EscapeString(account, info.account.c_str(), info.account.length());
    m_dbExecutor->EscapeString(name, info.name.c_str(), info.name.length());

    recorder = m_dbExecutor->Query("INSERT INTO `player` SET"
        "`account_name`='%s',"
        "`player_name`='%s',"
        "`player_id`=%llu,"
        "`platform_uid`='%s',"
        "`platform_uname`='%s',"
        "`platform`='%s',"
        "`platform_server_name`='%s',"
        "`region`=%d,"
        "`sex`=%d,"
        "`career`=%d,"
        "`level`=%d,"
        "`exp`=%d,"
        "`money_point`=%d,"
        "`game_point`=%d,"
        "`vigor`=%d,"
        "`vigor_reftime`=%d,"
        "`hp`=%d,"
        "`mp`=%d,"
        "`unionmap_id`=%u,"
        "`x`=%d,"
        "`y`=%d,"
        "`angle`=%d,"
        "`create_time`=CURRENT_TIMESTAMP"
        ";", 
        account,
        name,
        playerID,
        uid.c_str(),
        uname.c_str(),
        platform.c_str(),
        info.platform_server_name.c_str(), 
        info.region,
        info.sex,
        info.career, 
        info.level, 
        info.exp,
        info.money_point, 
        info.game_point,
        info.vigor,
        info.vigor_reftime,
        info.hp, 
        info.mp, 
        info.unionmap_id, 
        info.x, 
        info.y, 
        info.angle);

    if(!recorder.HasExecut())
    {
        CnInfo("create player failed, account=%s, by same name\n", info.account.c_str());
        return false;
    }

    return true;
}

bool PlayerInfoJob::CreateHeadPortrait(uint64 playerID, const PROPERTY_INFO& propInfo)
{
    // 创角以后，自动分配默认头像
    HEAD_PORTRAIT_INFO info;

    info.cur = 19000 + propInfo.sex;
    info.open_list.push_back(info.cur);
    info.open_list.push_back(19003);
    info.open_list.push_back(19004);
    info.open_list.push_back(19005);
    info.open_list.push_back(19006);

    char sqlTxt[MAX_SQLTXT_LEN] = {0};
    std::string head_str("");
    DBRecorder recorder;

    head_str = StructToJson(info);
    m_dbExecutor->EscapeString(sqlTxt, head_str.c_str(), head_str.length());

    recorder = m_dbExecutor->Query(
        "INSERT INTO `head_portrait` VALUES(%llu, '%s') "
        "ON DUPLICATE KEY UPDATE `head_portrait_open`='%s'",
        playerID, sqlTxt, sqlTxt);

    if(!recorder.HasExecut())
    {
        return false;
    }
    return true;
}

// ==================================================
// 充值返还
// ==================================================
bool PlayerInfoJob::RechargeReturn(uint64 playerID, const std::string& uname)
{
    DBRecorder recorder;
    recorder =  m_dbExecutor->Query("UPDATE `player` AS T1, `recharge_return` AS T2 "
        "SET T1.`money_point`=T2.`money_point`, "
        "T2.`flag`=1, T2.`player_id`=%llu, T2.`use_time`=CURRENT_TIMESTAMP() WHERE "
        "T2.`platform_uname`='%s' AND T2.`flag`=0 AND T1.`platform_uname` = T2.`platform_uname`", 
        playerID, uname.c_str());

    if(!recorder.HasExecut())
    {
        return false;
    }
    return true;
}

// ==================================================
// 创建角色副本信息
// ==================================================
bool PlayerInfoJob::CreatePlayerDungeonInfo(uint64 playerID)
{
    DBRecorder recorder;
    recorder =  m_dbExecutor->Query("INSERT INTO `player_dungeon_info` SET `player_id`=%llu, `plot_dungeon_info`='{}', `soul_dungeon_info`='{}'", playerID);
    if(!recorder.HasExecut())
    {
        return false;
    }
    return true;
}

// ==================================================
// 创建角色离线信息
// ==================================================
bool PlayerInfoJob::CreateOfflinePlayerInfo(uint64 playerID, const PROPERTY_INFO& propInfo)
{
    DBRecorder recorder;

    Json::FastWriter writer;
    Json::Value equip_json;
    Json::Value equip;
    Json::Value target;
    std::string battle_power_txt;
    uint32 task_num = 0;
    uint32 target_num = 0;
    char name[PLAYER_NAME_LEN] = {0};
    char equipTxt[MAX_SQLTXT_LEN] = {0};
    char battlePowerTxt[MAX_SQLTXT_LEN] = {0};

    m_dbExecutor->EscapeString(name, propInfo.name.c_str(), propInfo.name.length());
    //m_dbExecutor->EscapeString(equipTxt, equipInfo.c_str(), equipInfo.length());
    //m_dbExecutor->EscapeString(battlePowerTxt, battle_power_txt.c_str(), battle_power_txt.length());

    recorder =  m_dbExecutor->Query(
        "INSERT INTO `offline_player` SET `player_id`=%llu, `player_name`='%s', "
        "`sex`=%d, `career`=%d, `battle_power`=%d, "
        "`level`=%d, `vip_level`=%d, `exp`=%d, `max_exp`=%d, `guild_name`='%s', "
        "`equip_info`='%s', `wing`=%d, `battle_power_info`='%s'", 
        playerID, name, 
        propInfo.sex, propInfo.career, 0, 
        propInfo.level, 0, 0, 0, "0",
        "{}", 0, "{}"
        );
    if(!recorder.HasExecut())
    {
        return false;
    }
    return true;
}

// ==================================================
// 创建离线排行榜信息
// ==================================================
bool PlayerInfoJob::CreateOfflineRankingInfo(uint64 playerID)
{
    DBRecorder recorder;
    // 阻塞操作，防止多线程查询锁表
    recorder = m_dbExecutor->Query("INSERT INTO ranking_offline_player SELECT COUNT(rank)+1,COUNT(rank)+1,%llu,'0','1','1','','0','' FROM ranking_offline_player", playerID);
    if(!recorder.HasExecut())
    {
        CnError("PlayerID:%llu, Create ranking_offline_player failed\n", playerID);
        return false;
    }

    return true;
}

bool PlayerInfoJob::CreateItem(uint64 playerID )
{
    DBRecorder recorder;
    recorder =  m_dbExecutor->Query("INSERT INTO `item` SET `player_id`=%llu, `item_info`='{}' ON DUPLICATE KEY UPDATE `item_info`='{}';", playerID);
    if(!recorder.HasExecut())
    {
        return false;
    }
    return true;
}

bool PlayerInfoJob::CreateSkill(uint64 playerID)
{
    DBRecorder recorder;
    //char skill_txt[MAX_SQLTXT_LEN] = {0};
    //std::string skill_str("");

    //ITEM_STONE item_stone;
    //item_stone.base.id = 17001;
    //item_stone.base.pos = SKILL_SLOT_ML - 1;
    //item_stone.level = 1;
    //item_stone.exp = 0;

    //SKILL_COMPONENT_INFO skill_info;
    //skill_info.list.push_back(item_stone);

    //skill_str = StructToJson(skill_info);
    //m_dbExecutor->EscapeString(skill_txt, skill_str.c_str(), skill_str.length());

    recorder =  m_dbExecutor->Query("INSERT INTO `skill` SET `player_id`=%llu, `skill_info`='{}' ON DUPLICATE KEY UPDATE `skill_info`='{}';", playerID);
    if(!recorder.HasExecut())
    {
        return false;
    }
    return true;
}

bool PlayerInfoJob::CreatePet(uint64 playerID)
{
    DBRecorder recorder;
    recorder =  m_dbExecutor->Query("INSERT INTO `pet` SET `player_id`=%llu, `mercenary_info`='{}' ON DUPLICATE KEY UPDATE `mercenary_info`='{}';", playerID);
    if(!recorder.HasExecut())
    {
        return false;
    }
    return true;
}

