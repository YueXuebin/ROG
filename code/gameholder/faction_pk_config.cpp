#include "gameholder_pch.h"
#include "faction_pk_config.h"
#include "game_util.h"

IMPLEMENT_SINGLETON(FactionPkConfig)

FactionPkConfig::FactionPkConfig()
{

}

FactionPkConfig::~FactionPkConfig()
{

}

bool FactionPkConfig::LoadConfig(const char* path)
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
        else if(Crown::SDStrcmp(groupEle->Value(), "score_rewards") == 0)
        {
            if(!LoadScoreRewards(groupEle))
                return false;
        }
        else if(Crown::SDStrcmp(groupEle->Value(), "faction_rewards") == 0)
        {
            if(!LoadFactionRewards(groupEle))
                return false;
        }
    }

    return true;
}

bool FactionPkConfig::LoadScore(TiXmlElement* groupEle)
{
    FactionPKKillScore killScore;
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
            scoreEle->QueryIntAttribute("group_score", &m_comboStopGroupScore);
        }
    }

    return true;
}

bool FactionPkConfig::LoadScoreRewards(TiXmlElement* groupEle)
{
    int32 rank = 0;
    REWARD_TABLE    rewardTable;
    REWARD_ITEM     rewardItem;
    for(TiXmlElement* rewardsEle = groupEle->FirstChildElement(); rewardsEle; rewardsEle = rewardsEle->NextSiblingElement())
    {
        if(Crown::SDStrcmp(rewardsEle->Value(), "score_rewards") == 0)
        {
            rewardTable.Clear();
            rank = 0;
            rewardsEle->QueryIntAttribute("rank", &rank);

            if(m_rewards.find(rank) != m_rewards.end())
            {
                CnError("score_rewards rank:%d is exist\n", rank);
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

            m_rewards[rank] = rewardTable;
        }
    }

    return true;
}

bool FactionPkConfig::LoadFactionRewards(TiXmlElement* groupEle)
{
    REWARD_ITEM     rewardItem;
    for(TiXmlElement* rewardsEle = groupEle->FirstChildElement(); rewardsEle; rewardsEle = rewardsEle->NextSiblingElement())
    {
        if(Crown::SDStrcmp(rewardsEle->Value(), "success") == 0)
        {
            for(TiXmlElement* scoreRewardsEle = rewardsEle->FirstChildElement(); scoreRewardsEle; scoreRewardsEle = scoreRewardsEle->NextSiblingElement())
            {
                rewardItem.Clear();
                if(!LoadRewardItem(scoreRewardsEle, rewardItem))
                {
                    return false;
                }
                m_successRewards.items.push_back(rewardItem);
            }
        }
        else if(Crown::SDStrcmp(rewardsEle->Value(), "false") == 0)
        {
            for(TiXmlElement* scoreRewardsEle = rewardsEle->FirstChildElement(); scoreRewardsEle; scoreRewardsEle = scoreRewardsEle->NextSiblingElement())
            {
                rewardItem.Clear();
                if(!LoadRewardItem(scoreRewardsEle, rewardItem))
                {
                    return false;
                }
                m_falseRewards.items.push_back(rewardItem);
            }
        }
    }

    return true;
}

const FactionPKKillScore* FactionPkConfig::GetComboKillScore(int32 combo)
{
    // 倒序查找
    for(std::map<int32, FactionPKKillScore>::reverse_iterator comboIt = m_comboKill.rbegin(); comboIt != m_comboKill.rend(); ++comboIt)
    {
        if(combo >= comboIt->second.num)
        {
            return &comboIt->second;
        }
    }

    return NULL;
}

const FactionPKKillScore* FactionPkConfig::GetKillTotalScore(int32 killTotal)
{
    // 倒序查找
    for(std::map<int32, FactionPKKillScore>::reverse_iterator totalIt = m_killTotal.rbegin(); totalIt != m_killTotal.rend(); ++totalIt)
    {
        if(killTotal >= totalIt->second.num)
        {
            return &totalIt->second;
        }
    }

    return NULL;
}

const REWARD_TABLE* FactionPkConfig::GetRankRewards(int32 rank)
{
    // 倒序查找
    for(std::map<int32, REWARD_TABLE>::reverse_iterator rewardIt = m_rewards.rbegin(); rewardIt != m_rewards.rend(); ++rewardIt)
    {
        if(rank >= rewardIt->first)
        {
            return &rewardIt->second;
        }
    }

    return NULL;
}

const REWARD_TABLE* FactionPkConfig::GetFactionReward(bool win)
{
    if(win)
    {
        return &m_successRewards;
    }
    else
    {
        return &m_falseRewards;
    }
    return NULL;
}
