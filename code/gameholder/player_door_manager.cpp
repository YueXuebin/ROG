#include "gameholder_pch.h"
#include "player_door_manager.h"
#include "player.h"
#include "center_handler.h"
#include "battle_offline_config.h"
#include "vip_config.h"
#include "battle_manager.h"
#include "player_door_reward_config.h"
#include "player_manager.h"
#include "event_define.h"
#include "trade_manager.h"
#include "mail_title_config.h"
#include "mail_manager.h"
#include "item_manager.h"
#include "offline_player_manager.h"
#include "gameplaytimes_notifier.h"
#include "game_util.h"

PlayerDoorManager::PlayerDoorManager( Player* player ) :
m_player(player)
{
    m_state = kStateInit;
}

PlayerDoorManager::~PlayerDoorManager()
{

}

// 登录时进行数据重置
void PlayerDoorManager::CalDailyRefresh()
{
    SaveYesterdayData();

    m_PlayerDoorInfo.challenge_count = 0;
    m_PlayerDoorInfo.buy_challenge_count = 0;
    m_player->GetNotifyManager()->SetGameplayTimes(NOTIFY_PLAYER_DOOR, m_PlayerDoorInfo.challenge_count);

    SendPlayerOldRankByIdReq();
}

void PlayerDoorManager::LoadInfo( PLAYER_DOOR_INFO& info )
{
    m_PlayerDoorInfo = info;

    time_t curTime = Crown::SDNow().GetTimeValue();

    if (!info.is_init) //init
    {
        m_PlayerDoorInfo.is_init = 1;
        m_PlayerDoorInfo.winning_steak = 0;
        m_PlayerDoorInfo.buy_challenge_count = 0;
        m_PlayerDoorInfo.challenge_count = 0;
    }

    if (info.cd_timestamp != 0) // 检查CD时间有没有完结
    {
        uint32 cd_seconds = 0;
        if ((uint32)curTime - info.cd_timestamp >= cd_seconds || CheckVIPNoCD()) // cd到
        {
            info.cd_timestamp = 0;
        }
    }

    m_player->GetNotifyManager()->SetGameplayTimes(NOTIFY_PLAYER_DOOR, m_PlayerDoorInfo.challenge_count);

    // 检查领取排行奖励
    CheckReward();
}

void PlayerDoorManager::FillPlayerDoorInfo(PLAYER_DOOR_INFO& info)
{
    info = m_PlayerDoorInfo;
}

void PlayerDoorManager::OnRecv( uint32 msgID, void* data )
{
    switch(msgID)
    {
    case MSG_PLAYER_DOOR_INFO_REQ:          // 请求勇者争霸数据
        OnPlayerDoorInfoReq();
        break;
    case MSG_PLAYER_DOOR_SELECT_REQ:        // 选择PK对象
        OnPlayerDoorSelectReq(data);
        break;
//    case MSG_PLAYER_DOOR_RESET_CHALLENGE_COUNT_REQ:
//        OnPlayerDoorResetChallengeCountReq();
//        break;
    case MSG_PLAYER_DOOR_ADD_CHALLENGE_COUNT_REQ:       // 增加挑战次数
        OnPlayerDoorAddChallengeCountReq();
        break;
    case MSG_PLAYER_DOOR_KILL_CD_REQ:       // 秒CD
        OnPlayerDoorKillCDReq();
        break;
    case MSG_PLAYER_DOOR_CD_OVER_REQ:       // CD结束查询
        OnPlayerDoorCDOverReq();
        break;
    case MSG_PLAYER_DOOR_REWARD_REQ:        // 领取排行奖励
        OnPlayerDoorRewardReq();
        break;
    case MSG_PLAYER_DOOR_REWARD_INFO_REQ:    // 排行奖励信息查询
        OnPlayerDoorRewardInfoReq();
        break;
    default:
        break;
    }
}

void PlayerDoorManager::ReceiveEvent( uint32 frameCount, uint32 eventId, int32 arg1, int32 arg2, int32 arg3 )
{
    switch(eventId)
    {
    case SYSTEM_EVENT_LOAD_OFFLINE_PLAYER:
        {
            uint64 offlinePlayerID = U64ID(arg1, arg2);
            if(m_state == kStateRefreshPlayerReq)
            {
                AsyncRefreshPlayerInfoList();
            }
        }
        break;
    default:
        break;
    }
}

