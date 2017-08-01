#include "gameholder_pch.h"
#include "capture_flag_config.h"
#include "game_util.h"

IMPLEMENT_SINGLETON(CaptureFlagConfig)

CaptureFlagConfig::CaptureFlagConfig()
{
    m_flagsScoreList.clear();
    m_time = 0;
    m_killscore = 0;
    m_capture_score = 0;
    m_RewardList.clear();
    m_successRewardList.Clear();
    m_falseRewardList.Clear();
}

CaptureFlagConfig::~CaptureFlagConfig()
{

}

bool CaptureFlagConfig::LoadConfig(const char* path)
{
    TiXmlDocument xmlDoc;

    if (!xmlDoc.LoadFile(path))
    {
        CnError("Load capture file: %s failed\n", path);
        return false;
    }

    TiXmlElement* xmlRoot = xmlDoc.RootElement();
    if(!xmlRoot) 
        return false;

    for(TiXmlElement* rootEle = xmlRoot->FirstChildElement(); rootEle; rootEle = rootEle->NextSiblingElement())
    {
        if(Crown::SDStrcmp(rootEle->Value(), "group_score") == 0)
        {
            if(!LoadGroupScore(rootEle))
                return false;
        }
        else if(Crown::SDStrcmp(rootEle->Value(), "player_score") == 0)
        {
            rootEle->QueryIntAttribute("kill", &m_killscore);
            rootEle->QueryIntAttribute("capture_flag", &m_capture_score);
        }
        else if(Crown::SDStrcmp(rootEle->Value(), "score_rewards") == 0)
        {
            if(!LoadScoreRewards(rootEle))
                return false;
        }
        else if(Crown::SDStrcmp(rootEle->Value(), "group_rewards") == 0)
        {
            if(!LoadGroupRewards(rootEle))
                return false;
        }
    }

    return true;
}

bool CaptureFlagConfig::LoadGroupScore(TiXmlElement* rootEle)
{
    FlagsScore flagsScore;

    for(TiXmlElement* groupScoreEle = rootEle->FirstChildElement(); groupScoreEle; groupScoreEle = groupScoreEle->NextSiblingElement())
    {
        if(Crown::SDStrcmp(groupScoreEle->Value(), "time") == 0)
        {
            groupScoreEle->QueryIntAttribute("second", &m_time);
            m_time *= 1000;

            for(TiXmlElement* flagsEle = groupScoreEle->FirstChildElement(); flagsEle; flagsEle = flagsEle->NextSiblingElement())
            {
                if(Crown::SDStrcmp(flagsEle->Value(), "flags") == 0)
                {
                    flagsScore.Clear();

                    flagsEle->QueryIntAttribute("num", &flagsScore.num);
                    flagsEle->QueryIntAttribute("score", &flagsScore.score);

                    if(m_flagsScoreList.find(flagsScore.num) != m_flagsScoreList.end())
                    {
                        CnError("flags num:%d is exist!\n", flagsScore.num);
                        return false;
                    }

                    m_flagsScoreList[flagsScore.num] = flagsScore;
                }
            }
        }
    }

    return true;
}

bool CaptureFlagConfig::LoadScoreRewards(TiXmlElement* rootEle)
{
    REWARD_TABLE rewardTable;
    FlagRankRewardList rankRewardList;
    for(TiXmlElement* scoreRewardEle = rootEle->FirstChildElement(); scoreRewardEle; scoreRewardEle = scoreRewardEle->NextSiblingElement())
    {
        if(Crown::SDStrcmp(scoreRewardEle->Value(), "player_score") == 0)
        {
            int32 score = 0;
            scoreRewardEle->QueryIntAttribute("score", &score);
            rankRewardList.clear();
            for(TiXmlElement* playerScoreEle = scoreRewardEle->FirstChildElement(); playerScoreEle; playerScoreEle = playerScoreEle->NextSiblingElement())
            {
                if(Crown::SDStrcmp(playerScoreEle->Value(), "rank_rewards") == 0)
                {
                    int32 rank = 0;
                    playerScoreEle->QueryIntAttribute("rank", &rank);
                    rewardTable.Clear();
                    if(!LoadRewardTable(playerScoreEle, rewardTable))
                        return false;

                    if(rankRewardList.find(rank) != rankRewardList.end())
                    {
                        CnError("rank:%d is exit!\n", rank);
                        return false;
                    }

                    rankRewardList[rank] = rewardTable;
                }
            }

            if(m_RewardList.find(score) != m_RewardList.end())
            {
                CnError("score:%d is exit!\n", score);
                return false;
            }

            m_RewardList[score] = rankRewardList;
        }
    }
    return true;
}

bool CaptureFlagConfig::LoadGroupRewards(TiXmlElement* rootEle)
{
    REWARD_TABLE rewardTable;
    for(TiXmlElement* groupRewardEle = rootEle->FirstChildElement(); groupRewardEle; groupRewardEle = groupRewardEle->NextSiblingElement())
    {
        rewardTable.Clear();
        if(Crown::SDStrcmp(groupRewardEle->Value(), "success") == 0)
        {
            if(!LoadRewardTable(groupRewardEle, rewardTable))
            {
                CnError("success reward is err\n");
                return false;
            }

            m_successRewardList = rewardTable;
        }
        else if(Crown::SDStrcmp(groupRewardEle->Value(), "false") == 0)
        {
            if(!LoadRewardTable(groupRewardEle, rewardTable))
            {
                CnError("faild reward is err\n");
                return false;
            }

            m_falseRewardList = rewardTable;
        }
    }

    return true;
}

int32 CaptureFlagConfig::GetAddGroupScore(int32 num)
{
    FlagsScoreList::iterator it = m_flagsScoreList.find(num);
    if(it == m_flagsScoreList.end())
        return 0;

    return it->second.score;
}

const REWARD_TABLE* CaptureFlagConfig::GetRankRewardList(int32 score, int32 rank)
{
    // 倒序查找对应积分区间的奖励列表
    FlagPlayerRewardList::reverse_iterator rewardIt = m_RewardList.rbegin();
    for(; rewardIt != m_RewardList.rend(); ++rewardIt)
    {
        if(score >= rewardIt->first)
            break;
    }

    if(rewardIt == m_RewardList.rend())
        return NULL;

    FlagRankRewardList& flagPlayerReard = rewardIt->second;
    // 倒序查找对应排名区间的奖励列表
    FlagRankRewardList::reverse_iterator rankIt = flagPlayerReard.rbegin();
    for(; rankIt != flagPlayerReard.rend(); ++rankIt)
    {
        if(rank >= rankIt->first)
            break;
    }

    if(rankIt == flagPlayerReard.rend())
        return NULL;

    return &(rankIt->second);
}
