#include "gameholder_pch.h"
#include "city_survival_config.h"
#include "config_reader.h"
#include "td_config.h"

IMPLEMENT_SINGLETON(CitySurvivalConfig)
CitySurvivalConfig::CitySurvivalConfig()
{
    Clear();
}

CitySurvivalConfig::~CitySurvivalConfig()
{

}

void CitySurvivalConfig::Clear()
{
    //m_reward_map.clear();
    m_baseRewards.clear();
}

bool CitySurvivalConfig::LoadConfig( const char* path )
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
        if(Crown::SDStrcmp(xmlEle->Value(), "base_rewards") == 0)
        {
            if(!LoadBaseRewardConfig(xmlEle))
            {
                return false;
            }
        }

        //if(strcmp(xmlEle->Value(), "wave_reward") == 0)
        //{
        //    for(TiXmlElement* rewardEle = xmlEle->FirstChildElement(); rewardEle; rewardEle = rewardEle->NextSiblingElement())
        //    {
        //        rewardTable.Clear();
        //        for(TiXmlElement* itEle = rewardEle->FirstChildElement(); itEle; itEle = itEle->NextSiblingElement())
        //        {
        //            rewardItem.Clear();
        //            if(!LoadRewardItem(itEle, rewardItem))
        //            {
        //                continue;
        //            }

        //            rewardTable.items.push_back(rewardItem);
        //        }

        //        rewardTable.reason = Crown::SDAtou(rewardEle->Attribute("reason"));

        //        uint32 wave = Crown::SDAtou(rewardEle->Attribute("wave"));
        //        m_reward_map.insert(std::make_pair(wave, rewardTable));
        //    }
        //}
    }

    return true;
}

const REWARD_TABLE* CitySurvivalConfig::GetReward(uint32 wave, uint32 rank)
{
    //SurvivalRewardMap::iterator iter = m_reward_map.find(wave);
    //if (iter != m_reward_map.end())
    //{
    //    return &(iter->second);
    //}

    //return nullptr;

    TdWaveRewardsList::iterator waveIt = m_baseRewards.find(wave);
    if(waveIt == m_baseRewards.end())
    {
        return NULL;
    }

    TdRankRewardList::iterator rankIt = waveIt->second.find(rank);
    if(rankIt == waveIt->second.end())
    {
        return NULL;
    }

    return &rankIt->second.rewardTable;
}

bool CitySurvivalConfig::LoadBaseRewardConfig(TiXmlElement* tdEle)
{
    TdRankRewardList rewardList;
    for(TiXmlElement* baseRewardsEle = tdEle->FirstChildElement(); baseRewardsEle; baseRewardsEle = baseRewardsEle->NextSiblingElement())
    {
        if(Crown::SDStrcmp(baseRewardsEle->Value(), "rewards") == 0)
        {
            rewardList.clear();
            int32 wave = 0;
            baseRewardsEle->QueryIntAttribute("wave", &wave);

            if(!LoadRankReward(baseRewardsEle, rewardList))
            {
                return false;
            }

            if(m_baseRewards.find(wave) != m_baseRewards.end())
            {
                CnError("tower defence wave reward rank:%d is exist\n", wave);
                continue;
            }

            m_baseRewards[wave] = rewardList;
        }
    }
    return true;
}

bool CitySurvivalConfig::LoadRankReward(TiXmlElement* tdEle, TdRankRewardList& rewardList)
{
    TdRankReward rankReward;
    for(TiXmlElement* rankRewardsEle = tdEle->FirstChildElement(); rankRewardsEle; rankRewardsEle = rankRewardsEle->NextSiblingElement())
    {
        if(Crown::SDStrcmp(rankRewardsEle->Value(), "rank_rewards") == 0)
        {
            rankReward.Clear();
            rankRewardsEle->QueryIntAttribute("rank", &rankReward.id);

            REWARD_ITEM rewardItem;
            for(TiXmlElement* rewardEle = rankRewardsEle->FirstChildElement(); rewardEle; rewardEle = rewardEle->NextSiblingElement())
            {
                rewardItem.Clear();
                if(Crown::SDStrcmp(rewardEle->Value(), "item") == 0)
                {
                    if(!ConfigReader::LoadRewardItem(rewardEle, rewardItem))
                    {
                        return false;
                    }

                    rankReward.rewardTable.items.push_back(rewardItem);
                }
            }
            rankReward.rewardTable.reason = REWARD_REASON_TD;

            if(rewardList.find(rankReward.id) != rewardList.end())
            {
                CnError("tower defence rank reward rank:%d is exist\n", rankReward.id);
                continue;
            }
            rewardList[rankReward.id] = rankReward;
        }
    }
    return true;
}
