#include "centerserver_pch.h"
#include "channel_manager.h"
#include "guild_job.h"
#include "config_loader.h"
#include "guild_config.h"
#include "game_handler.h"
#include "player.h"
#include "player_manager.h"
#include "player_net_handler.h"

IMPLEMENT_SINGLETON(ChannelManager)

// ==================================================
// 构造函数
// ==================================================
ChannelManager::ChannelManager()
{
    Clear();
}

// ==================================================
// 析构函数
// ==================================================
ChannelManager::~ChannelManager()
{
    
}

// ==================================================
// 清空
// ==================================================
void ChannelManager::Clear()
{
    //m_player = 0;
    //m_team_info.Clear();
}

// ==================================================
// 分配频道号
// ==================================================
uint32 ChannelManager::RequestChannelID(uint32 dungeonID, Player* player)
{
    uint32 channel = 0;

    for(DungeonCounterVert::iterator dungeonIt = m_dungeonCounter.begin();
        dungeonIt != m_dungeonCounter.end(); ++dungeonIt)
    {
        if(dungeonID == dungeonIt->dungeonID)
        {
            if(player)
            {
                channel = RequestFactionPKChannelID(*dungeonIt, player);
            }
            if(!channel) continue;
            return channel;
        }
        else if((dungeonID == GuildConfig::Instance()->GetCrusadeBossDungeonID()) && (dungeonID == dungeonIt->dungeonID))
        {
            if(player)
            {
                channel = RequestCrusadeBossChannelID(*dungeonIt, player->GetGuildID());
            }
            if(!channel) continue;
            return channel;
        }
        else if((dungeonID == GuildConfig::Instance()->GetTwilightWarDungeonID()) && (dungeonID == dungeonIt->dungeonID))
        {
            if(player)
            {
                channel = RequestTwilightWarChannelID(*dungeonIt, player->GetGuildID());
            }
            if(!channel) continue;
            return channel;
        }
        else
        {
            uint64 groupID = 0;
            if(player)
            {
                groupID = player->GetGuildID();
            }
            /// 直接返回频道ID,都分配到一个频道里
            for(GroupCounterVert::iterator guildIt = dungeonIt->groupVert.begin();
                guildIt != dungeonIt->groupVert.end(); ++guildIt)
            {
                if(guildIt->groupID != groupID) continue;
                return channel;
            }
        }
    }

    return 0;
}

// ==================================================
// 派别PK分配频道号
// ==================================================
uint32 ChannelManager::RequestFactionPKChannelID(DungeonPlayerCounter& dungeonCounter, Player* player)
{
    int32 scoreA = 0;
    int32 scoreB = 0;
    GroupCounter* groupA = 0;
    GroupCounter* groupB = 0;

    // 获得A组和B组的分数
    for(uint32 i=0; i < dungeonCounter.groupVert.size(); ++i)
    {
        if(i == 0)
        {
            scoreA = dungeonCounter.groupVert[i].score;
            groupA = &dungeonCounter.groupVert[i];
        }
        else if(i == 1)
        {
            scoreB = dungeonCounter.groupVert[i].score;
            groupB = &dungeonCounter.groupVert[i];
        }
    }

    if(!groupA)
    {   // A组没人,将人分配到A组
        //player->SetFactionID(1);
        return dungeonCounter.channelID;
    }
    else if(!groupB)
    {   // B组没人,将人分配到B组
        //player->SetFactionID(2);
        return dungeonCounter.channelID;
    }
    else
    {
        if((scoreA < scoreB) && (groupA->GetPlayerNum() < 2))
        {   // A组分数比B组少,且人数未满,将人分配到A组
            //player->SetFactionID(1);
            return dungeonCounter.channelID;
        }
        else if((scoreB < scoreA) && (groupB->GetPlayerNum() < 2))
        {   // B组分数比A组少,且人数未满,将人分配到B组
            //player->SetFactionID(2);
            return dungeonCounter.channelID;
        }
        else if(scoreA == scoreB)
        {
            if(groupA->GetPlayerNum() < 2)
            {   // 将人分配到A组
                //player->SetFactionID(1);
                return dungeonCounter.channelID;
            }
            else if(groupB->GetPlayerNum() < 2)
            {
                //player->SetFactionID(2);
                return dungeonCounter.channelID;
            }
        }
    }

    //player->SetFactionID(1);
    return 0;
}

// ==================================================
// 公会分配频道号
// ==================================================
uint32 ChannelManager::RequestCrusadeBossChannelID(DungeonPlayerCounter& dungeonCounter, uint64 guildID)
{
    for(GroupCounterVert::iterator groupIt = dungeonCounter.groupVert.begin();
        groupIt != dungeonCounter.groupVert.end(); ++groupIt)
    {
        if(groupIt->groupID != guildID) continue;
        // 从配置表中读最大人数
        if(groupIt->GetPlayerNum() >= GuildConfig::Instance()->GetCrusadeBossMaxPlayerNum())
        {
            continue;
        }
        // 添加预备役的数量
        groupIt->IncPrepareNum();
        return dungeonCounter.channelID;
    }
    return 0;
}

