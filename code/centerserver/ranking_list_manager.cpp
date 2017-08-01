#include "centerserver_pch.h"
#include "ranking_list_manager.h"
#include "ranking_loader_job.h"
#include "player.h"
#include "player_manager.h"
#include "game_handler.h"
#include "season_manager.h"
#include "global_db_operation.h"
#include "ranking_loader_job.h"
#include "rank_query_def.h"
#include "top_up_rank_query.h"
#include "festival_job.h"

IMPLEMENT_SINGLETON(RankListManager)

RankListManager::RankListManager()
{
    Clear();
}

RankListManager::~RankListManager()
{
    Clear();
}

void RankListManager::Clear()
{
    RankQueryRegister::iterator i, iEnd = m_rankQueryRegister.end();
    for (i = m_rankQueryRegister.begin(); i != iEnd; ++i)
    {
        CnDelete i->second;
    }
    m_rankQueryRegister.clear();

    m_top_player_level_rank.Clear();
}

void RankListManager::Init()
{
    // TODO: 初始化时从数据库拉取top榜信息,查询量待定
    OnTopPlayerLevelRankReq(20);

    RankingLoaderJob::Instance()->OnGetRefreshTime();
}

void RankListManager::Update()
{
    RankQueryRegister::iterator i, iEnd = m_rankQueryRegister.end();
    for (i = m_rankQueryRegister.begin(); i != iEnd; ++i)
    {
        i->second->Update();
    }
}

void RankListManager::DailyRefresh()
{
    RankQueryRegister::iterator i, iEnd = m_rankQueryRegister.end();
    for (i = m_rankQueryRegister.begin(); i != iEnd; ++i)
    {
        i->second->DailyRefresh();
    }
}

void RankListManager::OnRecv(uint64 playerID, uint32 sessionID, uint32 msgID, void* data)
{
    switch(msgID)
    {
    case MSG_RANKING_RANK_REQ:
        OnRankingRankReq(playerID, sessionID, msgID, data);
        break;
    case MSG_RANKING_PLAYER_ID_REQ:
        OnRankingPlayerIDReq(playerID, sessionID, msgID, data);
        break;
    case MSG_RANKING_CHANGE_REQ:
        OnChangeTwoPlayerRanking(playerID, sessionID, msgID, data);
        break;
    case MSG_RANKING_INFO_NTF:
        OnRankingInfoReq(playerID, sessionID, msgID, data);
        break;
    case MSG_RANKING_SAVED_NTF:
        {
            OnPlayerLevelRankRefresh();
            //每小时排行榜刷新
            RankListManager::Instance()->SendRankHourFresh();
        }
        break;
    case MSG_RANKING_TOP_UP_RANK_QUERY_CREATE_REQ:
        {
            OnTopUpRankQueryCreate(playerID, sessionID, msgID, data);
        }
        break;
    default:
        break;
    }
}

void RankListManager::OnRankingRankReq(uint64 playerID, uint32 sessionID, uint32 msgID, void* data)
{
    RANKING_RANK_REQ* recvMsg = (RANKING_RANK_REQ*)data;

    if(!recvMsg)
        return;

    std::vector<uint64> playerList;

    for(std::vector<BIGINT_ID>::iterator playerIt = recvMsg->playerList.begin(); playerIt != recvMsg->playerList.end(); playerIt++)
    {
        uint64 id = U64ID(playerIt->id_h, playerIt->id_l);
        playerList.push_back(id);
    }

    RANKING_RANK_ACK rankingRankAck;
    rankingRankAck.reason = recvMsg->reason;
    rankingRankAck.rank_type = recvMsg->rank_type;

    std::string rankTxt = "";

    switch(recvMsg->rank_type)
    {
    case RANKING_TYPE_OFFLINE_BATTLE:
    case RANKING_TYPE_TITLE_OFF_LINE_RANK:
        rankTxt = "ranking_offline_player";
        break;
    case RANKING_TYPE_OLD_OFFLINE_BATTLE:
        rankTxt = "ranking_offline_player";
        RankingLoaderJob::Instance()->LoadPlayerOfflineOldRankingByPlayerID(playerID, playerList, rankingRankAck, rankTxt);
        return;     // 比较特殊 取的是oldrank
    case RANKING_TYPE_BATTLE_POWER:
        rankTxt = "ranking_battle_power";
        break;
    case RANKING_TYPE_HOROSCOPE:
        rankTxt = "ranking_horoscope";
        break;
    case RANKING_TYPE_GAME_POINT:
        rankTxt = "gamepoint_ranking";
        break;
    case RANKING_TYPE_ASHURA:
        rankTxt = "ashura_rank";
        break;
    case RANKING_TYPE_PLAYER_LEVEL:
        rankTxt = "ranking_player_level";
        break;
    case RANKING_TYPE_GUILD:
        rankTxt = "guild_ranking";
        RankingLoaderJob::Instance()->LoadGuildRankingByGuildID(playerID, playerList, rankingRankAck, rankTxt);
        return;     // 公会不以playerID为 以guildID来查
    case RANKING_TYPE_WING:
        rankTxt = "ranking_wing";
        break;
    case RANKING_TYPE_FOLLOW_PET:
        rankTxt = "ranking_pet";
        break;
    case RANKING_TYPE_RIDE_PET:
        rankTxt = "ranking_ride";
        break;
    default:
        CnAssert(false);
        return;
    }

    RankingLoaderJob::Instance()->LoadPlayerRankingByPlayerID(playerID, playerList, rankingRankAck, rankTxt);
}

