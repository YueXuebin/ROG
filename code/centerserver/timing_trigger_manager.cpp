#include "centerserver_pch.h"
#include "timing_trigger_manager.h"
#include "ranking_list_manager.h"
#include "game_handler.h"
#include "daily_fashion_manager.h"

IMPLEMENT_SINGLETON(TimingTriggerManager)

TimingTriggerManager::TimingTriggerManager()
{
}

TimingTriggerManager::~TimingTriggerManager()
{
}

void TimingTriggerManager::OnRecv(uint32 msgID, void* data)
{
    switch (msgID)
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
    }

    //转发给所有GameServer
    GameHandler::Instance()->SendToAll(msgID, data);
}

void TimingTriggerManager::OnTimingTriggerStart(void* data)
{
    TIMING_TRIGGER_START_NTF* ntf = (TIMING_TRIGGER_START_NTF*)data;

    if (!ntf)
    {
        return;
    }

    switch (ntf->timer_id)
    {
    case TIMING_TRIGGER_DAILY_REFRESH:
        {
            OnChangeDay();
        }
        break;
    }
}

void TimingTriggerManager::OnTimingTriggerEnd(void* data)
{
}

void TimingTriggerManager::OnChangeDay()
{
    RankListManager::Instance()->DailyRefresh();
    DailyFashionManager::Instance()->DailyRefresh();
}