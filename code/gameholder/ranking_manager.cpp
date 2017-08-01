#include "gameholder_pch.h"
#include "ranking_manager.h"
#include "center_handler.h"
#include "player_manager.h"
#include "player_door_manager.h"
#include "void_field_manager.h"
#include "event_define.h"


IMPLEMENT_SINGLETON(RankingManager)

RankingManager::RankingManager()
{
    m_top_player_level_rank_list.Clear();
}

RankingManager::~RankingManager()
{

}

void RankingManager::OnRankingRecv(uint64 playerID, uint32 msgID, void* data)
{
    switch(msgID)
    {
    case MSG_RANKING_INFO_REQ:
        OnRankingInfoReq(data, playerID);
        break;
    case MSG_RANKING_RANK_ACK:
        OnRankingRankAck(data, playerID);
        break;
    case MSG_RANKING_PLAYER_ID_ACK:
        OnRankingPlayerIDAck(data, playerID);
        break;
    case MSG_RANKING_CHANGE_ACK:
        OnRankingChangeAck(data, playerID);
        break;
    case MSG_RANKING_DAILY_REFRESH:
        OnRecvRankDailyRefresh(msgID, data);
        break;
    case MSG_RANKING_TOP_RANK_NTF:
        OnRankingTopPlayerLevelRankNtf(data);
        break;
    case MSG_RANKING_HOURS_FRESH_RANK_NTF:
        OnRecvRankHoursRefresh();
        break;
    default:
        break;
    }
}

void RankingManager::OnRankingInfoReq(void* data, uint64 playerID)
{
    RANKING_INFO_REQ* recvMsg = (RANKING_INFO_REQ*)data;

    RankingInfoReq(playerID, recvMsg->rank_type);
}

void RankingManager::RankingInfoReq(uint64 playerID, uint8 type)
{
    RANKING_PLAYER_ID_REQ sendMsg;
    ERROR_CODE err;
    int rank = 1;

    sendMsg.rank_type = type;

    /*if(page <= 0)
    {
        CnFatal("page: %d is no exist\n", page);
        return;
    }
    else
    {
        page--;
        page *= RANKING_PAGE_LINE_NUM;
    }

    

    for(int32 i = 1; i <= RANKING_PAGE_LINE_NUM; ++i)
    {
        rank = page + i;
        sendMsg.rankList.push_back(rank);
    }*/


    Player* player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    if(!player)
    {
        CnDbg("RankingInfoReq player id=%llu not exist\n", playerID);
        return;
    }

    CenterHandler::Instance()->SendByCenterID(player->GetCenterSvrID(), MSG_RANKING_INFO_NTF, &sendMsg, playerID);
}

void RankingManager::GMRefreshRanking()
{
    RANKING_REFRESH_REQ sendMsg;
    CenterHandler::Instance()->SendToMasterCenter(MSG_GMRANKING_REFRESH_REQ, &sendMsg);
}

std::vector<TOP_RANK_INFO>* RankingManager::GetTopPlayerLevelRankList()
{
    return &m_top_player_level_rank_list.rankList;
}

void RankingManager::OnRankingRankAck(void* data, uint64 playerID)
{
    RANKING_RANK_ACK* recvMsg = (RANKING_RANK_ACK*)data;
    if(!recvMsg)
        return;

    switch(recvMsg->reason)
    {
    case RANKING_SEASON_PLAYER_DOOR_INFO:
        OnRankSeasonPlayerDoorInfo(recvMsg, playerID);
        break;
    case RANKING_TITLE_OFF_LINE_RANK_INFO:
      //  OnRankAshuraTitle(recvMsg, playerID);
        break;
    case RANKING_SEASON_OLD_OFF_LINE_RANK_INFO:
        OnRankSeasonOldOfflineInfo(recvMsg, playerID);
        break;
    case RANKING_TITLE_RANK_INFO:
        OnRankTitleInfo(playerID, recvMsg);
        break;
    default:
        CnDbg("rank reason:%d is not exist!\n", recvMsg->reason);
        break;
    }
}

void RankingManager::OnRankTitleInfo(uint64 playerID, RANKING_RANK_ACK* recvMsg)
{
    if(!recvMsg)
        return;

    Player* player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    if(!player)
        return;
    switch (recvMsg->rank_type)
    {
    case RANKING_TYPE_HOROSCOPE:
        OnSendRankEvent(player, recvMsg, EVENT_HOROSCOPE_RANK_TITLE);
        break;
    case RANKING_TYPE_BATTLE_POWER:
        OnSendRankEvent(player, recvMsg, EVENT_BATTLE_RANK_TITLE);
        break;
    case RANKING_TYPE_GUILD:
        OnSendRankEvent(player, recvMsg, EVENT_GUILD_RANK_TITLE);
        break;
    case RANKING_TYPE_ASHURA:
        OnSendRankEvent(player, recvMsg, EVENT_ASHURA_RANK_TITLE);
        break;
    case RANKING_TYPE_WING:
        OnSendRankEvent(player, recvMsg, EVENT_WING_RANK_TITLE);
        break;
    case RANKING_TYPE_FOLLOW_PET:
        OnSendRankEvent(player, recvMsg, EVENT_PET_RANK_TITLE);
        break;
    case RANKING_TYPE_RIDE_PET:
        OnSendRankEvent(player, recvMsg, EVENT_RIDE_RANK_TITLE);
        break;
    }
}

