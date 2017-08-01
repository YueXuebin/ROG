#include "gameholder_pch.h"

#include "world_map_config.h"
#include "config_reader.h"
#include "game_util.h"

IMPLEMENT_SINGLETON(WorldMapConfig)

WorldMapConfig::WorldMapConfig()
{
    Clear();
}

WorldMapConfig::~WorldMapConfig()
{

}

void WorldMapConfig::Clear()
{
    m_boss_map.clear(); 
    m_leve_scene_boss_over_time = 0;
    m_leve_scene_boss_over_rate = 0;
    m_boss_player_dead_rate = 0;
    m_boss_player_dead_rate = 0;
    m_boss_dead_pick_time = 0;
    m_boss_dead_life_time = 0;
    m_boss_corpse_pick_time = 0;
    m_boss_corpse_life_time = 0;
    m_kill_moster_pick_time = 0;
    m_kill_moster_life_time = 0;
}

bool WorldMapConfig::LoadConfig(const char* path)
{
    TiXmlDocument xmlDoc;

    if (!xmlDoc.LoadFile(path))
    {
        CnError("Load item file: %s failed\n", path);
        return false;
    }

    TiXmlElement* xmlRoot = xmlDoc.RootElement();
    if(!xmlRoot) return false;

    for(TiXmlElement* xmlEle = xmlRoot->FirstChildElement(); xmlEle; xmlEle = xmlEle->NextSiblingElement())
    {
        if(strcmp(xmlEle->Value(), "field_boss") == 0)
        {
            LoadBossBaseConfig(xmlEle);
        }
        else if(strcmp(xmlEle->Value(), "boss_refresh_time") == 0)
        {
            LoadRefreshTimeConfig(xmlEle);
        }
        else if(strcmp(xmlEle->Value(), "boss_dead_drop_items") == 0)
        {
            LoadBossDeadDropItemsConfig(xmlEle);
        }
        else if(strcmp(xmlEle->Value(), "boss_corpse_drop_items") == 0)
        {
            LoadBossCorpseDropItemsConfig(xmlEle);
        }
        else if(strcmp(xmlEle->Value(), "monster_dead_drop_items") == 0)
        {
            LoadMonsterDeadDropItemsConfig(xmlEle);
        }
        else if(strcmp(xmlEle->Value(), "damage_rank_reward_items") == 0)
        {
            LoadDamageRankRewardItemsConfig(xmlEle);
        }
        else if (strcmp(xmlEle->Value(), "leve_scene_boss_over_time") == 0)
        {
            ConfigReader::Read(xmlEle, "value", m_leve_scene_boss_over_time);
        }
        else if (strcmp(xmlEle->Value(), "leve_scene_boss_over_rate") == 0)
        {
           ConfigReader::Read(xmlEle, "value", m_leve_scene_boss_over_rate);
        }
        else if (strcmp(xmlEle->Value(), "boss_dead_pick_time") == 0)
        {
            ConfigReader::Read(xmlEle, "value", m_leve_scene_boss_over_rate);
        }
        else if (strcmp(xmlEle->Value(), "boss_dead_life_time") == 0)
        {
            ConfigReader::Read(xmlEle, "value", m_boss_dead_life_time);
        }
        else if (strcmp(xmlEle->Value(), "boss_corpse_pick_time") == 0)
        {
            ConfigReader::Read(xmlEle, "value", m_boss_corpse_pick_time);
        }
        else if (strcmp(xmlEle->Value(), "boss_corpse_life_time") == 0)
        {
            ConfigReader::Read(xmlEle, "value", m_boss_corpse_life_time);
        }
        else if (strcmp(xmlEle->Value(), "kill_moster_pick_time") == 0)
        {
            ConfigReader::Read(xmlEle, "value", m_kill_moster_pick_time);
        }
        else if (strcmp(xmlEle->Value(), "kill_moster_life_time") == 0)
        {
            ConfigReader::Read(xmlEle, "value", m_kill_moster_life_time);
        }
        else if (strcmp(xmlEle->Value(), "boss_player_dead_rate") == 0)
        {
            ConfigReader::Read(xmlEle, "value", m_boss_player_dead_rate);
        }
        else if(strcmp(xmlEle->Value(), "hook_exp") == 0)
        {
            ConfigReader::Read(xmlEle, "interval", m_hook_exp_interval);
            ConfigReader::Read(xmlEle, "exp", m_hook_exp_value);
        }
        else if(strcmp(xmlEle->Value(), "boss_leak") == 0)
        {
            ConfigReader::Read(xmlEle, "exp_multi", m_boss_leak_exp_multi);
            ConfigReader::Read(xmlEle, "duration", m_boss_leak_duration);
        }
        else if(strcmp(xmlEle->Value(), "boss_scourge_corpse") == 0)
        {
            ConfigReader::Read(xmlEle, "duration", m_boss_scourge_corpse_duration);
        }
    }

    return true;
}

