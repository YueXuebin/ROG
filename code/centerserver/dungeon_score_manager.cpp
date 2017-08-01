#include "centerserver_pch.h"
#include "dungeon_score_manager.h"
#include "dungeon_job.h"
#include "player_manager.h"
#include "player.h"
#include "game_handler.h"
#include "config_loader.h"


IMPLEMENT_SINGLETON(DungeonScoreManager)

DungeonScoreManager::DungeonScoreManager()
{
    Clear();
}

DungeonScoreManager::~DungeonScoreManager()
{
    
}

void DungeonScoreManager::Clear()
{
    m_dungeonScoreMap.clear();
}

bool DungeonScoreManager::Init()
{
    DungeonJob::Instance()->LoadDungeonScoreInfo(m_dungeonScoreMap);

    return true;
}

void DungeonScoreManager::OnRecv(uint64 playerID, uint32 sessionID, uint32 msgID, void* data)
{
    switch(msgID)
    {
    case MSG_SAVE_DUNGEON_SCORE_INFO:
        OnSaveDungeonInfo(playerID, data);
        break;
    case MSG_DUNGEON_SCORE_INFO_REQ:
        OnDungeonScoreInfoReq(playerID, data);
        break;
    case MSG_GS_DUNGEON_SCORE_INFO_REQ:
        OnCrossTeamDungeonScoreInfoReq(sessionID, data);
        break;
    default:
        CnAssert(false);
        break;
    }
}

// 保存副本信息
void DungeonScoreManager::OnSaveDungeonInfo(uint64 playerID, void* data)
{
    DUNGEON_SCORE_INFO* recvMsg = (DUNGEON_SCORE_INFO*)data;
    if(!recvMsg)
        return;

    // 按副本号查找副本通关信息
    DungeonScoreMap::iterator dungeonIt = m_dungeonScoreMap.find(recvMsg->dungeon_id);
    if(dungeonIt != m_dungeonScoreMap.end())
    {
        // 检查花费的时间
        if(dungeonIt->second.cost_time < recvMsg->cost_time)
        {
            return;
        }
        dungeonIt->second = *recvMsg;
    }
    else
    {
        m_dungeonScoreMap[recvMsg->dungeon_id] = *recvMsg;
    }

    // 保存副本通关信息
    DungeonJob::Instance()->SaveDungeonScoreInfo(*recvMsg);
}

// 请求副本分数信息
void DungeonScoreManager::OnDungeonScoreInfoReq(uint64 playerID, void* data)
{
    DUNGEON_SCORE_INFO_LIST sendMsg;

    Player* player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    if(!player)
    {
        return;
    }

    for(DungeonScoreMap::iterator it = m_dungeonScoreMap.begin(); it != m_dungeonScoreMap.end(); ++it)
    {
        sendMsg.dungeon_list.push_back(it->second);
    }

    player->SendToGate(MSG_DUNGEON_SCORE_INFO_ACK, &sendMsg);
}

void DungeonScoreManager::OnCrossTeamDungeonScoreInfoReq(uint32 sessionID, void* data)
{
    uint32 regionID = GameHandler::Instance()->GetRegionIDBySessionID(sessionID);
    if(!regionID)
        return;

    CROSS_TEAM_DUNGEON_SCORE_INFO_REQ* req = (CROSS_TEAM_DUNGEON_SCORE_INFO_REQ*)data;

    CROSS_TEAM_DUNGEON_SCORE_INFO_LIST sendMsg;
    sendMsg.gatesvr_id = req->gatesvr_id;
    sendMsg.player_id_h = req->player_id_h;
    sendMsg.player_id_l = req->player_id_l;
    for(DungeonScoreMap::iterator it = m_dungeonScoreMap.begin(); it != m_dungeonScoreMap.end(); ++it)
    {
        sendMsg.dungeon_list.push_back(it->second);
    }

    GameHandler::Instance()->SendByID(regionID, MSG_CROSS_TEAM_DUNGEON_SCORE_INFO_ACK, &sendMsg);
}

