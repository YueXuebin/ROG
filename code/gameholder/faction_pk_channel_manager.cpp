#include "gameholder_pch.h"
#include "faction_pk_channel_manager.h"
#include "channel_manager.h"
#include "dungeon_set.h"
#include "dungeon.h"
#include "scene.h"
#include "regular.h"
#include "faction_manager.h"
#include "activity_manager.h"
#include "regular_manager.h"
#include "game_define.h"
#include "game_setting_manager.h"

FactionPkChannelManager::FactionPkChannelManager(DungeonSet* pDungeonSet) :
ChannelManager(pDungeonSet)
{

}

FactionPkChannelManager::~FactionPkChannelManager()
{

}

void FactionPkChannelManager::Update()
{

}

uint32 FactionPkChannelManager::AskEnterUnionID(Player* player, uint32 sceneID, uint32 channelID)
{
    if(FactionManager::Instance()->GetState() == FactionManager::kEnd)
    {
        return EnterInvalidChannel();
    }
    else if(FactionManager::Instance()->GetState() == FactionManager::kReady)
    {
        return ReadyUnionID();
    }
    else if(FactionManager::Instance()->GetState() == FactionManager::kStart)
    {
        return StartUnionID(channelID);
    }

    CnAssert(false);
    return 0;
}

uint32 FactionPkChannelManager::ReadyUnionID()
{
    // 先在已使用的频道中查找可用的频道
    for(ChannelMap::iterator channelIt = m_ChannelIDs.begin(); channelIt != m_ChannelIDs.end(); ++channelIt)
    {
        Dungeon* dungeon = m_pDungeonSet->GetDungeon(channelIt->first);
        CnAssert(dungeon);

        // 场景2是否存在
        Scene* scene2 = dungeon->GetScene(SCENE_WAITING_ROOM);
        if(!scene2)
        {
            continue;
        }

        // 场景2玩家数是否达到要求
        if(scene2->GetPlayerCount() >= g_FactionPK_WAIT_NUM)
        {
            continue;
        }

        SetAllStateTime(dungeon);
        return UNION_ID(channelIt->first, m_pDungeonSet->GetDungeonID(), SCENE_WAITING_ROOM);
    }

    // 没有可用频道就创建
    uint32 channelID = findValidChannelID();
    Dungeon* pDungeon = m_pDungeonSet->CreateDungeon(channelID);
    CnAssert(pDungeon);

    SetAllStateTime(pDungeon);
    return UNION_ID(channelID, m_pDungeonSet->GetDungeonID(), SCENE_WAITING_ROOM);
}

uint32 FactionPkChannelManager::StartUnionID(uint32 channelID)
{
    // 不指定频道
    if(channelID == 0)
    {
        // 先在已使用的频道中查找可用的频道
        for(ChannelMap::iterator channelIt = m_ChannelIDs.begin(); channelIt != m_ChannelIDs.end(); ++channelIt)
        {
            Dungeon* dungeon = m_pDungeonSet->GetDungeon(channelIt->first);
            CnAssert(dungeon);

            // 场景1是否存在
            Scene* scene1 = dungeon->GetScene(1);
            if(!scene1)
            {
                continue;
            }

            // 场景1玩家数是否达到要求
            if(scene1->GetPlayerCount() > GameSettingManager::Instance()->GetFactionPKMaxPlayer())
            {
                continue;
            }

            return UNION_ID(channelIt->first, m_pDungeonSet->GetDungeonID(), 1);
        }

        // 找不到可用的就创建一个
        channelID = findValidChannelID();
        Dungeon* pDungeon = m_pDungeonSet->CreateDungeon(channelID);
        CnAssert(pDungeon);

        SetAllStateTime(pDungeon);
        return UNION_ID(channelID, m_pDungeonSet->GetDungeonID(), 1);
    }
    // 指定频道
    else
    {
        // 已有此频道
        if(m_ChannelIDs.find(channelID) != m_ChannelIDs.end())
        {
            return UNION_ID(channelID, m_pDungeonSet->GetDungeonID(), 1);
        }

        // 没有此频道就创建
        Dungeon* pDungeon = m_pDungeonSet->CreateDungeon(channelID);
        CnAssert(pDungeon);

        SetAllStateTime(pDungeon);
        return UNION_ID(channelID, m_pDungeonSet->GetDungeonID(), 1);
    }
}

void FactionPkChannelManager::SetAllStateTime(Dungeon* dungeon)
{
    if(!dungeon)
    {
        return;
    }

    // 准备时进入副本
    uint32 currTime = FactionManager::Instance()->GetCurrTime();

    if(currTime < FACTION_PK_READY_TIME)
    {
        dungeon->SetSceneAllStateTime(SCENE_WAITING_ROOM, 0, (FACTION_PK_READY_TIME)/1000, 0, currTime);
    }

    ACTIVITY_INFO* activityFactionPK = ActivityManager::Instance()->GetActivityByType(ACTIVITY_TYPE_FACTION_PK);
    int32 runTime = activityFactionPK->end_time - activityFactionPK->start_time;
    if(runTime < 0)
    {
        runTime = 0;
        CnError("ready time > activity time");
        CnAssert(false);
    }
    dungeon->SetSceneAllStateTime(1, 0, runTime, 0, currTime);

}
