#include "gameholder_pch.h"
#include "world_map_channel_manager.h"
#include "channel_manager.h"
#include "dungeon.h"
#include "activity_manager.h"
#include "ashura_manager.h"
#include "regular.h"
#include "scene.h"
#include "regular_manager.h"
#include "dungeon_manager.h"
#include "world_map_config.h"


WorldMapChannelManager::WorldMapChannelManager(DungeonSet* pDungeonSet) :
ChannelManager(pDungeonSet)
{

}

WorldMapChannelManager::~WorldMapChannelManager()
{

}

void WorldMapChannelManager::Update()
{

}

uint32 WorldMapChannelManager::AskEnterUnionID(Player* player, uint32 sceneID, uint32 channelID)
{
    channelID = 0;
    // 先在已使用的频道中查找可用的频道
    for(ChannelMap::iterator channelIt = m_ChannelIDs.begin(); channelIt != m_ChannelIDs.end(); ++channelIt)
    {
        // 此频道中的人是否大于n
        Dungeon* dungeon = m_pDungeonSet->GetDungeon(channelIt->first);
        CnAssert(dungeon);

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

        pDungeon->SetAllStateTime(0, 0, 0, (int32)(Crown::SDNow().GetTimeValue() * 1000));
    }

    return UNION_ID(channelID, m_pDungeonSet->GetDungeonID(), 1);
}

