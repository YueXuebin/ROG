#include "centerserver_pch.h"
#include "player_job.h"
#include "dbdriver.h"
#include "mail_job.h"
#include "player.h"
#include "player_manager.h"
#include "player_net_handler.h"
#include "centerserver.h"
#include "gate_handler.h"
#include "game_handler.h"
#include "player_config.h"
#include "dungeon_manager.h"
#include "config_loader.h"
#include "guild_manager.h"
#include "kunlun_log.h"
#include "json_helper.h"
#include "festival_job.h"
#include "dongyi_log.h"
#include "log_writer.h"
#include "dg_login.h"
#include "kunlun_login.h"
#include "player_save_job.h"
#include "player_info_job.h"

IMPLEMENT_SINGLETON(PlayerJob)

// 这几个BUFF只会在阻塞的SQL线程中使用
char PlayerJob::m_dungeon_buff[MAX_SQLTXT_LEN] = {0};
char PlayerJob::m_vip_buff[MAX_SQLTXT_LEN] = {0};
char PlayerJob::m_open_system_buff[MAX_SQLTXT_LEN] = {0};
char PlayerJob::m_guild_welfare_buff[MAX_SQLTXT_LEN] = {0};
char PlayerJob::m_city_fight_buff[MAX_SQLTXT_LEN] = {0};
char PlayerJob::m_fcm_buff[MAX_SQLTXT_LEN] = {0};
char PlayerJob::m_vigor_buff[MAX_SQLTXT_LEN] = {0};


// ==================================================
// 加载账号数据
// ==================================================
void PlayerJob::LoginLoadAfAccount(uint32 gateSvrID, uint64 loginID, const std::string& token, uint32 addrIP, uint16 addrPort)
{
    char token_str[PLAYER_TOKEN_LEN*2+1] = {0};

    m_dbExecutor->EscapeString(token_str, token.c_str(), token.length());

    m_dbExecutor->Query(this, &PlayerJob::CBLoginLoadAfAccount, gateSvrID, loginID, addrIP, addrPort, 
        "SELECT `account` FROM `dg_account` WHERE `token`='%s'", token_str);
}

// ==================================================
// 加载账号数据
// ==================================================
void PlayerJob::LoginLoadKLAccount(uint32 gateSvrID, uint64 loginID, std::string& uid, std::string& uname, uint32 addrIP, uint16 addrPort)
{
    char account_str[PLAYER_ACCOUNT_LEN*2+1] = {0};

    m_dbExecutor->EscapeString(account_str, uid.c_str(), uid.length());

    m_dbExecutor->Query(this, &PlayerJob::CBLoginLoadKlAccount, gateSvrID, loginID, uid, uname, addrIP, addrPort, 
        "SELECT `account` FROM `kunlun_account` WHERE `account`='%s'", account_str);
}
// 创建角色数据
void PlayerJob::CreatePlayer(uint32 gateSvrID, uint64 loginID, const std::string& account, const std::string& name, 
                                 const std::string& uid, const std::string& uname, const std::string& platform, const std::string& platform_server_name,
                                 int32 region_id, uint8 sex, uint8 career)
{
    uint64 playerID = 0;
    PROPERTY_INFO info;

    info.Clear();

    playerID = PlayerManager::Instance()->GenPlayerUUID();      // 生成唯一player id

    info.account = account;
    info.name = name;
    info.sex = sex;
    info.career = career;
    info.level = 1;
    info.platform_server_name = platform_server_name;
    info.region = region_id;

    // 性别错误
    if((sex < SEX_MALE) || (sex > SEX_FEMALE))
    {
        GateHandler::Instance()->SendCreatePlayerAck(ERR_CREATE_PLAYER_FAILED, gateSvrID, loginID);
        return;
    }

    // 职业错误
    if((career < CAREER_WARRIOR) || (career > CAREER_ARCHER))
    {
        GateHandler::Instance()->SendCreatePlayerAck(ERR_CREATE_PLAYER_FAILED, gateSvrID, loginID);
        return;
    }

    // 设置角色出生属性
    PlayerConfig::Instance()->FillCreateProperyInfo(info);

    // 在数据库中创建玩家数据
    PlayerInfoJob::Instance()->CreatePlayerInfo(gateSvrID, loginID, playerID, uid, uname, platform, info);
}
// 查询角色信息
void PlayerJob::AskPlayerInfo(uint32 gateSvrID,  uint64 loginID, const std::string& account, const std::string& platform_server_name)
{
    char account_str[PLAYER_TOKEN_LEN*2+1] = {0};
    m_dbExecutor->EscapeString(account_str, account.c_str(), account.length());

    char server_name_str[PLAYER_TOKEN_LEN*2+1] = {0};
    m_dbExecutor->EscapeString(server_name_str, platform_server_name.c_str(), platform_server_name.length());

    m_dbExecutor->Query(this, &PlayerJob::CBAskPlayerInfo, gateSvrID, loginID, 
        "SELECT `account_name`, `player_name`, `player_id`, `platform_server_name`, `region`, `sex`, `career`, `level`, `unionmap_id`"
        "FROM player where `account_name`='%s' AND `platform_server_name`='%s'", account_str, server_name_str);     // 通过账号和服务器名定位角色
}