void PlayerDoorManager::OnPlayerDoorSelectReq(void* data)
{
    PLAYER_DOOR_SELECT* pSelect = (PLAYER_DOOR_SELECT*)data;
    int32 SelectRank = pSelect->select_player_rank;

    if(m_PlayerInfoList.size() == 0)
    {
        m_PlayerDoorInfo.select_player.Clear();
        return;
    }

    // 设置选择的被挑战者
    for(std::vector<OFFLINE_PLAYER_INFO>::iterator playerIt = m_PlayerInfoList.begin(); playerIt != m_PlayerInfoList.end(); ++playerIt)
    {
        OFFLINE_PLAYER_INFO& playerInfo = *playerIt;
        /*if (playerInfo.rank == SelectRank)
        {
            m_PlayerDoorInfo.select_player = playerInfo;
            break;
        }*/
    }

    // 通知被挑战者
    Player* player = PlayerManager::Instance()->GetPlayerByPlayerID(U64ID(m_PlayerDoorInfo.select_player.player_id_h, m_PlayerDoorInfo.select_player.player_id_l));
    if (player)
    {
        PLAYER_DOOR_REPORT sendMsg;
        sendMsg.report_type = 2;
        sendMsg.challenger_name = m_player->GetName();
        player->SendToGate(MSG_PLAYER_DOOR_REPORT_NTF, &sendMsg);
    }
}

void PlayerDoorManager::SaveYesterdayData()
{
    //bool isSaveMax = m_player->GetGainbackResourceManager()->IsSaveMaxCount();

    //int32 count = 0;
    //if(isSaveMax)
    //    count = 0;
    //else
    //    count = m_PlayerDoorInfo.challenge_count;

    //m_player->SaveYesterdayCount(OPEN_SYS_PLAYER_DOOR, count);
}

void PlayerDoorManager::OnPlayerDoorInfoReq()
{
    if(m_state == kStateRefreshPlayerReq)
        return;     // 如果正在请求刷新角色列表则返回

    m_state = kStateRefreshPlayerReq;

    // req rank first
    std::vector<uint64> playerIdList;
    playerIdList.push_back(m_player->GetPlayerID());
    SendPlayerRankByIdReq(playerIdList);
}

// 根据Id获取Rank
void PlayerDoorManager::SendPlayerRankByIdReq(const std::vector<uint64>& playerIdList)
{
    RANKING_RANK_REQ sendMsg;

    sendMsg.reason = RANKING_SEASON_PLAYER_DOOR_INFO;
    sendMsg.rank_type = RANKING_TYPE_OFFLINE_BATTLE;

    for(std::vector<uint64>::const_iterator playerIt = playerIdList.begin(); playerIt != playerIdList.end(); playerIt++)
    {
        BIGINT_ID Id;
        Id.id_h = U64ID_HIGH(*playerIt);
        Id.id_l = U64ID_LOW(*playerIt);
        sendMsg.playerList.push_back(Id);
    }

    m_player->SendToCenter(MSG_RANKING_RANK_REQ, &sendMsg);
}

// ==================================================
// 排序勇者争霸玩家列表
// ==================================================
void PlayerDoorManager::OrderPlayerList()
{
    //std::vector<OFFLINE_PLAYER_INFO> orderedPlayerInfoList;
    //// 结果列表保存的角色身上
    //for(std::vector<OFFLINE_PLAYER_INFO>::iterator playerIt = m_PlayerInfoList.begin(); playerIt != m_PlayerInfoList.end(); ++playerIt)
    //{
    //    OFFLINE_PLAYER_INFO* playerInfo = &(*playerIt);
    //    if(playerInfo->sex == 0)
    //    {   // 机器人
    //        CnAssert(false);        
    //        if(Crown::SDRandom(100) <= 50)
    //            playerInfo->sex = 1;       // 男性
    //        else
    //            playerInfo->sex = 2;       // 女性

    //        CnAssert(playerInfo->move_speed != 0);

    //        CnAssert(playerInfo->battle_power == 0);    // 机器人没有配置战斗力
    //        playerInfo->battle_power = CalcBattlePowerForFakePlayer(*playerInfo);
    //    }

    //    // rank
    //    for (std::vector<PLAYER_RANK_INFO>::iterator playerIt = m_RankInfos.begin(); playerIt != m_RankInfos.end(); playerIt++)
    //    {
    //        if (playerIt->player_id.id_h == playerInfo->player_id_h && playerIt->player_id.id_l == playerInfo->player_id_l)
    //        {
    //            playerInfo->rank = playerIt->rank;
    //            break;
    //        }
    //    }
    //    orderedPlayerInfoList.push_back(*playerInfo);
    //}

    //m_PlayerInfoList = orderedPlayerInfoList;
}

