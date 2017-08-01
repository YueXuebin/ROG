#include "gameholder_pch.h"
#include "td_config.h"
#include "game_util.h"

IMPLEMENT_SINGLETON(TdConfig)

TdConfig::TdConfig()
{
    m_startTime = 0;
    m_baseRewards.clear();
}

TdConfig::~TdConfig()
{

}

bool TdConfig::LoadConfig(const char* path)
{
    TiXmlDocument xmlDoc;

    if (!xmlDoc.LoadFile(path))
    {
        CnError("Load task file: %s failed\n", path);
        return false;
    }

    TiXmlElement* xmlRoot = xmlDoc.RootElement();
    if(!xmlRoot) return false;

    for(TiXmlElement* tdEle = xmlRoot->FirstChildElement(); tdEle; tdEle = tdEle->NextSiblingElement())
    {
        if(Crown::SDStrcmp(tdEle->Value(), "tower") == 0)
        {
            tdEle->QueryIntAttribute("start_time", &m_startTime);
            m_startTime *= 1000;
        }
        else if(Crown::SDStrcmp(tdEle->Value(), "base_rewards") == 0)
        {
            if(!LoadBaseRewardConfig(tdEle))
            {
                return false;
            }
        }
    }

    return true;
}

bool TdConfig::LoadRankReward(TiXmlElement* tdEle, TdRankRewardList& rewardList)
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
                    if(!LoadRewardItem(rewardEle, rewardItem))
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

bool TdConfig::LoadBaseRewardConfig(TiXmlElement* tdEle)
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

const REWARD_TABLE* TdConfig::GetRewards(uint32 wave, int32 rank)
{
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