void PlayerJob::CBAskPlayerInfo(DBRecorder& res, uint32 gateSvrID, uint64 loginID)
{
    ASK_PLAYER_LIST playerList;
    if(res.HasRecord())
    {
        ASK_PLAYER_INFO playerInfo;
        std::string account;
        res.GetRecord(account);
        res.GetRecord(playerInfo.player_name);
        uint64 player_id;
        res.GetRecord(player_id);
        playerInfo.player_id.id_h = U64ID_HIGH(player_id);
        playerInfo.player_id.id_l = U64ID_LOW(player_id);
        res.GetRecord(playerInfo.platform_server_name);
        res.GetRecord(playerInfo.region);
        res.GetRecord(playerInfo.sex);
        res.GetRecord(playerInfo.career);
        res.GetRecord(playerInfo.level);
        res.GetRecord(playerInfo.unionmap_id);
        playerList.character_list.push_back(playerInfo);
    }
    GateHandler::Instance()->SendAskPlayerAck(playerList, gateSvrID, loginID);
}

void PlayerJob::ChangePlayerName(uint64 playerID, std::string& name)
{
    char name_str[PLAYER_NAME_LEN*2+1] = {0};

    m_dbExecutor->EscapeString(name_str, name.c_str(), name.length());

    m_dbExecutor->Query(this, &PlayerJob::CBChangePlayerName, playerID, name, 
        "UPDATE `player` SET `player_name`='%s' WHERE `player_id`=%llu", name_str, playerID);
}

// ==================================================
// 自动回存角色属性到DB
// ==================================================
void PlayerJob::AutoSavePlayerDataToDB(Player* player)
{
    bool SaveToDBOK = PlayerSaveJob::Instance()->SavePlayerDataToDB(player);
    if(SaveToDBOK)
    {
        CnDbg("auto save player %s succeed!\n", player->GetName().c_str());
    }
    else
    {
        CnDbg("auto save player %s failed!\n", player->GetName().c_str());
    }
}
// ==================================================
// 保存角色数据,并销毁角色
// ==================================================
void PlayerJob::SavePlayerDataAndDestory(Player* player)
{
    if(!player) return;
    CnInfo("save player account=%s name=%s\n", player->GetAccount().c_str(), player->GetName().c_str());

    // 设置登出时间
    player->SetlogoutTime((uint32)Crown::SDNow().GetTimeValue());

    bool SaveToDBOK = PlayerSaveJob::Instance()->SavePlayerDataToDB(player);
    if(SaveToDBOK)
    {
        // 记录统计信息
        RecordStatistic(player);
    }
    else
    {
        CnError("save player %s failed\n", player->GetName().c_str());
    }

    // 角色销毁处理
    player->OnDestory();

    // 删除角色
    PlayerManager::Instance()->DelPlayer(player->GetPlayerID());
}

bool PlayerJob::FindPlayerNameIsExit(const std::string& playername)
{
    DBRecorder recorder;
    char sqlTxt[MAX_SQLTXT_LEN] = {0};
    std::string curdata_txt;

    Json::FastWriter writer;
    curdata_txt = writer.write(playername);
    m_dbExecutor->EscapeString(sqlTxt, curdata_txt.c_str(), curdata_txt.length());

    recorder = m_dbExecutor->Query("SELECT `player_name` FROM `player`  where `player_name`='%s'",playername.c_str(), sqlTxt);
    if(!recorder.HasExecut())
    {
        return false;
    }

    if(recorder.GetRowNum() > 0)
    {
        return true;
    }

    return false;
}
void PlayerJob::AddFriend(uint64 player_id_a, uint64 player_id_b)
{
    m_dbExecutor->Query(this, &PlayerJob::CBAddFriend,
        "INSERT INTO `friend` SET `player_id_a`=%llu, `player_id_b`=%llu", player_id_a, player_id_b);
}