// ==================================================
// 请求暮光之战频道号
// ==================================================
uint32 ChannelManager::RequestTwilightWarChannelID(DungeonPlayerCounter& dungeonCounter, uint64 guildID)
{
    // 检查进入的公会数量
    for(GroupCounterVert::iterator groupIt = dungeonCounter.groupVert.begin();
        groupIt != dungeonCounter.groupVert.end(); ++groupIt)
    {
        if(groupIt->groupID != guildID) continue;
        // 从数据表中读取最大人数
        if(groupIt->GetPlayerNum() >= GuildConfig::Instance()->GetTwilightWarMaxPlayerNum())
        {
            return 0;
        }
        // 添加预备役的数量
        groupIt->IncPrepareNum();
        return dungeonCounter.channelID;
    }

    // 检查进入的公会数量
    if(dungeonCounter.groupVert.size() < (uint32)GuildConfig::Instance()->GetTwilightWarMaxGuildNum())
    {
        return dungeonCounter.channelID;
    }

    return 0;
}

// ==================================================
// 进入组队派别PK副本
// ==================================================
void ChannelManager::OnEnterTeamDungeon(uint32 channelID, uint32 dungeonID, Player* player)
{
    DungeonPlayerCounter dungeonCounter;
    GroupCounter playerCounter;
    DungeonCounterVert::iterator dungeonIt;
    GroupCounterVert::iterator guildIt;
    uint64 playerID = 0;
    uint64 factionID = 0;

    if(!player) return;

    playerID = player->GetPlayerID();
    factionID = player->GetGuildID();

    // 如果此角色此时无公会，则无需记录，因为他即使分配到了channelID
    // 在登陆 GS 的时候也不会让他进入副本，这样channelID 会再次被收回
    if(!factionID) return;

    // 查找相同的公会ID
    for(dungeonIt = m_dungeonCounter.begin(); dungeonIt != m_dungeonCounter.end(); ++dungeonIt)
    {
        // 匹配副本号和频道号
        if((dungeonIt->dungeonID != dungeonID) || (dungeonIt->channelID != channelID))
        {
            continue;
        }
        break;
    }

    // 是否找到副本的计数信息
    if(dungeonIt != m_dungeonCounter.end())
    {
        // 添加总人数
        CnAssert(dungeonIt->GetPlayerNum() > 0);
        dungeonIt->IncPlayerNum();

        // 如果在列表中查找到这个派别
        for(guildIt = dungeonIt->groupVert.begin(); guildIt != dungeonIt->groupVert.end(); ++guildIt)
        {
            if(guildIt->groupID != factionID) continue;
            break;
        }

        // 若查找到公会信息，则减去预备数，并将角色添加到列表
        if(guildIt != dungeonIt->groupVert.end())
        {
            guildIt->DecPrepareNum();
            guildIt->playerIDList.push_back(playerID);
            return;
        }

        // 未查找到此公会，则添加新的公会成员计数
        playerCounter.Clear();
        playerCounter.groupID = factionID;
        playerCounter.playerIDList.push_back(playerID);

        dungeonIt->groupVert.push_back(playerCounter);

        return; // 退出
    }

    // 添加新的计数时，是不需要考虑 prepareNum 的
    // 因为如果同时同公会的两个成员进入，会进入到不同频道的副本

    // 添加新计数
    dungeonCounter.Clear();
    playerCounter.Clear();

    dungeonCounter.channelID = channelID;
    dungeonCounter.dungeonID = dungeonID;
    dungeonCounter.playerNum = 1;

    playerCounter.groupID = factionID;
    playerCounter.playerIDList.push_back(playerID);

    dungeonCounter.groupVert.push_back(playerCounter);

    m_dungeonCounter.push_back(dungeonCounter);
}

// ==================================================
// 离开最对派别PK副本
// ==================================================
void ChannelManager::OnExitTeamDungeon(uint32 channelID, uint32 dungeonID, Player* player)
{
    DungeonCounterVert::iterator dungeonIt;
    uint64 playerID = 0;

    if(!player) return;

    playerID = player->GetPlayerID();

    // 这里不使用 player 的公会ID来再次索引了
    // 因为有可能 Player 在公会副本中战斗的时候，有人将他踢出公会
    // （虽然他还在公会副本中会得到结算，但他已经不在公会中了）
    for( dungeonIt = m_dungeonCounter.begin(); dungeonIt != m_dungeonCounter.end(); ++dungeonIt)
    {
        // 匹配副本号和频道号
        if((dungeonIt->dungeonID != dungeonID) || (dungeonIt->channelID != channelID))
        {
            continue;
        }
        break;
    }

    // 是否找到副本的计数信息
    if(dungeonIt == m_dungeonCounter.end())
    {
        return;
    }

    // 减少总人数
    CnAssert(dungeonIt->GetPlayerNum() > 0);
    dungeonIt->DecPlayerNum();

    // 在工会中
    for(GroupCounterVert::iterator groupIt = dungeonIt->groupVert.begin();
        groupIt != dungeonIt->groupVert.end(); ++groupIt)
    {
        // 检查此公会中是否存在此人
        if(!groupIt->HasPlayer(playerID)) continue;

        // 在列表中删除此人
        groupIt->DelPlayer(playerID);

        break;
    }

    // 如果副本中无人，则删除信息
    if(dungeonIt->GetPlayerNum() < 1)
    {
        m_dungeonCounter.erase(dungeonIt);
    }
}

