#include "gameholder_pch.h"
#include "channel_manager.h"
#include "dungeon_set.h"
#include "player.h"
#include "dungeon.h"

ChannelManager::ChannelManager(DungeonSet* pDungeonSet) :
m_pDungeonSet(pDungeonSet)
{

}

ChannelManager::~ChannelManager()
{

}

void ChannelManager::RegisterChannel( uint32 channelID )
{
    CnAssert(!HasChannelID(channelID));
    m_ChannelIDs[channelID] = channelID;
}

void ChannelManager::UnregisterChannel( uint32 channelID )
{
    CnAssert(HasChannelID(channelID));
    m_ChannelIDs.erase(m_ChannelIDs.find(channelID));
}

bool ChannelManager::HasChannelID( uint32 channelID )
{
    for(ChannelMap::iterator iter = m_ChannelIDs.begin(); iter != m_ChannelIDs.end(); iter++)
    {
        if(iter->first == channelID)
            return true;
    }
    return false;
}

uint32 ChannelManager::findValidChannelID()
{
    return m_pDungeonSet->findValidChannelID();
}

uint32 ChannelManager::GetDungeonID()
{
    return m_pDungeonSet->GetDungeonID();
}

uint32 ChannelManager::EnterInvalidChannel()
{
    uint32 channelID = findValidChannelID();
    Dungeon* pDungeon = m_pDungeonSet->CreateDungeon(channelID);
    CnAssert(pDungeon);

    pDungeon->SetInvalide(true);
    // 该副本1秒后踢出玩家
    pDungeon->SetAllStateTime(0, 1, 1, 0);

    return UNION_ID(channelID, m_pDungeonSet->GetDungeonID(), 1);
}

