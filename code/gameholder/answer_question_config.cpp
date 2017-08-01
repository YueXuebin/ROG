#include "gameholder_pch.h"
#include "answer_question_config.h"
#include "game_util.h"

IMPLEMENT_SINGLETON(AnswerQuestionConfig)

AnswerQuestionConfig::AnswerQuestionConfig()
{
    m_questionTemple.clear();
    m_timeScore.clear();
    m_continuation.clear();
    m_buy_times = 0;
    m_double_times = 0;
    m_buy_times_money_point = 0;
    m_gen_question_num = 0;
    m_sys_double_times = 0;
}

AnswerQuestionConfig::~AnswerQuestionConfig()
{

}

bool AnswerQuestionConfig::LoadConfig( const char* path )
{
    TiXmlDocument xmlDoc;

    if (!xmlDoc.LoadFile(path))
    {
        CnError("Load babel file: %s failed\n", path);
        return false;
    }

    TiXmlElement* xmlRoot = xmlDoc.RootElement();
    if(!xmlRoot) return false;

    for (TiXmlElement* answerEle = xmlRoot->FirstChildElement(); answerEle; answerEle = answerEle->NextSiblingElement())
    {
        if (Crown::SDStrcmp(answerEle->Value(), "questions") == 0)
        {
            LoadQuestion(answerEle);
        }
        else if (Crown::SDStrcmp(answerEle->Value(), "scores") == 0)
        {
            LoadTimeScore(answerEle);
        }
        else if (Crown::SDStrcmp(answerEle->Value(), "continuations") == 0)
        {
            LoadContinuation(answerEle);
        }
        else if (Crown::SDStrcmp(answerEle->Value(), "rewards") == 0)
        {
            LoadReward(answerEle);
        }
        else if (Crown::SDStrcmp(answerEle->Value(), "buy_times") == 0)
        {
            answerEle->QueryIntAttribute("value", &m_buy_times);
        }
        else if (Crown::SDStrcmp(answerEle->Value(), "double_times") == 0)
        {
            answerEle->QueryIntAttribute("value", &m_double_times);
        }
        else if (Crown::SDStrcmp(answerEle->Value(), "buy_times_money_point") == 0)
        {
            answerEle->QueryIntAttribute("value", &m_buy_times_money_point);
        }
        else if(Crown::SDStrcmp(answerEle->Value(), "gen_question_num") == 0)
        {
            m_gen_question_num = Crown::SDAtou(answerEle->Attribute("value"));
        }
        else if(Crown::SDStrcmp(answerEle->Value(), "sys_double_times") == 0)
        {
            answerEle->QueryIntAttribute("value", &m_sys_double_times);
        }
    }
    return true;
}

bool AnswerQuestionConfig::LoadQuestion( TiXmlElement* answerEle )
{
    uint32 questionID = 0;
    uint32 result = 0;
    for (TiXmlElement* xmlEle = answerEle->FirstChildElement(); xmlEle; xmlEle = xmlEle->NextSiblingElement())
    {
        if (Crown::SDStrcmp(xmlEle->Value(), "question") == 0)
        {
            xmlEle->QueryUnsignedAttribute("id", &questionID);
            xmlEle->QueryUnsignedAttribute("result", &result);
            m_questionTemple[questionID] = result;
        }
    }
    return true;
}

bool AnswerQuestionConfig::LoadTimeScore( TiXmlElement* answerEle )
{
    uint32 time = 0;
    uint32 score = 0;
    for (TiXmlElement* xmlEle = answerEle->FirstChildElement(); xmlEle; xmlEle = xmlEle->NextSiblingElement())
    {
        if (Crown::SDStrcmp(xmlEle->Value(), "score") == 0)
        {
            xmlEle->QueryUnsignedAttribute("time", &time);
            xmlEle->QueryUnsignedAttribute("value", &score);
            m_timeScore[time] = score;
        }
    }
    return true;
}

