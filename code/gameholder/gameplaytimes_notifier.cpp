#include "gameholder_pch.h"
#include "gameplaytimes_notifier.h"
#include "player.h"

GameplayTimesNotifier::GameplayTimesNotifier( Player* player ) :
m_player(player)
{

}

GameplayTimesNotifier::~GameplayTimesNotifier()
{

}

void GameplayTimesNotifier::OnRecv( uint32 msgID, void* data )
{
    switch(msgID)
    {
    case MSG_GAMEPLAY_TIMES_NOTIFY_REQ: // 客户端请求玩法数据
        OnGameNotifyReq(data);
        break;
    }
}

void GameplayTimesNotifier::OnGameNotifyReq(void* data)
{
    GAMEPLAY_TIMES_NOTIFY_REQ* recvMsg = (GAMEPLAY_TIMES_NOTIFY_REQ*)data;

    // ack
    if (recvMsg->game_id == NOTIFY_ALL)
    {
        SendGameNotifyList();
    }
    else
    {
        SendGameNotify(recvMsg->game_id);
    }
}

void GameplayTimesNotifier::SendGameNotify(uint32 notifyId)
{
    GameNotifyMap::iterator iter = m_GamePlayTimesMap.find(notifyId);

    if (iter != m_GamePlayTimesMap.end())
    {
        GAMEPLAY_TIMES_NOTIFY_LIST notifyList;
        notifyList.game_notify_list.push_back(iter->second);
        m_player->SendToGate(MSG_GAMEPLAY_TIMES_NOTIFY_ACK, &notifyList);
    }
}

void GameplayTimesNotifier::SendGameNotifyList()
{
    GAMEPLAY_TIMES_NOTIFY_LIST notifys;
    for(GameNotifyMap::iterator iter = m_GamePlayTimesMap.begin(); iter != m_GamePlayTimesMap.end(); iter++)
    {
        notifys.game_notify_list.push_back(iter->second);
    }
    m_player->SendToGate(MSG_GAMEPLAY_TIMES_NOTIFY_ACK, &notifys);
}

void GameplayTimesNotifier::SendRewardNotify(uint32 notifyId)
{
    GameNotifyMap::iterator iter = m_RewardNotifyMap.find(notifyId);

    if (iter != m_RewardNotifyMap.end())
    {
        GAMEPLAY_TIMES_NOTIFY notify;
        notify = iter->second;
        m_player->SendToGate(MSG_GAME_NOTIFY_REWARD_NTF, &notify);
    }
}

void GameplayTimesNotifier::SetGameplayTimes(uint32 gameplayID, int32 currTimes, int32 sumTimes)
{
    if (gameplayID < NOTIFY_ALL)
    {
        GAMEPLAY_TIMES_NOTIFY notify;
        notify.game_id = gameplayID;
        notify.curr_times = currTimes;
        notify.sum_times = sumTimes;
        m_GamePlayTimesMap[gameplayID] = notify;
        SendGameNotify(gameplayID);
    }
}

void GameplayTimesNotifier::RewardNotify(uint32 notifyId, bool bNotify)
{
    if (notifyId < NOTIFY_REWARD_ALL)
    {
        GAMEPLAY_TIMES_NOTIFY notify;
        notify.game_id = notifyId;
        notify.curr_times = (bNotify? 1:0);
        m_RewardNotifyMap[notifyId] = notify;
        SendRewardNotify(notifyId);
    }
}