void PlayerDoorManager::SendRefreshOfflinePlayerAck()
{
    time_t curTime = Crown::SDNow().GetTimeValue();
    if (m_PlayerDoorInfo.cd_timestamp != 0) // 检查CD时间有没有完结
    {
        uint32 cd_seconds = 0;
        if ((uint32)curTime - m_PlayerDoorInfo.cd_timestamp >= cd_seconds || CheckVIPNoCD()) // cd到
        {
            m_PlayerDoorInfo.cd_timestamp = 0;
            m_PlayerDoorInfo.cd_seconds = 0;
        }
        else // 计算当前剩下的cd秒数
        {
            m_PlayerDoorInfo.cd_seconds = cd_seconds - ((uint32)curTime - m_PlayerDoorInfo.cd_timestamp);
        }
    }
    
    // 更新排行奖励CD时间
    CheckReward();

    PLAYER_DOOR_ALL_INFO allInfo;
    allInfo.info = m_PlayerDoorInfo;
    allInfo.player_list.player_list = m_PlayerInfoList;

    m_player->SendToGate(MSG_PLAYER_DOOR_INFO_ACK, &allInfo);
}

const OFFLINE_PLAYER_INFO* PlayerDoorManager::GetBattleOfflinePlayerInfo()
{
    if(m_PlayerDoorInfo.select_player.player_id_l == 0)
        return NULL;
    return &m_PlayerDoorInfo.select_player;
}

void PlayerDoorManager::SendPlayerOldRankByIdReq()
{
    RANKING_RANK_REQ sendMsg;

    sendMsg.reason = RANKING_SEASON_OLD_OFF_LINE_RANK_INFO;
    sendMsg.rank_type = RANKING_TYPE_OLD_OFFLINE_BATTLE;

    BIGINT_ID playerID;
    playerID.id_h = U64ID_HIGH(m_player->GetPlayerID());
    playerID.id_l = U64ID_LOW(m_player->GetPlayerID());
    sendMsg.playerList.push_back(playerID);

    m_player->SendToCenter(MSG_RANKING_RANK_REQ, &sendMsg);
}

void PlayerDoorManager::SendTileRank(uint64 playerid)
{
    RANKING_RANK_REQ sendMsg;
    sendMsg.reason = RANKING_TITLE_OFF_LINE_RANK_INFO;
    sendMsg.rank_type = RANKING_TYPE_OFFLINE_BATTLE;
    BIGINT_ID Id;
    Id.id_h = U64ID_HIGH(playerid);
    Id.id_l = U64ID_LOW(playerid);
    sendMsg.playerList.push_back(Id);
    m_player->SendToCenter(MSG_RANKING_RANK_REQ, &sendMsg);
}

// 根据Rank获取Id
void PlayerDoorManager::SendPlayerIdByRankReq(std::vector<uint32> rankList)
{
    RANKING_PLAYER_ID_REQ sendMsg;

    sendMsg.reason = RANKING_SEASON_PLAYER_DOOR_INFO;
    sendMsg.rank_type = RANKING_TYPE_OFFLINE_BATTLE;
    for (std::vector<uint32>::iterator rankIt = rankList.begin(); rankIt != rankList.end(); rankIt++)
    {
        sendMsg.rankList.push_back(*rankIt);
    }

    m_player->SendToCenter(MSG_RANKING_PLAYER_ID_REQ, &sendMsg);
}

