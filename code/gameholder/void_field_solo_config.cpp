#include "gameholder_pch.h"
#include "void_field_solo_config.h"
#include "config_reader.h"
#include "game_util.h"


IMPLEMENT_SINGLETON(VoidFieldSoloConfig)

VoidFieldSoloConfig::VoidFieldSoloConfig()
{
    Clear();
}

VoidFieldSoloConfig::~VoidFieldSoloConfig()
{

}

void VoidFieldSoloConfig::Clear()
{
    m_player_level_need                 = 0;
    m_boss_broadcast_id                 = "";
    m_regular_boss_run_time             = 0;
    m_resurgence_time                   = 0;
    m_normal_chests_rate                = 0;
    m_boss_chests_rate                  = 0;
    m_boss_player_dead_rate             = 0;
    m_boss_overrange_rate               = 0;
    m_boss_overrange_time               = 0;
    m_rare_boss_id                      = 0;
    m_outscope_notify_interval          = 1;
    m_channel_max_player                = 0;
    m_boss_killed_levelup_seconds       = 0;

    m_boss_topone_rewards.Clear();
    m_boss_topone_guild_rewards.Clear();
    m_boss_killer_rewards.Clear();
    m_chests_items.clear();
    m_diff_level_drop_map.clear();
}

bool VoidFieldSoloConfig::LoadConfig(const char* path)
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
        if (strcmp(xmlEle->Value(), "player_level_need") == 0)
        {
            ConfigReader::Read(xmlEle, "value", m_player_level_need);
        }
        else if(strcmp(xmlEle->Value(), "boss_broadcast_id") == 0)
        {
            ConfigReader::Read(xmlEle, "value", m_boss_broadcast_id);
        }
        else if(strcmp(xmlEle->Value(), "outscope_notify_interval") == 0)
        {
            ConfigReader::Read(xmlEle, "value", m_outscope_notify_interval);
        }
        else if(strcmp(xmlEle->Value(), "channel_max_player") == 0)
        {
            ConfigReader::Read(xmlEle, "value", m_channel_max_player);
        }
        else if(strcmp(xmlEle->Value(), "regular_boss_run_time") == 0)
        {
            ConfigReader::Read(xmlEle, "value", m_regular_boss_run_time);
        }
        else if (strcmp(xmlEle->Value(), "resurgence_time") == 0)
        {
            ConfigReader::Read(xmlEle, "value", m_resurgence_time);
        }
        else if(strcmp(xmlEle->Value(), "regular_normal_chests_rate") == 0)
        {
            ConfigReader::Read(xmlEle, "value", m_normal_chests_rate);
        }
        else if(strcmp(xmlEle->Value(), "regular_boss_chests_rate") == 0)
        {
            ConfigReader::Read(xmlEle, "value", m_boss_chests_rate);
        }
        else if (strcmp(xmlEle->Value(), "regular_boss_overrange_rate") == 0)
        {
            ConfigReader::Read(xmlEle, "value", m_boss_overrange_rate);
            if(m_boss_overrange_rate < BASE_RATE_NUM)
                m_boss_overrange_rate = BASE_RATE_NUM - m_boss_overrange_rate;
        }
        else if (strcmp(xmlEle->Value(), "regular_boss_player_dead_rate") == 0)
        {
            ConfigReader::Read(xmlEle, "value", m_boss_player_dead_rate);
            if(m_boss_player_dead_rate < BASE_RATE_NUM)
                m_boss_player_dead_rate = BASE_RATE_NUM - m_boss_player_dead_rate;
        }
        else if (strcmp(xmlEle->Value(), "regular_boss_overrange_time") == 0)
        {
            ConfigReader::Read(xmlEle, "value", m_boss_overrange_time);
        }
        else if(strcmp(xmlEle->Value(), "rare_boss_id") == 0)
        {
            ConfigReader::Read(xmlEle, "value", m_rare_boss_id);
        }
        else if(strcmp(xmlEle->Value(), "regular_normal_exp") == 0)
        {
            LoadRewardExpItem(xmlEle, VOID_FIELD_TYPE_NORMAL);
        }
        else if(strcmp(xmlEle->Value(), "regular_boss_exp") == 0)
        {
            LoadRewardExpItem(xmlEle, VOID_FIELD_TYPE_BOSS);
        }
        else if(strcmp(xmlEle->Value(), "diff_level_rates") == 0)
        {
            LoadDiffLevelDropRate(xmlEle);
        }
        else if (strcmp(xmlEle->Value(), "regular_boss_top_one_rewards") == 0)
        {
            LoadRewardTable(xmlEle, m_boss_topone_rewards);
        }
        else if (strcmp(xmlEle->Value(), "regular_boss_guild_rewards") == 0)
        {
            LoadRewardTable(xmlEle, m_boss_topone_guild_rewards);
        }
        else if (strcmp(xmlEle->Value(), "regular_boss_killer_rewards") == 0)
        {
            LoadRewardTable(xmlEle, m_boss_killer_rewards);
        }
        else if(strcmp(xmlEle->Value(), "chests_items") == 0)
        {
            LoadChestsItems(xmlEle);
        }
        else if(strcmp(xmlEle->Value(), "drop_items") == 0)
        {
            LoadDropItems(xmlEle);
        }
        else if(strcmp(xmlEle->Value(), "updatetime") == 0)
        {
            std::string TimeString;
            TimeString = xmlEle->Attribute("value");
            update_time.FromString(TimeString);
        }
        else if (strcmp(xmlEle->Value(), "chests_probability") == 0)
        {
            LoadChestsProbability(xmlEle);
        }
        else if (strcmp(xmlEle->Value(), "boss_killed_levelup_seconds") == 0)
        {
            ConfigReader::Read(xmlEle, "value", m_boss_killed_levelup_seconds);
        }
    }

    return true;
}

