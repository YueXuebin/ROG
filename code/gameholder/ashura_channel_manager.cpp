#include "gameholder_pch.h"
#include "ashura_channel_manager.h"
#include "channel_manager.h"
#include "dungeon.h"
#include "activity_manager.h"
#include "ashura_manager.h"
#include "regular.h"
#include "scene.h"
#include "regular_manager.h"
#include "game_setting_manager.h"


AshuraChannelManager::AshuraChannelManager(DungeonSet* pDungeonSet) :
ChannelManager(pDungeonSet)
{

}

AshuraChannelManager::~AshuraChannelManager()
{

}

void AshuraChannelManager::Update()
{

}

uint32 AshuraChannelManager::AskEnterUnionID(Player* player, uint32 sceneID, uint32 channelID)
{
    channelID = 0;
    // 先在已使用的频道中查找可用的频道
    for(ChannelMap::iterator channelIt = m_ChannelIDs.begin(); channelIt != m_ChannelIDs.end(); ++channelIt)
    {
        // 此频道中的人是否大于n
        Dungeon* dungeon = m_pDungeonSet->GetDungeon(channelIt->first);
        CnAssert(dungeon);
        if(dungeon->GetPlayerNum() >= GameSettingManager::Instance()->GetAshuraMaxPlayer())       // 频道上线人数
        {
            continue;
        }

        if(!dungeon->GetDefaultScene()->GetRegular()->CanEnter())
        {
            continue;
        }

        channelID = channelIt->first;
    }

    if(channelID == 0)
    {
        // 查找未用有效ID的频道
        channelID = findValidChannelID();
        Dungeon* pDungeon = m_pDungeonSet->CreateDungeon(channelID);
        CnAssert(pDungeon);

        ACTIVITY_INFO* activityInfo = ActivityManager::Instance()->GetActivityByType(ACTIVITY_TYPE_ASHURA);
        if(activityInfo)
        {
            //RegularDoc* doc = RegularManager::Instance()->GetRegularDoc(ASHURA_MAP, 1);
            //if(doc)
            //{
            //    int32 runTime = activityInfo->end_time - activityInfo->start_time - doc->m_ReadyTime;
            //    if(runTime < 0)
            //    {
            //        CnAssert(false);
            //        return UNION_ID(channelID, m_pDungeonSet->GetDungeonID(), 1);
            //    }
            //    uint32 currTime = AshuraManager::Instance()->GetCurrTime();
            //    pDungeon->SetAllStateTime(doc->m_ReadyTime, runTime, 0, currTime);
            //}
        }
    }

    return UNION_ID(channelID, m_pDungeonSet->GetDungeonID(), 1);
}