void PlayerDoorManager::OnPlayerRankByIdAck(RANKING_RANK_ACK* rankAck)
{
    if(!rankAck)
        return;

    if (rankAck->rank_type == RANKING_TYPE_OFFLINE_BATTLE)
    {
        int32 rank = 1;
        // 根据玩家自己的排名，请求获取离线用户排名列表
        for (std::vector<PLAYER_RANK_INFO>::iterator playerIt = rankAck->playerList.begin(); playerIt != rankAck->playerList.end(); playerIt++)
        {
            rank = playerIt->rank;
            break;
        }

        m_PlayerDoorInfo.rank = rank;

        int32 interval = 2000;
        if (rank <= 20)//1
        {
            interval = 1;
        }
        else if (rank <= 100)//2
        {
            interval = 2;
        }
        else if (rank <= 200)//5
        {
            interval = 5;
        }
        else if (rank <= 400)//10
        {
            interval = 10;
        }
        else if (rank <= 1000)//30
        {
            interval = 30;
        }
        else if (rank <= 2000)//50
        {
            interval = 50;
        }
        else if (rank <= 10000)//400
        {
            interval = 400;
        }

        // rankList: rank from num low -->high

        // rank 1, 2 and 3
        std::vector<uint32> rankList;
        rankList.push_back(1);
        rankList.push_back(2);
        rankList.push_back(3);

        if (rank <= 3) // in 1, 2, or 3
        {
            rankList.push_back(4);
            rankList.push_back(5);
            rankList.push_back(6);
            rankList.push_back(7);
        }
        else if (rank <= 7)
        {
            for (int i = 4; i<rank; i++)
                rankList.push_back(i);
            for (int i= rank+1; i<9; i++)
                rankList.push_back(i);
        }
        else
        {
            for (int i = 4; i>=1; i--)
            {
                rankList.push_back(rank-interval*i);
            }
        }

        SendPlayerIdByRankReq(rankList);
    }
}

void PlayerDoorManager::OnPlayerOldRankByIdAck(RANKING_RANK_ACK* rankAck)
{
    if(rankAck->rank_type != RANKING_TYPE_OLD_OFFLINE_BATTLE)
    {
        return;
    }

    if((rankAck->playerList.size() > 1) || (rankAck->playerList.size() == 0) ||
        (U64ID(rankAck->playerList.begin()->player_id.id_h, rankAck->playerList.begin()->player_id.id_l) != m_player->GetPlayerID()))
    {
        return;
    }

    int32 rank = rankAck->playerList.begin()->rank;
    m_YesterdayRank = rank;

    MY_OLD_RANK sendMsg;
    sendMsg.old_rank = rank;
    m_player->SendToGate(MSG_PLAYER_DOOR_MY_OLD_RANK_NTF, &sendMsg);
}

void PlayerDoorManager::OnPlayerIdByRankAck(RANKING_PLAYER_ID_ACK* playerIDAck)
{
    if(!playerIDAck)
    {
        return;
    }

    if (playerIDAck->rank_type == RANKING_TYPE_OFFLINE_BATTLE)
    {
        m_RankInfos.clear();
        for (std::vector<PLAYER_RANK_INFO>::iterator playerIt = playerIDAck->playerList.begin(); playerIt != playerIDAck->playerList.end(); playerIt++)
        {
            m_RankInfos.push_back(*playerIt);
        }

        AsyncRefreshPlayerInfoList();
    }
}

void PlayerDoorManager::AsyncRefreshPlayerInfoList()
{
    m_PlayerInfoList.clear();
    bool bFindAll = true;
    for(uint32 i=0; i<m_RankInfos.size(); i++)
    {
        PLAYER_RANK_INFO& playerRankInfo = m_RankInfos[i];
        uint64 player_id = U64ID(playerRankInfo.player_id.id_h, playerRankInfo.player_id.id_l);
        const OFFLINE_PLAYER_INFO* playerInfo = OfflinePlayerManager::Instance()->GetOfflinePlayerRecord(player_id);
        if(!playerInfo)
        {
            bFindAll = false;
            OfflinePlayerManager::Instance()->AskCSOfflinePlayerInfo(player_id, 0);
        }
        else
            m_PlayerInfoList.push_back(*playerInfo);
    }

    if(bFindAll)
    {
        // 得到旧的离线战斗排名
        SendPlayerOldRankByIdReq();
        // 排序离线战斗的玩家列表
        OrderPlayerList();
        
        SendRefreshOfflinePlayerAck();

        m_state = kStateNormal;
    }
    else
    {
        // 等待离线玩家数据获取后，再次Call这个函数
    }
}

