#include "gameholder_pch.h"
#include "team_channel_manager.h"
#include "player.h"
#include "dungeon_set.h"
#include "dungeon.h"


TeamChannelManager::TeamChannelManager( DungeonSet* pDungeonSet ) :
ChannelManager(pDungeonSet)
{

}

TeamChannelManager::~TeamChannelManager()
{

}

void TeamChannelManager::Update()
{

}

uint32 TeamChannelManager::AskEnterUnionID( Player* player, uint32 sceneID, uint32 channelID )
{
    uint64 teamID = player->GetTeamID();
    channelID = 0;
    if(teamID == 0)
    {
        CnWarn("player %s in team but no team\n", player->GetName().c_str());        // 没有队伍的玩家，进入了组队副本
        channelID = findValidChannelID();
        Dungeon* pDungeon = m_pDungeonSet->CreateDungeon(channelID);
        CnAssert(pDungeon);
        return UNION_ID(pDungeon->GetChannelID(), m_pDungeonSet->GetDungeonID(), 1);
    }

    if(m_TeamChannelMap.find(teamID) == m_TeamChannelMap.end())
    {
        channelID = findValidChannelID();
    }
    else
    {
        channelID = m_TeamChannelMap[teamID];
    }

    // 如果有队伍副本，检查这个队伍副本是否已过期
    Dungeon* pDungeon = m_pDungeonSet->GetDungeon(channelID);
    if(pDungeon && pDungeon->IsClose())
    {
        channelID = findValidChannelID();
    }

    if(!m_pDungeonSet->GetDungeon(channelID))
    {
        //CnAssert(m_TeamChannelMap.find(teamID) == m_TeamChannelMap.end());
        Dungeon* pDungeon = m_pDungeonSet->CreateDungeon(channelID);
        CnAssert(pDungeon);

        m_TeamChannelMap[teamID] = channelID;
    }

    return UNION_ID(channelID, m_pDungeonSet->GetDungeonID(), 1);
}

void TeamChannelManager::RegisterChannel( uint32 channelID )
{
    ChannelManager::RegisterChannel(channelID);

    CnAssert(FindTeamIDByChannelID(channelID) == 0);
}

void TeamChannelManager::UnregisterChannel( uint32 channelID )
{
    ChannelManager::UnregisterChannel(channelID);

    for(std::map<uint64, uint32>::iterator iter = m_TeamChannelMap.begin(); iter != m_TeamChannelMap.end(); iter++)
    {
        if(iter->second == channelID)
        {
            m_TeamChannelMap.erase(iter);
            break;
        }
    }
}

uint64 TeamChannelManager::FindTeamIDByChannelID( uint32 channelID )
{
    for(std::map<uint64, uint32>::iterator iter = m_TeamChannelMap.begin(); iter != m_TeamChannelMap.end(); iter++)
    {
        if(iter->second == channelID)
            return iter->first;
    }
    return 0;
}

