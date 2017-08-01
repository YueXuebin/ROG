#include "gameholder_pch.h"
#include "group_pk_config.h"
#include "game_util.h"

IMPLEMENT_SINGLETON(GroupPkConfig)

GroupPkConfig::GroupPkConfig()
{

}

GroupPkConfig::~GroupPkConfig()
{

}

bool GroupPkConfig::LoadConfig(const char* path)
{
    TiXmlDocument xmlDoc;

    if (!xmlDoc.LoadFile(path))
    {
        CnError("Load group pk file: %s failed\n", path);
        return false;
    }

    TiXmlElement* xmlRoot = xmlDoc.RootElement();
    if(!xmlRoot) return false;

    bool b_find = false;

    for(TiXmlElement* groupEle = xmlRoot->FirstChildElement(); groupEle; groupEle = groupEle->NextSiblingElement())
    {
        if(Crown::SDStrcmp(groupEle->Value(), "score") == 0)
        {
            if(!LoadScore(groupEle))
                return false;
        }
        else if(Crown::SDStrcmp(groupEle->Value(), "rewards") == 0)
        {
            if(!LoadRewards(groupEle))
                return false;
        }
    }

    return true;
}

bool GroupPkConfig::LoadScore(TiXmlElement* groupEle)
{
    GPKkillScore killScore;
    for(TiXmlElement* scoreEle = groupEle->FirstChildElement(); scoreEle; scoreEle = scoreEle->NextSiblingElement())
    {
        if(Crown::SDStrcmp(scoreEle->Value(), "combo_kill") == 0)
        {
            for(TiXmlElement* comboEle = scoreEle->FirstChildElement(); comboEle; comboEle = comboEle->NextSiblingElement())
            {
                if(Crown::SDStrcmp(comboEle->Value(), "item") == 0)
                {
                    killScore.Clear();
                    comboEle->QueryIntAttribute("num", &killScore.num);
                    comboEle->QueryIntAttribute("player_score", &killScore.player_score);
                    comboEle->QueryIntAttribute("group_score", &killScore.group_score);
                    m_comboKill[killScore.num] = killScore;
                }
            }
        }
        else if(Crown::SDStrcmp(scoreEle->Value(), "kill_total") == 0)
        {
            for(TiXmlElement* killTotalEle = scoreEle->FirstChildElement(); killTotalEle; killTotalEle = killTotalEle->NextSiblingElement())
            {
                if(Crown::SDStrcmp(killTotalEle->Value(), "item") == 0)
                {
                    killScore.Clear();
                    killTotalEle->QueryIntAttribute("num", &killScore.num);
                    killTotalEle->QueryIntAttribute("player_score", &killScore.player_score);
                    killTotalEle->QueryIntAttribute("group_score", &killScore.group_score);
                    m_killTotal[killScore.num] = killScore;
                }
            }
        }
        else if(Crown::SDStrcmp(scoreEle->Value(), "time") == 0)
        {
            scoreEle->QueryIntAttribute("second", &m_scoreTimeSecond);
            m_scoreTimeSecond *= 1000;
            scoreEle->QueryIntAttribute("player_score", &m_scoreTimePlayerScore);
        }
        else if(Crown::SDStrcmp(scoreEle->Value(), "combo_stop") == 0)
        {
            scoreEle->QueryIntAttribute("combo_num", &m_comboStopNum);
            scoreEle->QueryIntAttribute("player_score", &m_comboStopPlayerScore);
            scoreEle->QueryIntAttribute("player_score", &m_comboStopGroupScore);
        }
    }

    return true;
}

bool GroupPkConfig::LoadRewards(TiXmlElement* groupEle)
{
    int32 score = 0;
    REWARD_TABLE    rewardTable;
    REWARD_ITEM     rewardItem;
    for(TiXmlElement* rewardsEle = groupEle->FirstChildElement(); rewardsEle; rewardsEle = rewardsEle->NextSiblingElement())
    {
        if(Crown::SDStrcmp(rewardsEle->Value(), "score_rewards") == 0)
        {
            score = 0;
            rewardsEle->QueryIntAttribute("score", &score);

            if(m_rewards.find(score) != m_rewards.end())
            {
                CnError("score_rewards score:%d is exist\n", score);
                continue;
            }

            for(TiXmlElement* scoreRewardsEle = rewardsEle->FirstChildElement(); scoreRewardsEle; scoreRewardsEle = scoreRewardsEle->NextSiblingElement())
            {
                rewardItem.Clear();
                if(!LoadRewardItem(scoreRewardsEle, rewardItem))
                {
                    return false;
                }
                rewardTable.items.push_back(rewardItem);
            }

            m_rewards[score] = rewardTable;
        }
    }

    return true;
}

const GPKkillScore* GroupPkConfig::GetComboKillScore(int32 combo)
{
    // 倒序查找
    for(std::map<int32, GPKkillScore>::reverse_iterator comboIt = m_comboKill.rbegin(); comboIt != m_comboKill.rend(); ++comboIt)
    {
        if(combo >= comboIt->second.num)
        {
            return &comboIt->second;
        }
    }

    return NULL;
}

const GPKkillScore* GroupPkConfig::GetKillTotalScore(int32 killTotal)
{
    // 倒序查找
    for(std::map<int32, GPKkillScore>::reverse_iterator totalIt = m_killTotal.rbegin(); totalIt != m_killTotal.rend(); ++totalIt)
    {
        if(killTotal >= totalIt->second.num)
        {
            return &totalIt->second;
        }
    }

    return NULL;
}

const REWARD_TABLE* GroupPkConfig::GetRewards(int32 score)
{
    // 倒序查找
    for(std::map<int32, REWARD_TABLE>::reverse_iterator rewardIt = m_rewards.rbegin(); rewardIt != m_rewards.rend(); ++rewardIt)
    {
        if(score >= rewardIt->first)
        {
            return &rewardIt->second;
        }
    }

    return NULL;
}