void PlayerDoorManager::SubPlayerPkCount()
{
    if (m_PlayerDoorInfo.challenge_count > 0)
    {
        m_PlayerDoorInfo.challenge_count--;
        m_player->GetNotifyManager()->SetGameplayTimes(NOTIFY_PLAYER_DOOR, m_PlayerDoorInfo.challenge_count);
    }
}

void PlayerDoorManager::CombatFinish(bool bWin)
{
    if (bWin)
    {
        // winning steak count
        m_PlayerDoorInfo.winning_steak++;
    }
    else
    {
        // winning steak count
        m_PlayerDoorInfo.winning_steak = 0;
    }

    //胜利或者失败都有CD
    // cd time set to current server time
    time_t curTime = Crown::SDNow().GetTimeValue();
    if (CheckVIPNoCD())
        m_PlayerDoorInfo.cd_timestamp = 0;
    else
        m_PlayerDoorInfo.cd_timestamp = (uint32)curTime;
}

uint32 PlayerDoorManager::RankSwap()
{
    //if (m_PlayerDoorInfo.rank > m_PlayerDoorInfo.select_player.rank)
    //{
    //    // swap rank
    //    RANKING_CHANGE_REQ sendMsg;

    //    sendMsg.rank_type = RANKING_TYPE_OFFLINE_BATTLE;
    //    uint64 playerId = m_player->GetPlayerID();
    //    sendMsg.player1.id_l = U64ID_LOW(playerId);
    //    sendMsg.player1.id_h = U64ID_HIGH(playerId);
    //    sendMsg.player2.id_l = m_PlayerDoorInfo.select_player.player_id_l;
    //    sendMsg.player2.id_h = m_PlayerDoorInfo.select_player.player_id_h;
    //    sendMsg.level1 = m_player->GetLevel();

    //    // 查找是否存在离线玩家
    //    playerId = U64ID(m_PlayerDoorInfo.select_player.player_id_h, m_PlayerDoorInfo.select_player.player_id_l);
    //    Player* player = PlayerManager::Instance()->GetPlayerByPlayerID(playerId);
    //    if(player)
    //        sendMsg.level2 = player->GetLevel();
    //    else
    //        sendMsg.level2 = 0;

    //    // 公告名次换位并刷新排行榜第一
    //    SendMarQuee(m_PlayerDoorInfo.select_player.rank);

    //    // 发送邮件给被攻击者
    //    REWARD_TABLE rewardTable;
    //    if(m_PlayerDoorInfo.select_player.level >= 17)
    //    {
    //        MailManager::SendMessageMail(m_player->GetCenterSvrID(), 0, GetOpponentName(), rewardTable.items, MAIL_MESSAGE_TYPE_OFFLINE_PK, 2, 
    //            Crown::ToString(m_PlayerDoorInfo.rank).c_str(), m_player->GetName().c_str()); 
    //    }

    //    m_player->SendToCenter(MSG_RANKING_CHANGE_REQ, &sendMsg);

    //    return (m_PlayerDoorInfo.rank - m_PlayerDoorInfo.select_player.rank);
    //}

    return 0;
}
// 领取排名奖励
void PlayerDoorManager::OnPlayerDoorRewardReq()
{
    PLAYER_DOOR_REWARD_ACK sendMsg;
    sendMsg.is_reward = 0;

    bool canReward = CheckReward();

    if (canReward)
    {
        uint32 curRank = m_YesterdayRank; //昨日的排名

        if (curRank >= 4)
        {
            if (curRank < 11)
                curRank = 4;
            else if (curRank < 31)
                curRank = 11;
            else if (curRank < 51)
                curRank = 31;
            else if (curRank < 101)
                curRank = 51;
            else if (curRank < 201)
                curRank = 101;
            else if (curRank < 301)
                curRank = 201;
            else if (curRank < 401)
                curRank = 301;
            else
                curRank = 401;
        }

        std::vector<REWARD_ITEM>    items;
        const PlayerDoorRewardTemplate* tmpl = PlayerDoorRewardConfig::Instance()->GetPlayerDoorRewardTemplate(curRank);
        if (tmpl)
        {
            m_player->GetFcmRewardList(tmpl->items, items);
            if (items.size() > 0)
            {
                REWARD_TABLE rewardTable;
                rewardTable.Clear();
                for (std::vector<REWARD_ITEM>::const_iterator gainIt = items.begin(); gainIt != items.end(); ++gainIt)
                {
                    rewardTable.items.push_back(*gainIt);
                }
                //ItemManager::AddRewardTableWithEmail(m_player->GetCenterSvrID(), m_player->GetPlayerID(), rewardTable);
            }
        }

        uint32 honor_point = PlayerDoorRewardConfig::Instance()->GetHonorPointByRank(curRank);
        uint32 game_point = PlayerDoorRewardConfig::Instance()->GetGamePointByRank(curRank);
        uint32 soul_point = PlayerDoorRewardConfig::Instance()->GetSoulPointByRank(curRank);
        // 领取排行奖励
        if (game_point > 0 && honor_point > 0)
        {
            time_t curTime = Crown::SDNow().GetTimeValue();
            // 提示通知
            m_player->GetNotifyManager()->RewardNotify(NOTIFY_REWARD_PLAYER_DOOR, false);
            // 获得奖励
            m_player->AddGamePoint(game_point, REWARD_REASON_PLAYER_DOOR);
            m_player->AddHonorPoint(honor_point);
            m_player->AddSoulPoint(soul_point, REWARD_REASON_PLAYER_DOOR);
            // 下次领取时间
            Crown::CSDDateTime nextDay = Crown::CSDDateTime(curTime);
            nextDay.IncDay();
            nextDay.SetTime(0, 0, 0);
            time_t nextDayTime = nextDay.GetTimeValue();

            sendMsg.is_reward = 1;      // 成功领取标记

            m_PlayerDoorInfo.rewarded_info.rewarded_today = uint32(curTime);
            m_PlayerDoorInfo.rewarded_info.reward_cd_seconds = (uint32)nextDayTime - (uint32)curTime;
        }

        m_player->SendTaskEvent(TASK_EVENT_PLAYER_DOOR_REWARD, 1, 0);

        m_player->SendPlaySystemLog(PLAYSYSTEM_PLAYER_DOOR_REWARD, curRank);
    }

    // ack
    m_player->SendToGate(MSG_PLAYER_DOOR_REWARD_ACK, &sendMsg);
}

