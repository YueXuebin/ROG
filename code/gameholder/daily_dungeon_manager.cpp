#include "gameholder_pch.h"
#include "daily_dungeon_manager.h"

DailyDungeonManager::DailyDungeonManager(Player* player) :
m_player(player)
{

}

DailyDungeonManager::~DailyDungeonManager(void)
{

}

void DailyDungeonManager::FillDailyDungeonInfo(DAILY_DUNGEON_INFO& info) const
{
    info = m_daily_dungeon_info;
}

void DailyDungeonManager::LoadDailyDungeonInfo(const DAILY_DUNGEON_INFO& info)
{
    m_daily_dungeon_info = info;
}