bool WorldMapConfig::LoadBossBaseConfig(TiXmlElement* xmlEle)
{
    BOSS_ITEM bossItem;

    for(TiXmlElement* itEle = xmlEle->FirstChildElement(); itEle; itEle = itEle->NextSiblingElement())
    {
        bossItem.Clear();
        ConfigReader::Read(itEle, "dungeon_id", bossItem.dungeon_id);
        ConfigReader::Read(itEle, "scene_id", bossItem.scene_id);
        ConfigReader::Read(itEle, "boss_id", bossItem.boss_id);
        ConfigReader::Read(itEle, "boss_name", bossItem.boss_name);
        ConfigReader::Read(itEle, "enter_level", bossItem.enter_level);

        int32 isopen = 0;
        ConfigReader::Read(itEle, "isopen", isopen);

        if(isopen)
            m_boss_map[bossItem.boss_id] = bossItem;
    }

    return true;
}

bool WorldMapConfig::LoadRefreshTimeConfig(TiXmlElement* xmlEle)
{
    uint32 order = 0;
    std::string time_point = "";
    for(TiXmlElement* itEle = xmlEle->FirstChildElement(); itEle; itEle = itEle->NextSiblingElement())
    {
        
        ConfigReader::Read(itEle, "order", order);
        ConfigReader::Read(itEle, "time_point", time_point);
        m_refresh_time_map[order] = time_point;
    }

    return true;
}

bool WorldMapConfig::LoadBossDeadDropItemsConfig(TiXmlElement* xmlEle)
{
    RandRewardVec rand_rewards;
    for(TiXmlElement* bossEle = xmlEle->FirstChildElement(); bossEle; bossEle = bossEle->NextSiblingElement())
    {
        rand_rewards.clear();
        if(strcmp(bossEle->Value(), "boss") == 0)
        {
            uint32 boss_id = 0;
            ConfigReader::Read(bossEle, "id", boss_id);

            RAND_REWARD_ITEM randRewardItem;
            for(TiXmlElement* itEle = bossEle->FirstChildElement(); itEle; itEle = itEle->NextSiblingElement())
            {
                randRewardItem.Clear();

                if(!LoadRandRewardItem(itEle, randRewardItem))
                {
                    continue;
                }

                rand_rewards.push_back(randRewardItem);
            }

            m_boss_dead_drop_items[boss_id] = rand_rewards;
        }
    }

    return true;
}

bool WorldMapConfig::LoadBossCorpseDropItemsConfig(TiXmlElement* xmlEle)
{
    RandRewardVec rand_rewards;
    for(TiXmlElement* bossEle = xmlEle->FirstChildElement(); bossEle; bossEle = bossEle->NextSiblingElement())
    {
        rand_rewards.clear();
        if(strcmp(bossEle->Value(), "boss") == 0)
        {
            uint32 boss_id = 0;
            ConfigReader::Read(bossEle, "id", boss_id);

            RAND_REWARD_ITEM randRewardItem;
            for(TiXmlElement* itEle = bossEle->FirstChildElement(); itEle; itEle = itEle->NextSiblingElement())
            {
                randRewardItem.Clear();

                if(!LoadRandRewardItem(itEle, randRewardItem))
                {
                    continue;
                }

                rand_rewards.push_back(randRewardItem);
            }

            m_boss_corpse_drop_items[boss_id] = rand_rewards;
        }
    }

    return true;
}