void PlayerDoorManager::OnPlayerDoorAddChallengeCountReq()
{
    int32 maxChallengeCount = 0;
    int32 addChallengeMoneyPoint = 0;
    if (m_PlayerDoorInfo.challenge_count < maxChallengeCount) // 挑战次数小于最大挑战次数
    {
        // check buy challenge count
        uint32 maxBuyChallengeCount = VipConfig::Instance()->GetVipValue(VIP_PRIVILEGE_19 , m_player->GetVipLevel());
        if (m_PlayerDoorInfo.buy_challenge_count < maxBuyChallengeCount) // 还有购买次数
        {
            // check money point
            if (m_player->GetMoneyPoint() >= addChallengeMoneyPoint) // 魔石足够
            {
                TradeManager::Instance()->SendTradeOrderReq(m_player, REWARD_REASON_PLAYER_DOOR_BUY_CHALLENGE_COUNT, PROP_PLAYER_DOOR_BUY_CHALLENGE, 0, 0, addChallengeMoneyPoint);
            }
        }
    }
}

void PlayerDoorManager::CBBuyChallengeCount()
{
    PLAYER_DOOR_ADD_CHALLENGE_COUNT_ACK sendMsg;

    sendMsg.is_ok = 1;
    sendMsg.added_challenge_count = 1;
    m_PlayerDoorInfo.challenge_count += sendMsg.added_challenge_count;
    m_PlayerDoorInfo.buy_challenge_count += sendMsg.added_challenge_count;

    m_player->GetNotifyManager()->SetGameplayTimes(NOTIFY_PLAYER_DOOR, m_PlayerDoorInfo.challenge_count);

    // ack
    m_player->SendToGate(MSG_PLAYER_DOOR_ADD_CHALLENGE_COUNT_ACK, &sendMsg);
}

