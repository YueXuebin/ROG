#include "gameholder_pch.h"
#include "ashura_config.h"
#include "game_util.h"

IMPLEMENT_SINGLETON(AshuraConfig)

AshuraConfig::AshuraConfig()
{
    m_ashReward.clear();
    m_refreshTime   = 0;
    m_refreshCount  = 0;
    m_attackUp      = 0;
    m_attackDown    = 0;
    m_killRankReward.clear();
    m_deadRankReward.clear();
}

AshuraConfig::~AshuraConfig()
{

}

bool AshuraConfig::LoadConfig(const char* path)
{
    char file_path[SD_MAX_PATH];

    SDSprintf(file_path, "%s%s", path, ASHURA_CONFIG_FILE);
    if(!LoadAshuraConfig(file_path))
    {
        CnFatal("load %s failed\n", file_path);
        return false;
    }

    SDSprintf(file_path, "%s%s", path, ASHURA_KILL_RANK_REWARD_FILE);
    if(!LoadKillRankConfig(file_path))
    {
        CnFatal("load %s failed\n", file_path);
        return false;
    }

    SDSprintf(file_path, "%s%s", path, ASHURA_DEAD_RANK_REWARD_FILE);
    if(!LoadDeadRankConfig(file_path))
    {
        CnFatal("load %s failed\n", file_path);
        return false;
    }
    return true;
}

bool AshuraConfig::LoadAshuraConfig(const char* path)
{
    REWARD_TABLE rewardTable;
    TiXmlDocument xmlDoc;

    if (!xmlDoc.LoadFile(path))
    {
        CnError("Load item file: %s failed\n", path);
        return false;
    }

    TiXmlElement* xmlRoot = xmlDoc.RootElement();
    if(!xmlRoot) return false;

    for(TiXmlElement* ashuraEle = xmlRoot->FirstChildElement(); ashuraEle; ashuraEle = ashuraEle->NextSiblingElement())
    {
        if(Crown::SDStrcmp(ashuraEle->Value(), "level") == 0)
        {
            if(!LoadLevelReward(ashuraEle))
            {
                return false;
            }
        }
        else if(Crown::SDStrcmp(ashuraEle->Value(), "refresh") == 0)
        {
            ashuraEle->QueryUnsignedAttribute("time", &m_refreshTime);
            ashuraEle->QueryUnsignedAttribute("count", &m_refreshCount);
        }
        else if(Crown::SDStrcmp(ashuraEle->Value(), "attack") == 0)
        {
            if(!LoadAttack(ashuraEle))
            {
                return false;
            }
        }
    }
    return true;
}

bool AshuraConfig::LoadLevelReward(TiXmlElement* ashuraEle)
{
    AshLevelReward ashLevel;
    ashLevel.clear();

    ashuraEle->QueryIntAttribute("level", &ashLevel.m_level);
    ashuraEle->QueryIntAttribute("max_gamepoint", &ashLevel.m_maxGamePoint);
    ashuraEle->QueryIntAttribute("max_soulpoint", &ashLevel.m_maxSoulPoint);
    ashuraEle->QueryIntAttribute("max_exp", &ashLevel.m_maxExp);

    for(TiXmlElement* levelEle = ashuraEle->FirstChildElement(); levelEle; levelEle = levelEle->NextSiblingElement())
    {
        if(Crown::SDStrcmp(levelEle->Value(), "kills") == 0)
        {
            AshRewardList killRewardList;
            killRewardList.clear();

            if(!LoadKillOrDeadReward(levelEle, killRewardList))
            {
                return false;
            }

            ashLevel.m_killRewardList = killRewardList;
        }
        else if(Crown::SDStrcmp(levelEle->Value(), "deads") == 0)
        {
            AshRewardList deadRewardList;
            deadRewardList.clear();

            if(!LoadKillOrDeadReward(levelEle, deadRewardList))
            {
                return false;
            }

            ashLevel.m_deadRewardList= deadRewardList;
        }
    }
    m_ashReward[ashLevel.m_level] = ashLevel;
    return true;
}

bool AshuraConfig::LoadKillOrDeadReward(TiXmlElement* levelEle, AshRewardList& ashRewardList)
{
    levelEle->QueryIntAttribute("gamepoint", &ashRewardList.m_gamepoint);
    levelEle->QueryIntAttribute("soulpoint", &ashRewardList.m_soulpoint);
    levelEle->QueryIntAttribute("exp", &ashRewardList.m_exp);

    REWARD_TABLE rewardTable;
    rewardTable.Clear();
    rewardTable.reason = REWARD_REASON_ASHURAN;
    rewardTable.reason_arg = 2;

    uint32 num = 0;

    for(TiXmlElement* killEle = levelEle->FirstChildElement(); killEle; killEle = killEle->NextSiblingElement())
    {
        rewardTable.Clear();

        killEle->QueryUnsignedAttribute("num", &num);

        if(ashRewardList.m_reward.find(num) != ashRewardList.m_reward.end())
        {
            CnWarn("ashura num %d already exist\n", num);
            continue;
        }

        REWARD_ITEM rewardItem;
        for(TiXmlElement* rewardEle = killEle->FirstChildElement(); rewardEle; rewardEle = rewardEle->NextSiblingElement())
        {
            rewardItem.Clear();

            // ¼ÓÔØ½±ÀøÏî
            if(!LoadRewardItem(rewardEle, rewardItem))
            {
                return false;
            }

            rewardItem.flag = REWARD_FLAG_NORMAL;
            rewardTable.items.push_back(rewardItem);
        }

        ashRewardList.m_reward[num] = rewardTable;
    }

    return true;
}

