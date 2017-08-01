#include "gameholder_pch.h"
#include "td_channel_manager.h"
#include "channel_manager.h"
#include "dungeon_set.h"
#include "dungeon.h"
#include "scene.h"
#include "regular.h"
#include "activity_manager.h"
#include "regular_manager.h"
#include "tower_defence_manager.h"

TdChannelManager::TdChannelManager(DungeonSet* pDungeonSet) :
ChannelManager(pDungeonSet)
{

}

TdChannelManager::~TdChannelManager()
{

}

void TdChannelManager::Update()
{

}

uint32 TdChannelManager::AskEnterUnionID(Player* player, uint32 sceneID, uint32 channelID)
{
    if(!TowerDefenceManager::Instance()->IsOpen())
    {
        return EnterInvalidChannel();
    }

    channelID = 0;
    // 先在已使用的频道中查找可用的频道
    for(ChannelMap::iterator channelIt = m_ChannelIDs.begin(); channelIt != m_ChannelIDs.end(); ++channelIt)
    {
        // 此频道中的人是否大于8
        Dungeon* dungeon = m_pDungeonSet->GetDungeon(channelIt->first);
        CnAssert(dungeon);

        if(!dungeon || dungeon->IsClose() || !dungeon->GetDefaultScene()->GetRegular()->CanEnter())
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

        uint32 currTime = TowerDefenceManager::Instance()->GetCurrTime();
        int32 runTime = 0;
        //RegularDoc* doc = RegularManager::Instance()->GetRegularDoc(TD_MAP, 1);
        //if(doc)
        //{
        //    ACTIVITY_INFO* activityInfo = ActivityManager::Instance()->GetActivityByType(ACTIVITY_TYPE_TOWER_DEFENCE);
        //    if(activityInfo)
        //    {
        //        runTime = activityInfo->end_time - activityInfo->start_time - doc->m_ReadyTime;
        //        CnAssert(runTime >= 0);
        //        if(runTime < 0) runTime = 0;
        //        pDungeon->SetAllStateTime(doc->m_ReadyTime, runTime, 0, currTime);
        //    }
        //}
    }

    return UNION_ID(channelID, m_pDungeonSet->GetDungeonID(), 1);
}

