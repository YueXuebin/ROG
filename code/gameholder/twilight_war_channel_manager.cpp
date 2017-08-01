#include "gameholder_pch.h"
#include "twilight_war_channel_manager.h"
#include "dungeon_set.h"
#include "dungeon_data.h"
#include "dungeon.h"
#include "player.h"
#include "guild_config.h"
#include "activity_manager.h"
#include "regular_data.h"
#include "regular_manager.h"
#include "twilight_war_manager.h"

TwilightWarChannelManager::TwilightWarChannelManager( DungeonSet* pDungeonSet ) :
GuildChannelManager(pDungeonSet)
{
    SetGroupNum(GuildConfig::Instance()->GetTwilightWarMaxGuildNum());
    SetGroupPlayerNum(GuildConfig::Instance()->GetTwilightWarMaxPlayerNum());
}

TwilightWarChannelManager::~TwilightWarChannelManager()
{

}

void TwilightWarChannelManager::Update()
{
    
}

void TwilightWarChannelManager::SetReadyRunTime(Dungeon* dungeon)
{
    ACTIVITY_INFO*  activityInfo = 0;
    int32 runTime = 0;
    uint32 currTime = 0;

    if(!dungeon) return;

    // 如果暮光之战已开启，则设置不能销毁频道
    if(TwilightWarManager::Instance()->IsStart())
    {
        dungeon->SetCanDestory(false);
    }

    activityInfo = ActivityManager::Instance()->GetActivityByType(ACTIVITY_TYPE_TWILIGHT_WAR);
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

            if(TwilightWarManager::Instance()->IsStart())
            {
                currTime = TwilightWarManager::Instance()->GetCurrTime();
            }
            else
            {
                currTime = (doc->m_ReadyTime + runTime) * 1000;
            }
            dungeon->SetAllStateTime(doc->m_ReadyTime, runTime, 0, currTime);
        }
    }
}