void VoidFieldSoloConfig::LoadChestsItems(TiXmlElement* xmlEle)
{
    RAND_REWARD_ITEM randRewardItem;

    for(TiXmlElement* itEle = xmlEle->FirstChildElement(); itEle; itEle = itEle->NextSiblingElement())
    {
        randRewardItem.Clear();

        if(!LoadRandRewardItem(itEle, randRewardItem))
        {
            continue;
        }

        m_chests_items.push_back(randRewardItem);
    }
}

//加载宝箱奖掉落概率跟随时间降低的概率
void VoidFieldSoloConfig::LoadChestsProbability(TiXmlElement* xmlEle)
{
    for(TiXmlElement* itEle = xmlEle->FirstChildElement(); itEle; itEle = itEle->NextSiblingElement())
    {
        uint32 time = 0;
        uint32 lower_probility = 0;
        ConfigReader::Read(itEle, "time", time);
        ConfigReader::Read(itEle, "lower", lower_probility);
        
        if((m_chestsProbabilityMap.find(time) == m_chestsProbabilityMap.end()))
        {
              m_chestsProbabilityMap[time] = lower_probility;
        }
        else
        {
            CnWarn("Duplicate void field chestprobability:  %u\n", time);
        }
    }

    if(m_chestsProbabilityMap.size() < 0)
    {
        CnWarn("have not chestprobability \n");
    }

}
//得到掉落经过多长时间的掉落概率
std::map<uint32, int32>& VoidFieldSoloConfig::GetChestsProbabilityByTime()
{
    return m_chestsProbabilityMap;
}

void VoidFieldSoloConfig::LoadDropItems(TiXmlElement* xmlEle)
{
    RAND_REWARD_ITEM randRewardItem;

    for(TiXmlElement* itEle = xmlEle->FirstChildElement(); itEle; itEle = itEle->NextSiblingElement())
    {
        randRewardItem.Clear();

        if(!LoadRandRewardItem(itEle, randRewardItem))
        {
            continue;
        }

        m_drop_items.push_back(randRewardItem);
    }
}

void VoidFieldSoloConfig::LoadDiffLevelDropRate(TiXmlElement* xmlEle)
{
    int32 diff_level = 0;
    int32 drop_rate  = 0;

    for(TiXmlElement* itEle = xmlEle->FirstChildElement(); itEle; itEle = itEle->NextSiblingElement())
    {
        ConfigReader::Read(itEle, "diff_level", diff_level);
        ConfigReader::Read(itEle, "rate", drop_rate);
        m_diff_level_drop_map[diff_level] = drop_rate;
    }
}

int32 VoidFieldSoloConfig::GetRewardExpInterval(int32 state_type)
{
    RewardExpItemMap::const_iterator iter = m_reward_expitems.find(state_type);
    if (iter == m_reward_expitems.end())
    {
        CnAssert(false);
        return 0;
    }

    return iter->second.interval;
}

int32 VoidFieldSoloConfig::GetRewardExpNum(int32 state_type)
{
    RewardExpItemMap::const_iterator iter = m_reward_expitems.find(state_type);
    if (iter == m_reward_expitems.end())
    {
        CnAssert(false);
        return 0;
    }

    return iter->second.exp;
}

// 获得宝箱随机物品
REWARD_ITEM* VoidFieldSoloConfig::GetChestsRandItem(int32 num)
{
    int32 totalNum = 0;

    for(std::vector<RAND_REWARD_ITEM>::iterator it = m_chests_items.begin();
        it != m_chests_items.end(); ++it)
    {
        totalNum += it->rate;
        if(num < totalNum)
            return &(it->item);
    }

    return NULL;
}

REWARD_ITEM* VoidFieldSoloConfig::GetDropRandItem(int32 num)
{
    int32 totalNum = 0;

    for(std::vector<RAND_REWARD_ITEM>::iterator it = m_drop_items.begin();
        it != m_drop_items.end(); ++it)
    {
        totalNum += it->rate;
        if(num < totalNum)
            return &(it->item);
    }

    return NULL;
}


int32 VoidFieldSoloConfig::GetDiffLevelDropRate(int32 diff_level)
{
    if (diff_level <= 0)
    {
        return BASE_RATE_NUM;
    }

    DiffLevelDropRateMap::const_iterator iter;
    for (iter = m_diff_level_drop_map.begin(); iter != m_diff_level_drop_map.end(); ++iter)
    {
        if (diff_level <= iter->first)
        {
            return iter->second;
        }
    }

    return 0;
}

bool VoidFieldSoloConfig::LoadRewardExpItem(TiXmlElement* xmlEle, int32 state_type)
{
    REWARD_EXP_ITEM exp_item;
    exp_item.Clear();

    ConfigReader::Read(xmlEle, "interval", exp_item.interval);
    ConfigReader::Read(xmlEle, "exp", exp_item.exp);

    m_reward_expitems[state_type] = exp_item;

    return true;
}
