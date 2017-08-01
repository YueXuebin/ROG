#include "gameholder_pch.h"
#include "guild_task_config.h"
#include "game_util.h"

IMPLEMENT_SINGLETON(GuildTaskConfig)

GuildTaskConfig::GuildTaskConfig()
{
    m_max_task_num = 0;
    m_use_token_num = 0;
    m_token_id = 0;
    m_can_task_num = 0;
    //m_qualityProList.clear();
    m_boxRewardList.clear();
}

GuildTaskConfig::~GuildTaskConfig()
{

}

bool GuildTaskConfig::LoadConfig(const char* path)
{
    TiXmlDocument xmlDoc;

    if(!xmlDoc.LoadFile(path))
    {
        CnError("Load guild task file: %s failed\n", path);
        return false;
    }

    TiXmlElement* xmlRoot = xmlDoc.RootElement();
    if(!xmlRoot) 
        return false;

    for(TiXmlElement* guildTaskEle = xmlRoot->FirstChildElement(); guildTaskEle; guildTaskEle = guildTaskEle->NextSiblingElement())
    {
        if(Crown::SDStrcmp(guildTaskEle->Value(), "base_info") == 0)
        {
            guildTaskEle->QueryUnsignedAttribute("max_task_num", &m_max_task_num);
            guildTaskEle->QueryUnsignedAttribute("can_task_num", &m_can_task_num);
            guildTaskEle->QueryUnsignedAttribute("use_token_num", &m_use_token_num);
            guildTaskEle->QueryUnsignedAttribute("token_id", &m_token_id);
        }
        //else if(Crown::SDStrcmp(guildTaskEle->Value(), "qualitypro") == 0)
        //{
            //ItemProbability qualitypro;
            //for (TiXmlElement* dailyEle = guildTaskEle->FirstChildElement(); dailyEle; dailyEle = dailyEle->NextSiblingElement())
            //{
            //    qualitypro.clear();
            //    dailyEle->QueryUnsignedAttribute("probability",&qualitypro.m_probability);
            //    dailyEle->QueryIntAttribute("item",&qualitypro.m_item);
            //    dailyEle->QueryUnsignedAttribute("multiple",&qualitypro.m_multiple);
            //    m_qualityProList[qualitypro.m_item] = qualitypro;
            //}

            //// 检验总几率是否大于100%
            //uint32 totalPro = 0;
            //for (ItemProList::iterator proIt = m_qualityProList.begin(); proIt != m_qualityProList.end(); proIt++)
            //{
            //    totalPro += proIt->second.m_probability;
            //}
            //if (totalPro < BASE_RATE_NUM)
            //{
            //    CnFatal("probability total less then 100%\n");
            //}
        //}
        else if(Crown::SDStrcmp(guildTaskEle->Value(), "reward_addtion") == 0)
        {
            RewardAddtionItem addtionItem;
            for(TiXmlElement* addtionEle = guildTaskEle->FirstChildElement(); addtionEle; addtionEle = addtionEle->NextSiblingElement())
            {
                addtionItem.Clear();
                addtionEle->QueryIntAttribute("guild_level", &addtionItem.guild_level);
                addtionEle->QueryIntAttribute("guild_point", &addtionItem.guild_point);
                addtionEle->QueryIntAttribute("score", &addtionItem.score);
                addtionEle->QueryIntAttribute("gamepoint", &addtionItem.gamepoint);
                addtionEle->QueryIntAttribute("exp", &addtionItem.exp);
                m_rewardAddtionList[addtionItem.guild_level] = addtionItem;
            }
        }
        else if(Crown::SDStrcmp(guildTaskEle->Value(), "score_boxs") == 0)
        {
            int32 score = 0;
            REWARD_TABLE boxRewardTable;
            for(TiXmlElement* boxEle = guildTaskEle->FirstChildElement(); boxEle; boxEle = boxEle->NextSiblingElement())
            {
                if(Crown::SDStrcmp(boxEle->Value(), "box") == 0)
                {
                    boxRewardTable.Clear();
                    score = 0;
                    boxEle->QueryIntAttribute("score", &score);

                    if(!LoadRewardTable(boxEle, boxRewardTable))
                        return false;

                    m_boxRewardList[score] = boxRewardTable;
                }
            }
        }
    }
    return true;
}

REWARD_TABLE GuildTaskConfig::GetRewardAddtion(int32 guildLevel)
{
    REWARD_TABLE rewardTable;

    std::map<int32, RewardAddtionItem>::iterator it = m_rewardAddtionList.find(guildLevel);
    if(it == m_rewardAddtionList.end())
    {
        CnAssert(false);            // 没有对应公会等级的奖励
        return rewardTable;
    }

    REWARD_ITEM rewardItem;

//     rewardItem.Clear();
//     rewardItem.type = PROP_GUILDPOINT;
//     rewardItem.num = it->second.guild_point;
//     rewardTable.items.push_back(rewardItem);

    rewardItem.Clear();
    rewardItem.id = PROP_GUILD_SCORE;
    rewardItem.num = it->second.score;
    rewardTable.items.push_back(rewardItem);

    rewardItem.Clear();
    rewardItem.id = PROP_GAMEPOINT;
    rewardItem.num = it->second.gamepoint;
    rewardTable.items.push_back(rewardItem);


    rewardItem.Clear();
    rewardItem.id = PROP_EXP;
    rewardItem.num = it->second.exp;
    rewardTable.items.push_back(rewardItem);

    return rewardTable;
}

const REWARD_TABLE* GuildTaskConfig::GetBoxReward(int score)
{
    std::map<int32, REWARD_TABLE>::iterator boxRewardIt = m_boxRewardList.find(score);
    if(boxRewardIt == m_boxRewardList.end())
    {
        return NULL;
    }
    return &boxRewardIt->second;
}


