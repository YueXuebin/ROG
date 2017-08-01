#include "gameholder_pch.h"
#include "timing_trigger_manager.h"
#include "player_manager.h"
#include "top_up_rank_manager.h"


IMPLEMENT_SINGLETON(TimingTriggerManager)

TimingTriggerManager::TimingTriggerManager()
{
    Clear();
}

TimingTriggerManager::~TimingTriggerManager()
{
}

void TimingTriggerManager::Clear()
{
}

void TimingTriggerManager::OnRecv(uint32 msgID, void* data)
{
    switch(msgID)
    {
    case MSG_TIMING_TRIGGER_START_NTF:
        {
            OnTimingTriggerStart(data);
        }
        break;
    case MSG_TIMING_TRIGGER_END_NTF:
        {
            OnTimingTriggerEnd(data);
        }
        break;
    default:
        break;
    }
}

void TimingTriggerManager::OnTimingTriggerStart( void* data )
{
    TIMING_TRIGGER_START_NTF* ntf = (TIMING_TRIGGER_START_NTF*)data;
    if (!ntf)
        return;

    switch(ntf->timer_id)
    {
    case TIMING_TRIGGER_DAILY_REFRESH:
        {
            OnChangeDay();
            break;
        }
    case TIMING_TRIGGER_EIGHT_REFRESH:
        {
            OnTimeFresh();
        }
        break;
    default:
        break;
    }
}

void TimingTriggerManager::OnTimingTriggerEnd( void* data )
{

}

void TimingTriggerManager::OnChangeDay()
{
    const PlayerManager::PlayerMap& playerMap = PlayerManager::Instance()->GetPlayers();

    for (PlayerManager::PlayerMap::const_iterator playerIt = playerMap.begin(); playerIt != playerMap.end(); playerIt++)
    {
        if (playerIt->second->GetNetState() == Player::kStateInGame)
        {
            playerIt->second->CalDailyRefresh();
        }
    }
}
void TimingTriggerManager::OnTimeFresh()
{
    const PlayerManager::PlayerMap& playerMap = PlayerManager::Instance()->GetPlayers();

    for (PlayerManager::PlayerMap::const_iterator playerIt = playerMap.begin(); playerIt != playerMap.end(); playerIt++)
    {
        if (playerIt->second->GetNetState() == Player::kStateInGame)
        {
            playerIt->second->OnTimeFresh();
        }
    }
}