// ==================================================
// 删除好友
// ==================================================
void PlayerJob::DelFriend(uint64 player_id_a, uint64 player_id_b)
{
    // 由于是单向加好与，所以这里，一方删除，另外一方不会也删除对方
    m_dbExecutor->Query(this, &PlayerJob::CBDelFriend,
        "DELETE FROM friend where `player_id_a`=%llu AND `player_id_b`=%llu", player_id_a, player_id_b);
   /* m_dbExecutor->Query(this, &PlayerJob::CBDelFriend,
        "DELETE FROM friend where `player_id_a`=%llu AND `player_id_b`=%llu "
        "OR `player_id_a`=%llu AND `player_id_b`=%llu", player_id_a, player_id_b, player_id_b, player_id_a);*/
}

// ==================================================
// 获得离线玩家数据
// ==================================================
void PlayerJob::RequestOfflinePlayerRecord(uint64 playerID, uint32 GameServerID, uint8 reason)
{
    m_dbExecutor->Query(this, &PlayerJob::CBRequestOfflinePlayerRecord, GameServerID, reason, 
        "SELECT `player_id`, `player_name`, `sex`, `career`, `battle_power`, `vip_level`, "
        "`attack`, `fire_up`, `ice_up`, `thunder_up`, "
        "`defence`, `fire_defence`, `ice_defence`, `thunder_defence`, `attack_speed`, `move_speed`, `critical_attack`, `cri_att_red`, `cri_att_damage_add`, `cri_att_damage_red`, "
        "`level`, `exp`, `max_exp`, `strength`, `agility`, `spirit`, `constitution`, "
        "`hit_rate`, `dodge`, `consume_mp`, `damage_reduction`, `res_hp`, `res_mp`, "
        "`max_hp`, `max_mp`, `equip_info`, `battle_power_info`, `wing`, `fashion_id`, `guild_name`"
        "FROM offline_player WHERE `player_id`=%llu", playerID);
}

void PlayerJob::RequestOfflinePlayerRecordByName(const std::string& playerName, uint32 GameServerID, uint8 reason)
{
    char name[PLAYER_NAME_LEN*2+1] = {0};
    m_dbExecutor->EscapeString(name, playerName.c_str(), playerName.length());

    m_dbExecutor->Query(this, &PlayerJob::CBRequestOfflinePlayerRecord, GameServerID, reason, 
        "SELECT `player_id`, `player_name`, `sex`, `career`, `battle_power`, `vip_level`, "
        "`attack`, `fire_up`, `ice_up`, `thunder_up`, "
        "`defence`, `fire_defence`, `ice_defence`, `thunder_defence`, `attack_speed`, `move_speed`, `critical_attack`, `cri_att_red`, `cri_att_damage_add`, `cri_att_damage_red`, "
        "`level`, `exp`, `max_exp`, `strength`, `agility`, `spirit`, `constitution`, "
        "`hit_rate`, `dodge`, `consume_mp`, `damage_reduction`, `res_hp`, `res_mp`, "
        "`max_hp`, `max_mp`, `equip_info`, `battle_power_info`, `wing`, `fashion_id`, `guild_name`"
        "FROM offline_player WHERE `player_name`='%s'", name);
}
// ==================================================
// 获得答题前10排名
// ==================================================
void PlayerJob::GetAnswerTopRank(uint8 isend)
{
    m_dbExecutor->Query(this, &PlayerJob::CBGetAnswerTopRank, isend, 
        "SELECT `player_name`, `score` FROM `answer_rank` WHERE `score`>'%d' ORDER BY `rank` ASC LIMIT %d", 0, ANSWER_TOPRANK_NUM);
}

// ==================================================
// 获得个人答题排名
// ==================================================
void PlayerJob::GetAnswerPlayerRank(uint64 playerID, uint32 msgID)
{
    m_dbExecutor->Query(this, &PlayerJob::CBGetAnswerPlayerRank, playerID, msgID, 
        "SELECT `rank` FROM `answer_rank` WHERE `player_id`=%llu AND `score`>'%d'", playerID, 0);
}

// ==================================================
// 保存答题个人信息
// ==================================================
void PlayerJob::SaveAnswerScore(uint64 playerID, std::string player_name, int32 score)
{
    char name[PLAYER_NAME_LEN*2+1] = {0};

    m_dbExecutor->EscapeString(name, player_name.c_str(), player_name.length());

    m_dbExecutor->Query(this, &PlayerJob::CBSaveAnswerScore, 
        "INSERT INTO `answer_rank` VALUES(%llu, '%s', %d, 0) "
        "ON DUPLICATE KEY UPDATE `score`=%d", playerID, name, score, score);
}
void PlayerJob::AddMoneyPoint(std::string account, int32 moneyPoint)
{
    char account_str[PLAYER_ACCOUNT_LEN*2+1] = {0};
    m_dbExecutor->EscapeString(account_str, account.c_str(), account.length());
    m_dbExecutor->Query(this, &PlayerJob::CBAddMoneyPoint, "UPDATE `player` SET `money_point`=`money_point`+%d WHERE `account_name`='%s'", moneyPoint, account_str);
}

