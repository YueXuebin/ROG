#include "centerserver_pch.h"
#include "daily_fashion_manager.h"
#include "special_activity_job.h"
#include "game_handler.h"
#include "daily_fashion_config.h"


IMPLEMENT_SINGLETON(DailyFashionManager)

DailyFashionManager::DailyFashionManager(void)
{
    m_dailyFashionConfig.Clear();
}

DailyFashionManager::~DailyFashionManager(void)
{
}

void DailyFashionManager::Init()
{
    SpecialActivityJob::Instance()->GetDailyFashionAllInfo(m_dailyFashionConfig);
    UpdateDailyInfo();
}

void DailyFashionManager::OnRecv( uint64 playerID, uint32 sessionID, uint32 msgID, void* data )
{
    switch(msgID)
    {
    case MSG_DAILY_FASHION_DB_INFO_REQ:
        SendDailyFashionInfo(playerID);
        break;
    default:
        break;
    }
}

void DailyFashionManager::SendDailyFashionInfo( uint64 playerID /*= 0*/ )
{
    GameHandler::Instance()->SendToAll(MSG_DAILY_FASHION_DB_INFO_ACK, &m_dailyFashionConfig);
}

void DailyFashionManager::DailyRefresh()
{
    UpdateDailyInfo();
}

void DailyFashionManager::UpdateDailyInfo()
{
    uint32 flag = DailyFashionConfig::Instance()->GetNextDayId(&m_dailyFashionConfig);
    if(flag == 1)
    {
        SaveDB();
        SendDailyFashionInfo();
    }
}

void DailyFashionManager::SaveDB()
{
    SpecialActivityJob::Instance()->SaveDailyFashionAllInfo(m_dailyFashionConfig);
}