bool AshuraConfig::LoadAttack(TiXmlElement* attackEle)
{
    attackEle->QueryIntAttribute("up", &m_attackUp);
    attackEle->QueryIntAttribute("down", &m_attackDown);

    return true;
}

const AshLevelReward* AshuraConfig::GetReward(int32 level)
{
    CnAssert(level >= 0);

    AshLevelRewardList::reverse_iterator it = m_ashReward.rbegin();
    for(; it != m_ashReward.rend(); ++it)
    {
        if(level >= it->first)
        {
            return &it->second;
        }
    }

    CnWarn("Warn: level:%d ashura reward is not exist!\n", level);
    return NULL;
}

bool AshuraConfig::LoadKillRankConfig(const char* path)
{
    AshuraRankReward ashuraRankReward;
    TiXmlDocument xmlDoc;
    REWARD_ITEM rewardItem;

    if (!xmlDoc.LoadFile(path))
    {
        CnError("Load item file: %s failed\n", path);
        return false;
    }

    TiXmlElement* xmlRoot = xmlDoc.RootElement();
    if(!xmlRoot) return false;

    for(TiXmlElement* ashuraEle = xmlRoot->FirstChildElement(); ashuraEle; ashuraEle = ashuraEle->NextSiblingElement())
    {
        if(Crown::SDStrcmp("row", ashuraEle->Value()) == 0)
        {
            ashuraRankReward.Clear();
            ashuraEle->QueryIntAttribute("rank", &ashuraRankReward.m_rank);

            std::string stringTable = ashuraEle->Attribute("reward");

            if(!LoadRewardTable(stringTable, ashuraRankReward.m_rewardTable, REWARD_FLAG_RANK1))
            {
                CnError("ashura rewards of rank:%d is error!\n", ashuraRankReward.m_rank);
                return false;
            }

            if(m_killRankReward.find(ashuraRankReward.m_rank) != m_killRankReward.end())
            {
                CnError("kill rank:%d reward is exit!\n", ashuraRankReward.m_rank);
                continue;
            }

            m_killRankReward[ashuraRankReward.m_rank] = ashuraRankReward;
        }
    }

    return true;
}

bool AshuraConfig::LoadDeadRankConfig(const char* path)
{
    AshuraRankReward ashuraRankReward;
    TiXmlDocument xmlDoc;
    REWARD_ITEM rewardItem;

    if (!xmlDoc.LoadFile(path))
    {
        CnError("Load item file: %s failed\n", path);
        return false;
    }

    TiXmlElement* xmlRoot = xmlDoc.RootElement();
    if(!xmlRoot) return false;

    for(TiXmlElement* ashuraEle = xmlRoot->FirstChildElement(); ashuraEle; ashuraEle = ashuraEle->NextSiblingElement())
    {
        if(Crown::SDStrcmp("row", ashuraEle->Value()) == 0)
        {
            ashuraRankReward.Clear();
            ashuraEle->QueryIntAttribute("rank", &ashuraRankReward.m_rank);

            std::string stringTable = ashuraEle->Attribute("reward");

            if(!LoadRewardTable(stringTable, ashuraRankReward.m_rewardTable, REWARD_FLAG_RANK2))
            {
                CnError("ashura rewards of rank:%d is error!\n", ashuraRankReward.m_rank);
                return false;
            }

            if(m_deadRankReward.find(ashuraRankReward.m_rank) != m_deadRankReward.end())
            {
                CnError("kill rank:%d reward is exit!\n", ashuraRankReward.m_rank);
                continue;
            }

            m_deadRankReward[ashuraRankReward.m_rank] = ashuraRankReward;
        }
    }

    return true;
}

const REWARD_TABLE* AshuraConfig::GetKillRankReward(int32 rank)
{
    AshRankRewardList::reverse_iterator rewardIt = m_killRankReward.rbegin();
    for(; rewardIt != m_killRankReward.rend(); ++rewardIt)
    {
        if(rank >= rewardIt->first)
            return &rewardIt->second.m_rewardTable;
    }

    return NULL;
}

const REWARD_TABLE*  AshuraConfig::GetDeadRankReward(int32 rank)
{
    AshRankRewardList::reverse_iterator rewardIt = m_deadRankReward.rbegin();
    for(; rewardIt != m_deadRankReward.rend(); ++rewardIt)
    {
        if(rank >= rewardIt->first)
            return &rewardIt->second.m_rewardTable;
    }

    return NULL;
}