bool AnswerQuestionConfig::LoadContinuation( TiXmlElement* answerEle )
{
    uint32 num = 0;
    uint32 score = 0;
    for (TiXmlElement* xmlEle = answerEle->FirstChildElement(); xmlEle; xmlEle = xmlEle->NextSiblingElement())
    {
        if (Crown::SDStrcmp(xmlEle->Value(), "continuation") == 0)
        {
            xmlEle->QueryUnsignedAttribute("num", &num);
            xmlEle->QueryUnsignedAttribute("score", &score);
            m_continuation[num] = score;
        }
    }
    return true;
}

bool AnswerQuestionConfig::LoadReward( TiXmlElement* answerEle )
{
    RankReward rankReward;
    REWARD_ITEM rewardItem;

    for (TiXmlElement* xmlEle = answerEle->FirstChildElement(); xmlEle; xmlEle = xmlEle->NextSiblingElement())
    {
        rankReward.Clear();

        std::string range = xmlEle->Attribute("range");
        std::vector<std::string> min_max = Crown::SDSplitStrings(range, '-');

        CnAssert(min_max.size() == 2);

        rankReward.m_minRank = atoi(min_max[0].c_str());
        rankReward.m_maxRank = atoi(min_max[1].c_str());

        for (TiXmlElement* rewardEle = xmlEle->FirstChildElement(); rewardEle; rewardEle = rewardEle->NextSiblingElement())
        {
            rewardItem.Clear();

            if (!LoadRewardItem(rewardEle, rewardItem))
            {
                CnWarn("Error answer: %u - %u reward error", rankReward.m_minRank, rankReward.m_maxRank);
            }
            else
            {
                rankReward.m_rewards.items.push_back(rewardItem);
            }
        }

        m_rankReward.push_back(rankReward);
    }
    return true;
}

uint8 AnswerQuestionConfig::GetRightResult( uint32 questionID )
{
    std::map<uint32, uint8>::iterator questionIt = m_questionTemple.find(questionID);
    if(questionIt != m_questionTemple.end())
    {
        return questionIt->second;
    }
    return 0;
}

uint8 AnswerQuestionConfig::GetTimeScore( uint8 time )
{
    std::map<uint8, uint8>::iterator it = m_timeScore.find(time);
    if(it != m_timeScore.end())
    {
        return it->second;
    }
    return 0;
}

uint8 AnswerQuestionConfig::GetContinationScore( uint8 num )
{
    std::map<uint8, uint8>::iterator it = m_continuation.find(num);
    if(it != m_continuation.end())
    {
        return it->second;
    }
    return 0;
}

const REWARD_TABLE* AnswerQuestionConfig::GetRankReward(uint32 rank)
{
    for(std::vector<RankReward>::iterator it = m_rankReward.begin(); it != m_rankReward.end(); ++it)
    {
        if((it->m_minRank <= rank) && (rank <= it->m_maxRank))
        {
            return &(it->m_rewards);
        }
    }
    return 0;
}

void AnswerQuestionConfig::GenQuestion(uint32 num, std::vector<uint32>& list)
{
    std::vector<uint32> questions;
    std::vector<uint32>::iterator pickIt;
    int32 rand_num = 0;

    for(std::map<uint32, uint8>::iterator it = m_questionTemple.begin();
        it != m_questionTemple.end(); ++it)
    {
        questions.push_back(it->first);
    }

    for(uint32 i=0; (i < num) && (questions.size() > 0); ++i)
    {
        rand_num = Crown::SDRandom(questions.size());

        pickIt=questions.begin();
        for(int j=0; (j<rand_num) && (pickIt!=questions.end()); ++j, ++pickIt);

        list.push_back(*pickIt);

        questions.erase(pickIt);
    }
}

//const REWARD_TABLE* AnswerQuestionConfig::GetTopRankReward(int32 rank)
//{
//    std::map<int32, REWARD_TABLE>::iterator it;
//    it = m_toprank_reward.find(rank);
//    if(it != m_toprank_reward.end())
//    {
//        return &(it->second);
//    }
//    return 0;
//}