void RankingManager::OnSendRankEvent(Player* player, RANKING_RANK_ACK* recvMsg, int32 type)
{
    if(!player)
        return;

    if(!recvMsg)
        return;
    for (std::vector<PLAYER_RANK_INFO>::iterator playerIt = recvMsg->playerList.begin(); playerIt != recvMsg->playerList.end(); playerIt++)
    {
        int32 rank = playerIt->rank;
        player->SendMyselfEvent(type, rank, 0, 0);
        break;
    }
}

void RankingManager::OnRankAshuraTitle(RANKING_RANK_ACK* rankAck, uint64 playerID)
{
    if(!rankAck)
    {
        return;
    }

    Player* player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    if(!player)
    {
        return;
    }

    if (rankAck->rank_type == RANKING_TYPE_OFFLINE_BATTLE)
    {
        int32 rank = 0;
        // 根据玩家自己的排名，请求获取离线用户排名列表
        for (std::vector<PLAYER_RANK_INFO>::iterator playerIt = rankAck->playerList.begin(); playerIt != rankAck->playerList.end(); playerIt++)
        {
            rank = playerIt->rank;
            player->SendMyselfEvent(EVENT_OFFLINE_RANK,rank,0,0);
            break;
        }
    }
}

void RankingManager::OnRankSeasonPlayerDoorInfo(RANKING_RANK_ACK* rankAck, uint64 playerID)
{
    if(!rankAck)
    {
        return;
    }

    Player* player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    if(!player)
    {
        return;
    }

    player->GetPlayerDoorManager()->OnPlayerRankByIdAck(rankAck);
}

void RankingManager::OnRankSeasonOldOfflineInfo(RANKING_RANK_ACK* rankAck, uint64 playerID)
{
    if(!rankAck)
    {
        return;
    }

    Player* player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    if(!player)
    {
        return;
    }

    player->GetPlayerDoorManager()->OnPlayerOldRankByIdAck(rankAck);
}

void RankingManager::OnRankingPlayerIDAck(void* data, uint64 playerID)
{
    RANKING_PLAYER_ID_ACK* recvMsg = (RANKING_PLAYER_ID_ACK*)data;

    switch(recvMsg->reason)
    {
    case RANKING_SEASON_PLAYER_DOOR_INFO:
        OnPlayerIDSeasonPlayerDoorInfo(recvMsg, playerID);
        break;
    default:
        CnDbg("rank reason:%d is not exist!\n", recvMsg->reason);
        break;
    }
}

void RankingManager::OnPlayerIDSeasonPlayerDoorInfo(RANKING_PLAYER_ID_ACK* playerIDAck, uint64 playerID)
{
    if(!playerIDAck)
    {
        return;
    }

    Player* player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    if(!player)
    {
        return;
    }

    player->GetPlayerDoorManager()->OnPlayerIdByRankAck(playerIDAck);
}

void RankingManager::OnRankingChangeAck(void* data, uint64 playerID)
{
}

// ==================================================
// 排行榜排名结束通知
// ==================================================
void RankingManager::OnRecvRankDailyRefresh(uint32 msgID, void* data)
{
    // 如果其他地方需求可以在此分流

    // 通知所有玩家排行榜排名结束
    const PlayerManager::PlayerMap& playerList = PlayerManager::Instance()->GetPlayers();
    for(PlayerManager::PlayerMap::const_iterator playerIt = playerList.begin(); playerIt != playerList.end(); ++playerIt)
    {
        playerIt->second->RankingDailyRefresh();
    }
}

//排行榜每小时刷新
void RankingManager::OnRecvRankHoursRefresh()
{
    // 通知所有玩家排行榜排名结束
    const PlayerManager::PlayerMap& playerList = PlayerManager::Instance()->GetPlayers();
    for(PlayerManager::PlayerMap::const_iterator playerIt = playerList.begin(); playerIt != playerList.end(); ++playerIt)
    {
        playerIt->second->RankingHourRefresh();
    }
}


void RankingManager::OnRankingTopPlayerLevelRankNtf(void* data)
{
    RANKING_TOP_RANK_NTF* rankAck = (RANKING_TOP_RANK_NTF*)data;
    if(!rankAck || rankAck->rank_type != RANKING_TYPE_PLAYER_LEVEL)
    {
        return;
    }

    m_top_player_level_rank_list.reason = rankAck->reason;
    m_top_player_level_rank_list.rank_type = rankAck->rank_type;
    m_top_player_level_rank_list.rankList = rankAck->rankList;
}

void RankingManager::OnRankBattlehall( uint64 playerID, RANKING_RANK_ACK* recvMsg )
{

}