void PlayerDoorManager::OnPlayerDoorKillCDReq()
{
    PLAYER_DOOR_KILL_CD_ACK sendMsg;
    sendMsg.is_ok = 0;

    if (m_PlayerDoorInfo.cd_timestamp != 0)
    {
        uint32 cd_seconds = 0;
        time_t curTime = Crown::SDNow().GetTimeValue();
        if ((uint32)curTime - m_PlayerDoorInfo.cd_timestamp >= cd_seconds || CheckVIPNoCD()) // cd到
        {
            sendMsg.is_ok = 1;
            m_PlayerDoorInfo.cd_timestamp = 0;
            m_PlayerDoorInfo.cd_seconds = 0;

            // ack
            m_player->SendToGate(MSG_PLAYER_DOOR_KILL_CD_ACK, &sendMsg);
        }
        else
        {
            uint32 cd = cd_seconds - ((uint32)curTime - m_PlayerDoorInfo.cd_timestamp);
            uint32 minu = cd/60;
            uint32 rem = cd%60;
            if (rem > 0) // 不满1分钟按照1分钟算
                minu++;
            int32 cdMoneyPointPerMinu = 0;
            int32 needMoneyPoint = minu*cdMoneyPointPerMinu;

            if (m_player->GetMoneyPoint() >= needMoneyPoint) // 魔石足够
            {
                TradeManager::Instance()->SendTradeOrderReq(m_player, REWARD_REASON_PLAYER_DOOR_KILL_CD, PROP_PLAYER_DOOR_KILL_CD, 0, needMoneyPoint, needMoneyPoint);
            }
        }
    }
}

void PlayerDoorManager::CBKillCD(uint32 needMoneyPoint)
{
    PLAYER_DOOR_KILL_CD_ACK sendMsg;

    sendMsg.is_ok = 1;
    sendMsg.money_point = needMoneyPoint;
    m_PlayerDoorInfo.cd_timestamp = 0;
    m_PlayerDoorInfo.cd_seconds = 0;

    // ack
    m_player->SendToGate(MSG_PLAYER_DOOR_KILL_CD_ACK, &sendMsg);
}

void PlayerDoorManager::OnPlayerDoorCDOverReq()
{
    PLAYER_DOOR_CD_OVER_ACK sendMsg;
    sendMsg.is_ok = 0;

    if (m_PlayerDoorInfo.cd_timestamp != 0)
    {
        uint32 cd_seconds = 0;
        time_t curTime = Crown::SDNow().GetTimeValue();
        if ((uint32)curTime - m_PlayerDoorInfo.cd_timestamp >= cd_seconds || CheckVIPNoCD()) // cd到
        {
            sendMsg.is_ok = 1;
            m_PlayerDoorInfo.cd_timestamp = 0;
            m_PlayerDoorInfo.cd_seconds = 0;
        }
    }
    else // 不可能执行到
    {
        sendMsg.is_ok = 1;
    }

    // ack
    m_player->SendToGate(MSG_PLAYER_DOOR_CD_OVER_ACK, &sendMsg);
}

void PlayerDoorManager::OnPlayerDoorRewardInfoReq()
{
    CheckReward();

    PLAYER_DOOR_REWARD_INFO rewardInfo;
    rewardInfo = m_PlayerDoorInfo.rewarded_info;

    // ack
    m_player->SendToGate(MSG_PLAYER_DOOR_REWARD_INFO_ACK, &rewardInfo);
}

