#include "centerserver_pch.h"
#include "game_setting_manager.h"
#include "game_handler.h"
#include "vip_statistics_manager.h"
#include "donate_manager.h"
#include "buy_cheap_config.h"
#include "daily_fashion_config.h"

IMPLEMENT_SINGLETON(GameSettingManager)
GameSettingManager::GameSettingManager(void)
{
}

GameSettingManager::~GameSettingManager(void)
{
}

void GameSettingManager::SendToAllGameSvrGameSettingList()
{
    GameHandler::Instance()->SendToAll(MSG_GAME_SETTING_LIST_ACK, &m_gameSettingInfo);
}

void GameSettingManager::SetGameSettingList( const GAME_SETTING_LIST& list )
{
    m_gameSettingInfo = list;
}

void GameSettingManager::SendGameSettingListToGameSvr( uint32 sessionID )
{
    uint32 regionID = GameHandler::Instance()->GetRegionIDBySessionID(sessionID);

    GameHandler::Instance()->SendByID(regionID, MSG_GAME_SETTING_LIST_ACK, &m_gameSettingInfo);
}

void GameSettingManager::NotifyChange()
{
    for(std::vector<GAME_SETTING_INFO>::iterator settingIt = m_gameSettingInfo.setting_list.begin(); settingIt != m_gameSettingInfo.setting_list.end(); ++settingIt)
    {
        switch(settingIt->id)
        {
        case GAME_SETTING_TYPE_VIP_STATISTICS_PLAN:
            VipStatisticsManager::Instance()->SetConfig(settingIt->info_json);
            break;
        case GAME_SETTING_TYPE_DONATE:
            DonateManager::Instance()->SetConfig(settingIt->info_json);
            break;
        case GAME_SETTING_TYPE_BUY_CHEAP:
            BuyCheapConfig::Instance()->DBSetConfig(settingIt->info_json);
            break;
        case GAME_SETTING_TYPE_DAILY_DRAW_FASHION:
            DailyFashionConfig::Instance()->DBSetConfig(settingIt->info_json);
            break;
        default:
            break;
        }
    }
}