void RankListManager::SendRankInfo(uint64 playerID, RANKING_RANK_ACK& rankingRankAck)
{
    Player* player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    if(!player)
        return;

    player->SendToGame(MSG_RANKING_RANK_ACK, &rankingRankAck);
}

void RankListManager::OnRankingPlayerIDReq(uint64 playerID, uint32 sessionID, uint32 msgID, void* data)
{
    RANKING_PLAYER_ID_REQ* recvMsg = (RANKING_PLAYER_ID_REQ*)data;

    switch(recvMsg->rank_type)
    {
    case RANKING_TYPE_OFFLINE_BATTLE:
        RankingLoaderJob::Instance()->LoadPlayerIDByPlayerOfflineRanking(playerID, sessionID, recvMsg->rankList, recvMsg->reason);
        break;
    default:
        CnAssert(false);
        break;
    }
}

void RankListManager::SendPlayerInfo(uint64 playerID, uint32 sessionID, std::map<uint64, int32> playerRankList, uint8 rank_type, uint8 reason)
{
    RANKING_PLAYER_ID_ACK sendMsg;
    PLAYER_RANK_INFO playerRankInfo;

    for(std::map<uint64, int32>::iterator playerIt = playerRankList.begin(); playerIt != playerRankList.end(); ++playerIt)
    {
        playerRankInfo.player_id.id_h = U64ID_HIGH(playerIt->first);
        playerRankInfo.player_id.id_l = U64ID_LOW(playerIt->first);
        playerRankInfo.rank = playerIt->second;
        sendMsg.playerList.push_back(playerRankInfo);
    }
    sendMsg.rank_type = rank_type;
    sendMsg.reason = reason;

    GameHandler::Instance()->SendByID(GameHandler::Instance()->GetRegionIDBySessionID(sessionID), MSG_RANKING_PLAYER_ID_ACK, &sendMsg, playerID);
}

void RankListManager::SendRankInfoToGate(uint64 playerID, RANKING_ACK rankingAck)
{
    Player* player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    if(!player)
    {
        CnDbg("SendRankInfoToGate player id=%llu not exist\n", playerID);
        return;
    }

    player->SendToGate(MSG_RANKING_INFO_ACK, &rankingAck);
}

void RankListManager::SendTopRankToGameSvr(uint32 sessionID)
{
    // TODO:暂时只推送玩家等级的Top榜
    uint32 regionID = 0;
    regionID = GameHandler::Instance()->GetRegionIDBySessionID(sessionID);
    GameHandler::Instance()->SendByID(regionID, MSG_RANKING_TOP_RANK_NTF, &m_top_player_level_rank);
}

void RankListManager::SendTopRankToAll()
{
    // TODO:暂时只推送玩家等级的Top榜
    GameHandler::Instance()->SendToAll(MSG_RANKING_TOP_RANK_NTF, &m_top_player_level_rank);
}

//每小时排行榜刷新
void RankListManager::SendRankHourFresh()
{
    //发送每小时刷新
    NONE msgsend;
    GameHandler::Instance()->SendToAll(MSG_RANKING_HOURS_FRESH_RANK_NTF, &msgsend);
}

void RankListManager::OnQueryTopPlayerLevelRankAck(RANKING_TOP_RANK_NTF rankingRankAck)
{
    rankingRankAck.rank_type = RANKING_TYPE_PLAYER_LEVEL;
    m_top_player_level_rank = rankingRankAck;
    SendTopRankToAll();
}

void RankListManager::OnChangeTwoPlayerRanking(uint64 playerID, uint32 sessionID, uint32 msgID, void* data)
{
    RANKING_CHANGE_REQ* recvMsg = (RANKING_CHANGE_REQ*)data;

    uint64 player1 = U64ID(recvMsg->player1.id_h, recvMsg->player1.id_l);
    uint64 player2 = U64ID(recvMsg->player2.id_h, recvMsg->player2.id_l);

    switch(recvMsg->rank_type)
    {
    case RANKING_TYPE_OFFLINE_BATTLE:
        {
            RankingLoaderJob::Instance()->ChangeTwoPlayerRanking(playerID, sessionID, player1, player2, recvMsg->level1, recvMsg->level2);
        }
        break;
    default:
        CnAssert(false);
        break;
    }
}