// ==================================================
// 回调添加魔石
// ==================================================
void PlayerJob::CBAddMoneyPoint(DBRecorder& res)
{

}

// ==================================================
// 减少魔石
// ==================================================
void PlayerJob::SubMoneyPoint(std::string account, int32 moneyPoint)
{
    char account_str[PLAYER_ACCOUNT_LEN*2+1] = {0};
    m_dbExecutor->EscapeString(account_str, account.c_str(), account.length());
    m_dbExecutor->Query(this, &PlayerJob::CBSubMoneyPoint, 
        "UPDATE `player` SET `money_point`=IF(`money_point`>%d,`money_point`-%d,0) WHERE `account_name`='%s'", moneyPoint, moneyPoint, account_str);
}

// ==================================================
// 回调减少魔石
// ==================================================
void PlayerJob::CBSubMoneyPoint(DBRecorder& res)
{

}

// ==================================================
// 记录统计信息
// ==================================================
void PlayerJob::RecordStatistic(Player* player)
{
    if(!player) return;

    RecordPlayerOnlineTime(player->GetPlayerID(), player->GetLoginTime(), player->GetLogoutTime());
}

// ==================================================
// 记录角色在线时长
// ==================================================
void PlayerJob::RecordPlayerOnlineTime(uint64 playerID, uint32 beginTime, uint32 endTime)
{
    //m_dbExecutor->Query(this, &PlayerJob::CBRecordStatistic,
    //    "INSERT INTO `statistic_player_online` SET `player_id`=%llu, `login_time`=%u, `logout_time`=%u",
    //    playerID, beginTime, endTime);
}

// ==================================================
// 记录角色在线时长
// ==================================================
void PlayerJob::CBRecordStatistic(DBRecorder& res)
{
}

// ==================================================
// 更改角色名称
// ==================================================
void PlayerJob::CBChangePlayerName(DBRecorder& res, uint64 playerID, std::string name)
{
    Player* player = 0;

    player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    if(!player) return;

    if(!res.HasExecut())
    {
        GameHandler::Instance()->SendChangePlayerNameAck(player->GetCentGameID(), playerID, ERR_DUPLICATE_PLAYER_NAME, name);
        return;
    }

    PlayerManager::Instance()->ChangePlayerName(playerID, name);

    GameHandler::Instance()->SendChangePlayerNameAck(player->GetCentGameID(), playerID, ERR_SUCCEED, name);
}

// ==================================================
// 获取账号信息回调
// ==================================================
void PlayerJob::CBLoginLoadAfAccount(DBRecorder& res, uint32 gateSvrID, uint64 loginID, uint32 addrIP, uint16 addrPort)
{
    std::string account;

    // 无此账号
    if(!res.HasRecord() || (res.GetRowNum() < 1))
    {
        CnDbg("ip=%s port=%d no account\n", Crown::GetIpStr(addrIP).c_str(), addrPort);
        GateHandler::Instance()->SendLoginGameAck(ERR_UNFOUND_PLAYER, gateSvrID, loginID);
        return;
    }

    // 读取账户信息
    res.GetRecord(account);

    // 账号转小写
    CommonLib::ToLower(account);

    OnVerifyAfModeLogin(gateSvrID, loginID, account, addrIP, addrPort);
}

// ==================================================
// 获取账号信息回调
// ==================================================
void PlayerJob::CBLoginLoadKlAccount(DBRecorder& res, uint32 gateSvrID, uint64 loginID, 
                                     std::string uid, std::string uname, uint32 addrIP, uint16 addrPort)
{
    std::string account;

    // 无此账号
    if(!res.HasRecord() || (res.GetRowNum() < 1))
    {
        //CnDbg("ip=%s port=%d no active kunlun account\n", Crown::GetIpStr(addrIP).c_str(), addrPort);
        GateHandler::Instance()->SendLoginGameAck(ERR_ACTIVE_NOT_ACTIVE_LOGIN, gateSvrID, loginID);
        return;
    }

    // 读取账户信息
    res.GetRecord(account);

    KlLoginExecutor::Instance()->OnVerifyKunlunCdkeyModeLoginCB(gateSvrID, loginID, account, addrIP, addrPort, uid, uname);
}

