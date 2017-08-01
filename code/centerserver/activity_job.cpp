#include "centerserver_pch.h"
#include "activity_job.h"
#include "dbdriver.h"
#include "player_job.h"
#include "player.h"
#include "player_manager.h"
#include "player_net_handler.h"
#include "centerserver.h"
#include "gate_handler.h"
#include "game_handler.h"
#include "player_config.h"


IMPLEMENT_SINGLETON(ActivityJob)

// ==================================================
// 获取冒险者信息
// ==================================================
void ActivityJob::GetAdventurerList(uint64 playerID)
{
    // 获得冒险者信息（在查询的时候多查找一个，如果发现自己，结果集中剔除）
    m_dbExecutor->Query(this, &ActivityJob::CBGetAdventurerList, playerID,
        "SELECT `player_id`, `player_name`, `guild_name`, `map_id`, `rob_times`, "
        "`rob_gamepoint`, `take_time`, `end_time` FROM `adventure` ORDER BY rand() LIMIT %d", MAX_ADVENTURE_LIST_NUM + 1);
}

// ==================================================
// 获得冒险者资源
// ==================================================
void ActivityJob::RobAdventurer(uint64 playerID, uint64 roberID)
{
    m_dbExecutor->Query(this, &ActivityJob::CBRobAdventurer, playerID, roberID, 
        "SELECT `rob_gamepoint` FROM `adventure` WHERE `player_id`=%llu AND `end_time`<CURRENT_TIMESTAMP AND rob_times>0", roberID);
}

// ==================================================
// 更新冒险信息
// ==================================================
void ActivityJob::UpdateAdventure(uint64 playerID, ADVENTURE_ADD_REQ adventureAdd)
{
    char player_name[PLAYER_NAME_LEN*2+1] = {0};
    char guild_name[GUILD_NAME_LEN*2+1] = {0};

    m_dbExecutor->EscapeString(player_name, adventureAdd.player_name.c_str(), adventureAdd.player_name.length());
    m_dbExecutor->EscapeString(guild_name, adventureAdd.guild_name.c_str(), adventureAdd.guild_name.length());

    // 更新冒险信息
    m_dbExecutor->Query(this, &ActivityJob::CBUpdateAdventure, playerID,
        "INSERT INTO `adventure` VALUES('%llu', '%s', '%s', '%u', '%d', '%d', '%u', '%u') "
        "ON DUPLICATE KEY UPDATE `player_name`='%s', `guild_name`='%s', map_id=%u, ",
        "rob_times=%d, rob_gamepoint=%d, take_time=%u, ",
        playerID, player_name, guild_name, adventureAdd.map_id, 
        adventureAdd.rob_times, adventureAdd.rob_gamepoint, 
        adventureAdd.take_time, adventureAdd.end_time,
        // ------------------------------------------
        player_name, guild_name, adventureAdd.map_id, 
        adventureAdd.rob_times, adventureAdd.rob_gamepoint, 
        adventureAdd.take_time, adventureAdd.end_time);
}

// ==================================================
// 获得冒险列表回调
// ==================================================
void ActivityJob::CBGetAdventurerList(DBRecorder& res, uint64 playerID)
{
    ADVENTURER_INFO_LIST sendmsg;
    ADVENTURER_INFO adventurerInfo;
    uint64 adventurerID = 0;
    Player* player = 0;

    player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    if(!player)
    {
        return;
    }

    for(uint32 i=0; (i<res.GetRowNum()) && (i<MAX_ADVENTURE_LIST_NUM); ++i)
    {
        adventurerInfo.Clear();

        res.GetRecord(adventurerID);
        adventurerInfo.player_id_h = U64ID_HIGH(adventurerID);
        adventurerInfo.player_id_l = U64ID_LOW(adventurerID);

        res.GetRecord(adventurerInfo.player_name);
        res.GetRecord(adventurerInfo.guild_name);
        res.GetRecord(adventurerInfo.map_id);
        res.GetRecord(adventurerInfo.rob_times);
        res.GetRecord(adventurerInfo.rob_gamepoint);
        res.GetRecord(adventurerInfo.take_time);
        res.GetRecord(adventurerInfo.end_time);

        if(adventurerID == playerID) continue;

        sendmsg.list.push_back(adventurerInfo);
    }

    player->SendToGate(MSG_ADVENTURER_LIST_ACK, &sendmsg);
}

// ==================================================
// 获得冒险者资源回调
// ==================================================
void ActivityJob::CBRobAdventurer(DBRecorder& res, uint64 playerID, uint64 roberID)
{
    int32 gamepoint = 0;

    if(res.GetRowNum() < 1)
    {
        return;
    }

    res.GetRecord(gamepoint);

    // 这个地方并不严谨，也未特意强调查询和操作数据的原子性（回调形式）
    m_dbExecutor->Query(this, &ActivityJob::CBSubRobTimes, playerID, roberID, gamepoint, 
        "UPDATE `adventure` SET `rob_times`=`rob_times`-1 WHERE `player_id`=%llu AND "
        "`end_time`<CURRENT_TIMESTAMP AND rob_times>0", roberID);
}

// ==================================================
// 减少掠夺次数回调
// ==================================================
void ActivityJob::CBSubRobTimes(DBRecorder& res, uint64 playerID, uint64 roberID, int32 gamepoint)
{
    ADVENTURE_ROB_ACK sendmsg;
    Player* player = 0;

    // 获得角色指针
    player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    if(!player)
    {
        return;
    }

    // 打劫获得的游戏币
    sendmsg.errcode = gamepoint;
    if(res.GetAffNum() < 1)
    {
        sendmsg.errcode = ERR_FAILED;
    }
    else
    {
        sendmsg.errcode = ERR_SUCCEED;
    }

    player->SendToGame(MSG_ADVENTURE_ROB_ACK, &sendmsg);
}

// ==================================================
// 更新冒险信息回调
// ==================================================
void ActivityJob::CBUpdateAdventure(DBRecorder& res, uint64 playerID)
{

}
