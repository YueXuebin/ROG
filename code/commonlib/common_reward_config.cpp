#include "commonlib_pch.h"
#include "common_reward_config.h"
#include "config_reader.h"

IMPLEMENT_SINGLETON(CommonRewardConfig)

CommonRewardConfig::CommonRewardConfig()
{
    Clear();
}

CommonRewardConfig::~CommonRewardConfig()
{

}

void CommonRewardConfig::Clear()
{
    m_common_reward_map.clear();
}

bool CommonRewardConfig::LoadConfig( const char* path )
{
    TiXmlDocument xmlDoc;
    if (!xmlDoc.LoadFile(path))
    {
        CnError("Load file: %s failed\n", path);
        return false;
    }

    TiXmlElement* xmlRoot = xmlDoc.RootElement();
    if(!xmlRoot) return false;

    REWARD_TABLE rewardTable;
    REWARD_ITEM rewardItem;

    // 读取信息
    for(TiXmlElement* xmlEle = xmlRoot->FirstChildElement(); xmlEle; xmlEle = xmlEle->NextSiblingElement())
    {
        if(strcmp(xmlEle->Value(), "reward_list") == 0)
        {
            for(TiXmlElement* rewardEle = xmlEle->FirstChildElement(); rewardEle; rewardEle = rewardEle->NextSiblingElement())
            {
                rewardTable.Clear();
                for(TiXmlElement* itEle = rewardEle->FirstChildElement(); itEle; itEle = itEle->NextSiblingElement())
                {
                    rewardItem.Clear();
                    if(!ConfigReader::LoadRewardItem(itEle, rewardItem))
                    {
                        continue;
                    }

                    rewardTable.items.push_back(rewardItem);
                }

                rewardTable.reason = Crown::SDAtou(rewardEle->Attribute("reason"));

                uint32 rewardId = Crown::SDAtou(rewardEle->Attribute("id"));
                m_common_reward_map.insert(std::make_pair(rewardId, rewardTable));
            }
        }
    }

    return true;
}

const REWARD_TABLE* CommonRewardConfig::GetReward( uint32 rewardId )
{
    CommonRewardMap::iterator iter = m_common_reward_map.find(rewardId);
    if (iter != m_common_reward_map.end())
    {
        return &(iter->second);
    }

    return nullptr;
}