bool WorldMapConfig::LoadMonsterDeadDropItemsConfig(TiXmlElement* xmlEle)
{
    RAND_REWARD_ITEM randRewardItem;

    for(TiXmlElement* itEle = xmlEle->FirstChildElement(); itEle; itEle = itEle->NextSiblingElement())
    {
        randRewardItem.Clear();

        if(!LoadRandRewardItem(itEle, randRewardItem))
        {
            continue;
        }

        m_monster_dead_drop_items.push_back(randRewardItem);
    }

    return true;
}

bool WorldMapConfig::LoadDamageRankRewardItemsConfig(TiXmlElement* xmlEle)
{
    for(TiXmlElement* bossEle = xmlEle->FirstChildElement(); bossEle; bossEle = bossEle->NextSiblingElement())
    {
        if(strcmp(xmlEle->Value(), "boss") == 0)
        {
            uint32 boss_id = 0;
            RankRewarMap rand_reward_map;
            ConfigReader::Read(bossEle, "id", boss_id);

            RankRewarMap rank_map;
            for(TiXmlElement* itEle = xmlEle->FirstChildElement(); itEle; itEle = itEle->NextSiblingElement())
            {
                rank_map.clear();
                if(strcmp(xmlEle->Value(), "rank") == 0)
                {
                    uint32 rank = 0;
                    ConfigReader::Read(itEle, "value", rank);

                    REWARD_TABLE reward_table;
                    reward_table.Clear();
                    LoadRewardTable(xmlEle, reward_table);

                    rank_map[rank] = reward_table;
                }
            }

            m_damage_rank_reward_items[boss_id] = rand_reward_map;
        }
    }

    return true;
}

uint32 WorldMapConfig::GetBossIDByUnionID(uint32 unionID) const
{
    uint32 dungeonID = DUNGEON_ID(unionID);
    uint32 sceneID = SCENE_ID(unionID);

    BossItemMap::const_iterator bossIter;
    for (bossIter = m_boss_map.begin(); bossIter != m_boss_map.end(); ++bossIter)
    {
        if (bossIter->second.dungeon_id == dungeonID && bossIter->second.scene_id == sceneID)
        {
            return bossIter->second.boss_id;
        }
    }

    return 0;
}

const REWARD_ITEM* WorldMapConfig::GetBossDeadRandItem(uint32 bossID, int32 num) const
{
    BossRandRewardMap::const_iterator bossIter = m_boss_dead_drop_items.find(bossID);
    if (bossIter == m_boss_dead_drop_items.end())
    {
        return NULL;
    }

    const RandRewardVec& rand_rewards = bossIter->second;
    int32 totalNum = 0;

    for(RandRewardVec::const_iterator it = rand_rewards.begin();
        it != rand_rewards.end(); ++it)
    {
        totalNum += it->rate;
        if(num < totalNum)
            return &(it->item);
    }

    return NULL;
}

const REWARD_ITEM* WorldMapConfig::GetBossCorpseRandItem(uint32 bossID, int32 num) const
{
    BossRandRewardMap::const_iterator bossIter = m_boss_corpse_drop_items.find(bossID);
    if (bossIter == m_boss_corpse_drop_items.end())
    {
        return NULL;
    }

    const RandRewardVec& rand_rewards = bossIter->second;
    int32 totalNum = 0;

    for(RandRewardVec::const_iterator it = rand_rewards.begin();
        it != rand_rewards.end(); ++it)
    {
        totalNum += it->rate;
        if(num < totalNum)
            return &(it->item);
    }

    return NULL;
}

const REWARD_TABLE* WorldMapConfig::GetDamageRankRewardItems(uint32 bossID, uint32 rank) const
{
    BossRankRewardMap::const_iterator bossIter = m_damage_rank_reward_items.find(bossID);
    if (bossIter == m_damage_rank_reward_items.end())
    {
        return NULL;
    }

    RankRewarMap::const_iterator rankIter = bossIter->second.find(rank);
    if (rankIter == bossIter->second.end())
    {
        return NULL;
    }

    return &(rankIter->second);
}

const REWARD_ITEM* WorldMapConfig::GetMonsterDeadRandItem(int32 num) const
{
    int32 totalNum = 0;

    for(std::vector<RAND_REWARD_ITEM>::const_iterator it = m_monster_dead_drop_items.begin(); it != m_monster_dead_drop_items.end(); ++it)
    {
        totalNum += it->rate;
        if(num < totalNum)
            return &(it->item);
    }

    return NULL;
}

