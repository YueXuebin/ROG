#include "gameholder_pch.h"
#include "city_fight_channel_manager.h"
#include "dungeon_set.h"
#include "dungeon_data.h"
#include "dungeon.h"
#include "regular_world_boss.h"
#include "player.h"
#include "activity_manager.h"
#include "regular_manager.h"
#include "city_fight_manager.h"
#include "guild_config.h"

CityFightChannelManager::CityFightChannelManager( DungeonSet* pDungeonSet ) :
GuildChannelManager(pDungeonSet)
{
    SetGroupNum(GuildConfig::Instance()->GetCityFightMaxGuildNumPerRoom());
    SetGroupPlayerNum(GuildConfig::Instance()->GetCityFightMaxPlayerNumPerGuild());
}

CityFightChannelManager::~CityFightChannelManager()
{
    
}

void CityFightChannelManager::Update()
{
    
}

void CityFightChannelManager::SetReadyRunTime(Dungeon* dungeon)
{
    ACTIVITY_INFO*  activityInfo = 0;
    int32 runTime = 0;
    uint32 currTime = 0;

    if(!dungeon) return;

    activityInfo = ActivityManager::Instance()->GetActivityByType(ACTIVITY_TYPE_CITY_FIGHT);
    if(activityInfo)
    {
        RegularDoc* doc = RegularManager::Instance()->GetRegularDoc(m_pDungeonSet->GetDungeonID(), 1);
        if(doc)
        {
            runTime = activityInfo->end_time - activityInfo->start_time - doc->m_ReadyTime;
            if(runTime < 0)
            {
                CnAssert(false);
                runTime = 0;
            }
            currTime = CityFightManager::Instance()->GetCurrTime();
            dungeon->SetAllStateTime(doc->m_ReadyTime, runTime, 0, currTime);
        }
    }
}