bool PlayerDoorManager::CheckReward()
{
    bool canReward = false;
    time_t curTime = Crown::SDNow().GetTimeValue();
    
    if (m_PlayerDoorInfo.rewarded_info.rewarded_today > 0) // 今天已领取过，检查是否已经到第二天了
    {
        int32 days = m_player->GetDaysBetweenDates(curTime, (time_t)(m_PlayerDoorInfo.rewarded_info.rewarded_today));
        if (days >= 1)
        {
            canReward = true;
        }
    }
    else
    {
        //canReward = true;       // 从未领取过
        canReward = false;       // 从未领取过
        Crown::CSDDateTime nDay = Crown::CSDDateTime(curTime);
        nDay.IncDay();
        nDay.SetTime(0, 0, 0);
        time_t nTime = nDay.GetTimeValue();
        //m_PlayerDoorInfo.rewarded_info.rewarded_today = uint32(nTime);
        m_PlayerDoorInfo.rewarded_info.rewarded_today = uint32(curTime);
    }
    
    if(canReward)
    {
        m_PlayerDoorInfo.rewarded_info.reward_cd_seconds = 0;           // CD为0，表示可以领取
        m_player->GetNotifyManager()->RewardNotify(NOTIFY_REWARD_PLAYER_DOOR, true);
    }
    else
    {
        Crown::CSDDateTime nextDay = Crown::CSDDateTime(curTime);
        nextDay.IncDay();
        nextDay.SetTime(0, 0, 0);
        time_t nextDayTime = nextDay.GetTimeValue();
        m_PlayerDoorInfo.rewarded_info.reward_cd_seconds = (uint32)nextDayTime - (uint32)curTime;
        m_player->GetNotifyManager()->RewardNotify(NOTIFY_REWARD_PLAYER_DOOR, false);
    }

    return canReward;
}

void PlayerDoorManager::OnRankingDailyRefresh()
{
    //m_PlayerDoorInfo.challenge_count = GameParamConfig::Instance()->GetPlayerDoorMaxChallengeCount();
    //m_PlayerDoorInfo.buy_challenge_count = 0;
    //m_player->GetNotifyManager()->SetGameplayTimes(NOTIFY_PLAYER_DOOR, m_PlayerDoorInfo.challenge_count);

    //// 刷新数据
    //OnPlayerDoorInfoReq();
}

void PlayerDoorManager::SendMarQuee(int32 rank)
{
    NONE sendMsg;

    switch(rank)
    {
    case 1:
        CenterHandler::Instance()->SendToMasterCenter(MSG_RANKING_OFFLINE_FIRST_CHANGE, &sendMsg);    // 刷新排行榜总榜第一名
    case 2:
    case 3:
        {
            SERVER_MARQUEE sendMsg;
            sendMsg.id = MARQUEE_RANK_OFFLINE_BATTLE;
            sendMsg.parame_list.push_back(MarQueePlayerName(m_player->GetVipLevel(), m_player->GetName()));
            sendMsg.parame_list.push_back(Crown::ToString(rank));
            m_player->OnMarQuee(sendMsg);
        }
        break;
    }
}

bool PlayerDoorManager::CheckVIPNoCD()
{
    bool noCD = (VipConfig::Instance()->GetVipValue(VIP_PRIVILEGE_18 , m_player->GetVipLevel()) > 0);
    if (!noCD && m_player->IsExperienceVIP())
    {
        noCD = true;
    }
    return noCD;
}

uint32 PlayerDoorManager::CalcBattlePowerForFakePlayer(const OFFLINE_PLAYER_INFO& offlinePlayerInfo)
{
    int32 battle_power;

    battle_power = 0;

    //battle_power = (int32)( 
    //    offlinePlayerInfo.max_hp * 1.0f + offlinePlayerInfo.max_mp * 1.0f + offlinePlayerInfo.attack * 10.0f + offlinePlayerInfo.defence * 10.0f 
    //    + (offlinePlayerInfo.level * 3) * 3.0f                                // 技能等级与玩家等级相同
    //    + (offlinePlayerInfo.fire_defence + offlinePlayerInfo.ice_defence + offlinePlayerInfo.thunder_defence) * 3.0f
    //    + offlinePlayerInfo.dodge * 16.8f + offlinePlayerInfo.critical_attack * 3.5f + offlinePlayerInfo.level * 10
    //    );

    return battle_power;
}

bool PlayerDoorManager::CardByChallengeCount( int32 num )
{
    int32 maxChallengeCount = 0;
    if (m_PlayerDoorInfo.challenge_count < maxChallengeCount) // 挑战次数小于最大挑战次数
    {
        // check buy challenge count
        uint32 maxBuyChallengeCount = VipConfig::Instance()->GetVipValue(VIP_PRIVILEGE_19 , m_player->GetVipLevel());
        if (m_PlayerDoorInfo.buy_challenge_count < maxBuyChallengeCount) // 还有购买次数
        {
            CBBuyChallengeCount();
            return true;
        }
    }
    m_player->NotifyErrorCode(ERR_FAILED);
    return false;
}

