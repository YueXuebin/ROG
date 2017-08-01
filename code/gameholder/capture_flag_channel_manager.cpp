#include "gameholder_pch.h"
#include "capture_flag_channel_manager.h"
#include "dungeon_set.h"
#include "channel_manager.h"
#include "scene.h"
#include "dungeon.h"
#include "capture_flag_manager.h"
#include "regular_data.h"
#include "regular_manager.h"
#include "activity_manager.h"


CaptureChannelManager::CaptureChannelManager(DungeonSet* pDungeonSet) : 
ChannelManager(pDungeonSet)
{

}

CaptureChannelManager::~CaptureChannelManager()
{

}

void CaptureChannelManager::Update()
{

}

uint32 CaptureChannelManager::AskEnterUnionID(Player* player, uint32 sceneID, uint32 channelID)
{
    // 如果活动没开启
    if(!CaptureFlagManager::Instance()->IsOpen())
        return EnterInvalidChannel();

    channelID = 0;
    // 先在已使用的频道中查找可用的频道
    for(ChannelMap::iterator channelIt = m_ChannelIDs.begin(); channelIt != m_ChannelIDs.end(); ++channelIt)
    {
        // 此频道中的人是否大于10
        Dungeon* dungeon = m_pDungeonSet->GetDungeon(channelIt->first);
        CnAssert(dungeon);

        if(!dungeon || dungeon->IsClose())
        {
            continue;
        }
        if(dungeon->GetPlayerNum() >= 10)
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

        uint32 currTime = CaptureFlagManager::Instance()->GetCurrTime();
        int32 runTime = 0;
        //RegularDoc* doc = RegularManager::Instance()->GetRegularDoc(CAPTURE_FLAG_MAP, 1);
        //if(doc)
        //{
        //    ACTIVITY_INFO* activityInfo = ActivityManager::Instance()->GetActivityByType(ACTIVITY_TYPE_CAPTURE_FLAG);
        //    if(activityInfo)
        //    {
        //        runTime = activityInfo->end_time - activityInfo->start_time - doc->m_ReadyTime;
        //        CnAssert(runTime >= 0);
        //        pDungeon->SetAllStateTime(doc->m_ReadyTime, runTime, 0, currTime);
        //    }
        //}
    }

    return UNION_ID(channelID, m_pDungeonSet->GetDungeonID(), 1);
}