// ==================================================
// 删除角色
// ==================================================
void PlayerJob::CBDeletePlayerInfo(DBRecorder& res)
{

}
void PlayerJob::ParseOfflinePlayerInfo(DBRecorder& db, OFFLINE_PLAYER_INFO& player_info)
{
    Json::Reader reader;
    Json::Value equip_json;
    Json::Value battle_power_json;
    std::string equip_info_str;
    std::string battle_power_str;

    OFFLINE_EQUIP_INFO      equip_record;
    player_info.Clear();

    equip_json.clear();
    equip_info_str.clear();
    battle_power_str.c_str();

    CnAssert(db.GetCol() == 0);

    uint64 player_uuid = 0;
    db.GetRecord(player_uuid);
    player_info.player_id_l = U64ID_LOW(player_uuid);
    player_info.player_id_h = U64ID_HIGH(player_uuid);

    db.GetRecord(player_info.name);
    db.GetRecord(player_info.sex);
    db.GetRecord(player_info.career);
    db.GetRecord(player_info.battle_power);
    db.GetRecord(player_info.vip);


    db.GetRecord(player_info.level);
    db.GetRecord(player_info.exp);
    db.GetRecord(player_info.max_exp);

    db.GetRecord(equip_info_str);
    db.GetRecord(battle_power_str);
    uint32 wing = 0;
    db.GetRecord(wing);
    player_info.wing = wing;

    db.GetRecord(player_info.guild_name);

    /*if(!reader.parse(equip_info_str, equip_json, false))
    {
        CnError("Parse player:%llu offline player equip info failed\n", player_uuid);
        return;
    }

    uint32 player_info_equip_count = equip_json["equip"].size();
    for(uint32 i=0; i<player_info_equip_count; ++i)
    {
        read_OFFLINE_EQUIP_INFO(equip_record, equip_json["equip"][i]);
        player_info.equip_list.push_back(equip_record);
    }

    if(battle_power_str.length() > 0)
    {
        if(!reader.parse(battle_power_str, battle_power_json, false))
        {
            CnError("Parse player:%llu offline player battle power info failed\n", player_uuid);
            return;
        }

        read_BATTLE_POWER_INFO(player_info.battle_power_info, battle_power_json);
    }*/
}
void PlayerJob::CBRequestOfflinePlayerRecord(DBRecorder& res, uint32 GameServerID, uint8 reason)
{
    if(res.GetRowNum() == 0)
        return;

    OFFLINE_PLAYER_ACK player_info_ack;
    player_info_ack.reason = reason;
    ParseOfflinePlayerInfo(res, player_info_ack.offline_player_info);

    // 发送离线角色数据
    GameHandler::Instance()->SendByID(GameServerID, MSG_OFFLINE_PLAYER_INFO_ACK, &player_info_ack);
}

// ==================================================
// 添加好友
// ==================================================
void PlayerJob::CBAddFriend(DBRecorder& res)
{

}

// ==================================================
// 删除好友
// ==================================================
void PlayerJob::CBDelFriend(DBRecorder& res)
{

}
// ==================================================
// 获得前TOP答题排行
// ==================================================
void PlayerJob::CBGetAnswerTopRank(DBRecorder& res, uint8 isend)
{
    ANSWER_TOPRANK_INFO sendMsg;
    ANSWER_PLAYER_TOPRANK player_toprank;

    sendMsg.Clear();

    sendMsg.isend = isend;

    for(uint32 i=0; i<res.GetRowNum(); ++i)
    {
        res.GetRecord(player_toprank.player_name);
        res.GetRecord(player_toprank.score);
        sendMsg.list.push_back(player_toprank);
    }

    GameHandler::Instance()->SendToAll(MSG_ANSWER_TOPRANK_ACK, &sendMsg);
}

// ==================================================
// 获得个人答题排行
// ==================================================
void PlayerJob::CBGetAnswerPlayerRank(DBRecorder& res, uint64 playerID, uint32 msgID)
{
    ANSWER_PLAYER_RANK sendMsg;
    Player* player = 0;

    player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    if(!player) return;

    sendMsg.Clear();

    if(res.GetRowNum() > 0)
    {
        res.GetRecord(sendMsg.rank);
    }

    if(msgID == MSG_ANSWER_PLAYER_RANK_REQ)
    {
        player->SendToGate(MSG_ANSWER_PLAYER_RANK_ACK, &sendMsg);
    }
    else if(msgID == MSG_ANSWER_REWARD_REQ)
    {
        player->SendToGame(MSG_ANSWER_REWARD_ACK, &sendMsg);
    }
}

// ==================================================
// 保存个人答题数据
// ==================================================
void PlayerJob::CBSaveAnswerScore(DBRecorder& res)
{

}