// ==================================================
// 进入工会BOSS副本
// ==================================================
void ChannelManager::OnEnterGuildBossDungeon(uint32 channelID, uint32 dungeonID, Player* player)
{
    DungeonPlayerCounter dungeonCounter;
    GroupCounter playerCounter;
    DungeonCounterVert::iterator dungeonIt;
    GroupCounterVert::iterator guildIt;
    uint64 playerID = 0;
    uint64 guildID = 0;

    if(!player) return;

    playerID = player->GetPlayerID();
    guildID = player->GetGuildID();

    // 如果此角色此时无公会，则无需记录，因为他即使分配到了channelID
    // 在登陆 GS 的时候也不会让他进入副本，这样channelID 会再次被收回
    if(!guildID) return;

    // 查找相同的公会ID
    for(dungeonIt = m_dungeonCounter.begin(); dungeonIt != m_dungeonCounter.end(); ++dungeonIt)
    {
        // 匹配副本号和频道号
        if((dungeonIt->dungeonID != dungeonID) || (dungeonIt->channelID != channelID))
        {
            continue;
        }
        break;
    }

    // 是否找到副本的计数信息
    if(dungeonIt != m_dungeonCounter.end())
    {
        // 添加总人数
        CnAssert(dungeonIt->GetPlayerNum() > 0);
        dungeonIt->IncPlayerNum();

        // 如果在列表中查找到这个公会
        for(guildIt = dungeonIt->groupVert.begin(); guildIt != dungeonIt->groupVert.end(); ++guildIt)
        {
            if(guildIt->groupID != guildID) continue;
            break;
        }

        // 若查找到公会信息，则减去预备数，并将角色添加到列表
        if(guildIt != dungeonIt->groupVert.end())
        {
            guildIt->DecPrepareNum();
            guildIt->playerIDList.push_back(playerID);
            return;
        }

        // 未查找到此公会，则添加新的公会成员计数
        playerCounter.Clear();
        playerCounter.groupID = guildID;
        playerCounter.playerIDList.push_back(playerID);

        dungeonIt->groupVert.push_back(playerCounter);

        return; // 退出
    }

    // 添加新的计数时，是不需要考虑 prepareNum 的
    // 因为如果同时同公会的两个成员进入，会进入到不同频道的副本

    // 添加新计数
    dungeonCounter.Clear();
    playerCounter.Clear();

    dungeonCounter.channelID = channelID;
    dungeonCounter.dungeonID = dungeonID;
    dungeonCounter.playerNum = 1;

    playerCounter.groupID = guildID;
    playerCounter.playerIDList.push_back(playerID);

    dungeonCounter.groupVert.push_back(playerCounter);

    m_dungeonCounter.push_back(dungeonCounter);
}

// ==================================================
// 离开公会BOSS副本
// ==================================================
void ChannelManager::OnExitGuildBossDungeon(uint32 channelID, uint32 dungeonID, Player* player)
{
    DungeonCounterVert::iterator dungeonIt;
    uint64 playerID = 0;

    if(!player) return;

    playerID = player->GetPlayerID();

    // 这里不使用 player 的公会ID来再次索引了
    // 因为有可能 Player 在公会副本中战斗的时候，有人将他踢出公会
    // （虽然他还在公会副本中会得到结算，但他已经不在公会中了）
    for( dungeonIt = m_dungeonCounter.begin(); dungeonIt != m_dungeonCounter.end(); ++dungeonIt)
    {
        // 匹配副本号和频道号
        if((dungeonIt->dungeonID != dungeonID) || (dungeonIt->channelID != channelID))
        {
            continue;
        }
        break;
    }

    // 是否找到副本的计数信息
    if(dungeonIt == m_dungeonCounter.end())
    {
        return;
    }

    // 减少总人数
    CnAssert(dungeonIt->GetPlayerNum() > 0);
    dungeonIt->DecPlayerNum();

    // 在工会中
    for(GroupCounterVert::iterator groupIt = dungeonIt->groupVert.begin();
        groupIt != dungeonIt->groupVert.end(); ++groupIt)
    {
        // 检查此公会中是否存在此人
        if(!groupIt->HasPlayer(playerID)) continue;

        // 在列表中删除此人
        groupIt->DelPlayer(playerID);

        break;
    }

    // 如果副本中无人，则删除信息
    if(dungeonIt->GetPlayerNum() < 1)
    {
        m_dungeonCounter.erase(dungeonIt);
    }
}