void RankListManager::OnRankingInfoReq(uint64 playerID, uint32 sessionID, uint32 msgID, void* data)
{
    RANKING_PLAYER_ID_REQ* recvMsg = (RANKING_PLAYER_ID_REQ*)data;
    switch(recvMsg->rank_type)
    {
    case RANKING_TYPE_ALL_FIRST:
        RankingLoaderJob::Instance()->LoadPlayerListByAllFirst(playerID, recvMsg->rankList);
        break;
    case RANKING_TYPE_BATTLE_POWER:
        RankingLoaderJob::Instance()->LoadPlayerListByBattlePowerRanking(playerID, recvMsg->rankList);
        break;
    case RANKING_TYPE_GAME_POINT:
        RankingLoaderJob::Instance()->LoadPlayerListByGamepointRanking(playerID, recvMsg->rankList);
        break;
    case RANKING_TYPE_HOROSCOPE:
        RankingLoaderJob::Instance()->LoadPlayerListByHoroscopeRanking(playerID, recvMsg->rankList);
        break;
    case RANKING_TYPE_OFFLINE_BATTLE:
        RankingLoaderJob::Instance()->LoadPlayerListByOfflineRanking(playerID, recvMsg->rankList);
        break;
    case RANKING_TYPE_ASHURA:
        RankingLoaderJob::Instance()->LoadPlayerListByAshuraRank(playerID, recvMsg->rankList);
        break;
    case RANKING_TYPE_GUILD:
        RankingLoaderJob::Instance()->LoadPlayerListByGuildRank(playerID,recvMsg->rankList);
        break;
    case RANKING_TYPE_PLAYER_LEVEL:
        RankingLoaderJob::Instance()->LoadPlayerListByPlayerLevelRank(playerID);
        break;
    case RANKING_TYPE_PLOT_DUNGON_SCORE:
        RankingLoaderJob::Instance()->LoadPlotDungonScoreRank(playerID);
        break;
    case RANKING_TYPE_ACHIVE_COUNT:
        RankingLoaderJob::Instance()->LoadAchiveCountRank(playerID);
        break;
    case RANKING_TYPE_BATTLEHALL:   // 跨服排行
        GlobalDbOperation::Instance()->GetRankingAlliances(playerID, recvMsg->rankList);
        return;
    case RANKING_TYPE_WING:         // 翅膀排行榜
        RankingLoaderJob::Instance()->LoadPlayerListByWingRanking(playerID, recvMsg->rankList);
        return;
    case RANKING_TYPE_FOLLOW_PET:   // 宠物排行榜
        RankingLoaderJob::Instance()->LoadPlayerListByPetRanking(playerID, recvMsg->rankList);
        return;
    case RANKING_TYPE_RIDE_PET:     // 坐骑排行榜
        RankingLoaderJob::Instance()->LoadPlayerListByRideRanking(playerID, recvMsg->rankList);
        return;
    case RANKING_TYPE_FASHION:     // 化身排行榜
        RankingLoaderJob::Instance()->LoadPlayerListByRideRanking(playerID, recvMsg->rankList);
        return;
    default:
        CnError("rank type:%d is no exist\n", recvMsg->rank_type);
        break;
    }
}

void RankListManager::OnPlayerLevelRankRefresh()
{
    // TODO:查询数量待定
    OnTopPlayerLevelRankReq(20);
}

void RankListManager::OnTopPlayerLevelRankReq(uint32 count)
{
    // 异步获取玩家等级排行前count名
    RankingLoaderJob::Instance()->GetTopPlayerLevelRank(count);
}

void RankListManager::OnTopUpRankQueryCreate(uint64 playerID, uint32 sessionID, uint32 msgID, void *data)
{
    TOP_UP_RANK_QUERY_CREATE_REQ* recvMsg = (TOP_UP_RANK_QUERY_CREATE_REQ*)data;

    if(recvMsg->rank_type == 9999)
    {
        GmActivityRankReq();
        return;
    }
    else if(recvMsg->rank_type == 8888)
    {
        FestivalJob::Instance()->HandleGmtFestivalList();
        return;
    }

    TopUpRankQuery *query = NULL;
    RankQueryRegister::iterator i = m_rankQueryRegister.find(recvMsg->rank_type);
    if (i != m_rankQueryRegister.end())
    {
        query = (TopUpRankQuery*)i->second;
    }
    else
    {
        query = CnNew TopUpRankQuery(this);
        m_rankQueryRegister[recvMsg->rank_type] = query;
    }
    //query->Create(sessionID, (TOP_UP_RANK_QUERY_CREATE_REQ*)data);

    query->CreateNewQuery(sessionID, (TOP_UP_RANK_QUERY_CREATE_REQ*)data);
}

void RankListManager::GmActivityRankReq()
{
    RankQueryRegister::iterator i, iEnd = m_rankQueryRegister.end();
    for (i = m_rankQueryRegister.begin(); i != iEnd; ++i)
    {
        i->second->GMDailyRefresh();
    }
}
