#include "gameholder_pch.h"
#include "guild_channel_manager.h"
#include "dungeon_set.h"
#include "dungeon_data.h"
#include "dungeon.h"
#include "player.h"

GuildChannelManager::GuildChannelManager( DungeonSet* pDungeonSet ) :
ChannelManager(pDungeonSet)
{
    m_groupNum = 0;
    m_groupPlayerNum = 0;
}

GuildChannelManager::~GuildChannelManager()
{

}

uint32 GuildChannelManager::AskEnterUnionID( Player* player, uint32 sceneID, uint32 channelID )
{
    ChannelCounter  channelCounter;
    GroupCounter    groupCounter;
    uint64          groupID = 0;
    uint32          unionID = 0;
    Dungeon*        dungeon = 0;

    if(!player) return 0;

    groupID = player->GetGuildID();

    // 在已有的副本集中查询ID
    unionID = AskIDInDungeonSet(groupID, player->GetPlayerID());
    if(unionID)
    {
        return unionID;
    }

    // 未找到合适的副本，任意开启一个频道给该玩家
    channelID = findValidChannelID();

    // 创建频道副本
    dungeon = m_pDungeonSet->CreateDungeon(channelID);
    CnAssert(dungeon);
    SetReadyRunTime(dungeon);

    // 添加
    channelCounter.Clear();
    groupCounter.Clear();

    groupCounter.AddPlayer(player->GetPlayerID());
    groupCounter.groupID = groupID;
    channelCounter.channelID = channelID;
    channelCounter.IncPlayerNum();
    channelCounter.groupVert.push_back(groupCounter);
    m_channelCounter.push_back(channelCounter);

    return UNION_ID(channelID, m_pDungeonSet->GetDungeonID(), 1);
}

uint32  GuildChannelManager::AskIDInDungeonSet(uint64 groupID, uint64 playerID)
{
    ChannelCounter  channelCounter;
    GroupCounter    groupCounter;
    uint32          channelID = 0;
    bool            hasFind = false;

    std::map<uint32, Dungeon*>& dungeonSet = m_pDungeonSet->GetDungeons();
    ChannelCounterVert::iterator channelIt;

    for(std::map<uint32, Dungeon*>::iterator dungeonIt= dungeonSet.begin(); 
            dungeonIt != dungeonSet.end(); ++dungeonIt)
    {
        if(!dungeonIt->second || dungeonIt->second->IsClose())
            continue;

        channelIt = GetChannelCounterVert(dungeonIt->first);
        if(channelIt != m_channelCounter.end())
        {
            // 检查频道中的最大人数
            if(channelIt->playerNum >= (m_groupNum * m_groupPlayerNum))
                continue;

            // 在已有的分组中查找
            for(GroupCounterVert::iterator groupIt = channelIt->groupVert.begin();
                groupIt != channelIt->groupVert.end(); ++groupIt)
            {
                if(groupIt->groupID != groupID)
                    continue;

                hasFind = true;

                if(groupIt->GetPlayerNum() >= m_groupPlayerNum)
                    continue;

                CnAssert(!groupIt->HasPlayer(playerID));

                groupIt->AddPlayer(playerID);
                channelIt->IncPlayerNum();

                return UNION_ID(channelIt->channelID, m_pDungeonSet->GetDungeonID(), 1);
            }

            if(channelIt->groupVert.size() >= m_groupNum)
                continue;

            if(hasFind)
                continue;

            groupCounter.Clear();
            groupCounter.AddPlayer(playerID);
            groupCounter.groupID = groupID;
            channelIt->IncPlayerNum();
            channelIt->groupVert.push_back(groupCounter);

            return UNION_ID(channelIt->channelID, m_pDungeonSet->GetDungeonID(), 1);
        }

        // 添加
        channelCounter.Clear();
        groupCounter.Clear();

        groupCounter.AddPlayer(playerID);
        groupCounter.groupID = groupID;
        channelCounter.channelID = dungeonIt->first;
        channelCounter.IncPlayerNum();
        channelCounter.groupVert.push_back(groupCounter);
        m_channelCounter.push_back(channelCounter);

        return UNION_ID(channelCounter.channelID, m_pDungeonSet->GetDungeonID(), 1);
    }

    return 0;
}

GuildChannelManager::ChannelCounterVert::iterator 
GuildChannelManager::GetChannelCounterVert(uint32 channelID)
{
    ChannelCounterVert::iterator channelIt = m_channelCounter.begin();

    for(; channelIt != m_channelCounter.end(); ++channelIt)
    {
        if(channelIt->channelID == channelID)
        {
            return channelIt;
        }
    }

    return channelIt;
}

void GuildChannelManager::PlayerLeave(uint64 playerID)
{
    for(ChannelCounterVert::iterator channelIt = m_channelCounter.begin(); 
        channelIt != m_channelCounter.end(); ++channelIt)
    {
        for(GroupCounterVert::iterator groupIt = channelIt->groupVert.begin();
            groupIt != channelIt->groupVert.end(); ++groupIt)
        {
            if(groupIt->HasPlayer(playerID))
            {
                groupIt->DelPlayer(playerID);
                channelIt->DecPlayerNum();

                if(groupIt->GetPlayerNum() < 1)
                    channelIt->groupVert.erase(groupIt);

                if(channelIt->GetPlayerNum() < 1)
                    m_channelCounter.erase(channelIt);

                return;
            }
        }
    }

    CnAssert(false);
}
