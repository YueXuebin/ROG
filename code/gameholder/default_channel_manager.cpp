#include "gameholder_pch.h"
#include "default_channel_manager.h"
#include "dungeon_set.h"
#include "player.h"


DefaultChannelManager::DefaultChannelManager(DungeonSet* pDungeonSet) : 
ChannelManager(pDungeonSet)
{

}

DefaultChannelManager::~DefaultChannelManager()
{

}

void DefaultChannelManager::Update()
{

}

uint32 DefaultChannelManager::AskEnterUnionID( Player* player, uint32 sceneID, uint32 channelID )
{
    if(channelID != 0 && HasChannelID(channelID)/* && player->IsGMJump()*/)
    {   // 只有GM可以任意进入临时副本的指定频道
        return UNION_ID(channelID, m_pDungeonSet->GetDungeonID(), 1);
    }

    if (channelID == 0)
    {
        channelID = findValidChannelID();
    }
    

    m_pDungeonSet->CreateDungeon(channelID);

    return UNION_ID(channelID, m_pDungeonSet->GetDungeonID(), 1);
}

