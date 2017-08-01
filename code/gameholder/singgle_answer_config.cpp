#include "gameholder_pch.h"
#include "singgle_answer_config.h"
#include "game_util.h"
IMPLEMENT_SINGLETON(SingleAnswerConfig)
SingleAnswerConfig::SingleAnswerConfig(void)
{
    m_once_time = 0;
    m_question_num = 0;
    m_reward_item = 0;
    m_cdMoney     = 0;
}

SingleAnswerConfig::~SingleAnswerConfig(void)
{
}
bool SingleAnswerConfig::LoadConfig(const char* path)
{
    TiXmlDocument xmlDoc;

    if (!xmlDoc.LoadFile(path))
    {
        CnError("Load single answer file: %s failed\n", path);
        return false;
    }

    TiXmlElement* xmlRoot = xmlDoc.RootElement();
    if(!xmlRoot) return false;

    for(TiXmlElement* singleEle = xmlRoot->FirstChildElement(); singleEle; singleEle = singleEle->NextSiblingElement())
    {
        if (Crown::SDStrcmp(singleEle->Value(), "rewards") == 0)
        {
            ReadRewards(singleEle);
        }
        else if (Crown::SDStrcmp(singleEle->Value(), "results") == 0)
        {
            ReadResults(singleEle);
        }
        else if (Crown::SDStrcmp(singleEle->Value(), "times") == 0)
        {
            ReadTimes(singleEle);
        }
        else if (Crown::SDStrcmp(singleEle->Value(), "once_time") == 0)
        {
            singleEle->QueryUnsignedAttribute("value", &m_once_time);
        }
        else if (Crown::SDStrcmp(singleEle->Value(), "question_num") == 0)
        {
            singleEle->QueryIntAttribute("value", &m_question_num);
        }
        else if (Crown::SDStrcmp(singleEle->Value(), "item") == 0)
        {
            singleEle->QueryUnsignedAttribute("value", &m_reward_item);
        }
        else if (Crown::SDStrcmp(singleEle->Value(), "cdmoney") == 0)
        {
            singleEle->QueryIntAttribute("value", &m_cdMoney);
        }
    }

    return true;
}

void SingleAnswerConfig::ReadRewards(TiXmlElement* singleEle)
{
    if(!singleEle)
        return;

    REWARD_TABLE rewardTable;
    for(TiXmlElement* rewardEle = singleEle->FirstChildElement(); rewardEle; rewardEle = rewardEle->NextSiblingElement())
    {
        int32 level = 0;
        //int32 num = 0;
        rewardEle->QueryIntAttribute("level", &level);
        //rewardEle->QueryIntAttribute("num", &num);

         rewardTable.Clear();
         //ReadAnswerReward(rewardEle, rewardTable);
         rewardTable.reason = REWARD_REASON_SINGLE_ANSWER_FINISH;
         std::string rewardstr = rewardEle->Attribute("num");
         if(!LoadRewardTable(rewardstr, rewardTable))
         {
             CnWarn("Unknow read singleanswer reward in ReadRewards:\n");
             return ;
         }

        if(m_levelRewardList.find(level) == m_levelRewardList.end())
            m_levelRewardList[level] = rewardTable;
        else
            CnWarn("Duplicate single answer level: %d", level);

        //if(rewardTable.items.size() < =0)
            //Cdebug(false);
    }
}

void SingleAnswerConfig::ReadAnswerReward(TiXmlElement* rewardEle, REWARD_TABLE& rewardTable)
{
    if(!rewardEle) return;

    rewardTable.reason = REWARD_REASON_SINGLE_ANSWER_FINISH;
    for(TiXmlElement* itEle = rewardEle->FirstChildElement(); itEle; itEle = itEle->NextSiblingElement())
    {
        REWARD_ITEM rewardItem;
        if(!LoadRewardItem(itEle, rewardItem))
        {
            continue;
        }
        rewardTable.items.push_back(rewardItem);
    }
}
void SingleAnswerConfig::ReadResults(TiXmlElement* singleEle)
{
    if(!singleEle)
        return;
    for(TiXmlElement* result = singleEle->FirstChildElement(); result; result = result->NextSiblingElement())
    {
        uint32 num = 0;
        int32 percentage = 0;
        result->QueryUnsignedAttribute("num", &num);
        result->QueryIntAttribute("percentage", &percentage);

        if(m_rightPecentage.find(num) == m_rightPecentage.end())
            m_rightPecentage[num] = percentage;
        else
            CnWarn("Duplicate  single answer num: %d", num);
    }
}

void SingleAnswerConfig::ReadTimes(TiXmlElement* singleEle)
{
    if(!singleEle)
        return;
    for(TiXmlElement* timeEle = singleEle->FirstChildElement(); timeEle; timeEle = timeEle->NextSiblingElement())
    {
        uint32 time = 0;
        int32 num = 0;
        timeEle->QueryUnsignedAttribute("time", &time);
        timeEle->QueryIntAttribute("num", &num);

        if(m_numtoCD.find(num) == m_numtoCD.end())
            m_numtoCD[num] = time;
        else
            CnWarn("Duplicate  single answer num: %d", num);
    }
}
int32 SingleAnswerConfig::CalRewardNum(int32 playerlevel, uint32 rightnum)
{
    //int32 rewardnum = 0;
    //std::map<int32, int32>::iterator leveliter;
    //leveliter = m_levelRewardList.find(playerlevel);
    //if(leveliter != m_levelRewardList.end())
    //    rewardnum = leveliter->second;

    //int32 percent = 0;
    //std::map<uint32, int32>::iterator rightiter;
    //rightiter = m_rightPecentage.find(rightnum);
    //if(rightiter != m_rightPecentage.end())
    //    percent = rightiter->second;

    //return rewardnum * percent / BASE_RATE_NUM;

    return 0;
}
//根据答对题的个数计算得到的奖励
void SingleAnswerConfig::RewardTableByRightNum(int32 playerlevel, uint32 rightnum ,REWARD_TABLE& rewardtable)
{
 /*   int32 rewardnum = 0;
    std::map<int32, int32>::iterator leveliter;
    leveliter = m_levelRewardList.find(playerlevel);
    if(leveliter != m_levelRewardList.end())
        rewardnum = leveliter->second;

    int32 percent = 0;
    std::map<uint32, int32>::iterator rightiter;
    rightiter = m_rightPecentage.find(rightnum);
    if(rightiter != m_rightPecentage.end())
        percent = rightiter->second;*/

    //return rewardnum * percent / BASE_RATE_NUM;


   // int32 rewardnum = 0;
    std::map<int32, REWARD_TABLE>::iterator leveliter;
    leveliter = m_levelRewardList.find(playerlevel);
    if(leveliter != m_levelRewardList.end())
        rewardtable = leveliter->second;

    int32 percent = 0;
    std::map<uint32, int32>::iterator rightiter;
    rightiter = m_rightPecentage.find(rightnum);
    if(rightiter != m_rightPecentage.end())
        percent = rightiter->second;

    std::vector<REWARD_ITEM>::iterator rewarditer;
    for (rewarditer = rewardtable.items.begin();   rewarditer != rewardtable.items.end();rewarditer++)
    {
        rewarditer->num =  rewarditer->num* percent / BASE_RATE_NUM;
    }

}
uint32 SingleAnswerConfig::GetCDTimeByChangeNum(int32 changenum)
{
    std::map<int32, uint32>::iterator timeiter;
    timeiter = m_numtoCD.find(changenum);

    if(timeiter != m_numtoCD.end())
       return timeiter->second;
    else
    {
       int32 num = m_numtoCD.size();
       return m_numtoCD[num];
    }
}