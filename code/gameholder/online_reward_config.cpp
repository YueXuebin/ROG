#include "gameholder_pch.h"
#include "online_reward_config.h"
#include "item_config_manager.h"
#include "item_base.h"
#include "player.h"
#include "game_util.h"
#include "item_manager.h"
#include "equip_manager.h"
#include "ride_manager.h"

IMPLEMENT_SINGLETON(OnlineRewardConfig)

OnlineRewardConfig::OnlineRewardConfig()
{
    Clear();
}

OnlineRewardConfig::~OnlineRewardConfig()
{
    
}

void OnlineRewardConfig::Clear()
{
    m_max_id = 0;
    m_online_reward_map.clear();
    m_singleProbablityMax = 0;
    m_gifts_Intervalday = 0;

    m_addGrowPrecent = 0;
    m_critNum = 0;
    m_online_singleprobablity = 0;
    m_online_cirtproblity = 0;

    m_maxnextlevel = 0;
}

bool OnlineRewardConfig::LoadConfig(const char* path)
{
    TiXmlDocument   xmlDoc;
    OnlineReward    onlineReward;
    REWARD_ITEM     rewardItem;

    if (!xmlDoc.LoadFile(path))
    {
        CnError("Load item file: %s failed\n", path);
        return false;
    }

    TiXmlElement* xmlRoot = xmlDoc.RootElement();
    if(!xmlRoot) return false;

    //
   // LoadRewardTable
    // 读取商品信息
    for(TiXmlElement* xmlEle = xmlRoot->FirstChildElement(); xmlEle; xmlEle = xmlEle->NextSiblingElement())
    {
        if(strcmp(xmlEle->Value(), "online_reward") == 0)
        {
            for(TiXmlElement* rewardEle = xmlEle->FirstChildElement(); rewardEle; rewardEle = rewardEle->NextSiblingElement())
            {
                onlineReward.Clear();
                onlineReward.id = Crown::SDAtou(rewardEle->Attribute("id"));
                onlineReward.time = Crown::SDAtou(rewardEle->Attribute("time"));

                onlineReward.rewardTable.reason = REWARD_REASON_ONLINE_REWARD;

                for(TiXmlElement* itEle = rewardEle->FirstChildElement(); itEle; itEle = itEle->NextSiblingElement())
                {
                    rewardItem.Clear();

                    if(!LoadRewardItem(itEle, rewardItem))
                    {
                        continue;
                    }

                    onlineReward.rewardTable.items.push_back(rewardItem);
                }

                if(m_online_reward_map.find(onlineReward.id) == m_online_reward_map.end())
                {
                    if(onlineReward.id > m_max_id)
                    {
                        m_max_id = onlineReward.id;
                    }

                    m_online_reward_map[onlineReward.id] = onlineReward;
                }
                else
                {
                    CnWarn("Duplicate online reward:%u\n", onlineReward.id);
                }
            }
        }
        else if(strcmp(xmlEle->Value(), "fixed_reward") == 0)
        {
            ReadFixedReward(xmlEle);
        }
        else if(strcmp(xmlEle->Value(), "grow_reward") == 0)
        {
            ReadGrowReward(xmlEle);
        }
        else if(strcmp(xmlEle->Value(), "single_probability") == 0)
        {
            ReadSingleProbability(xmlEle);
        }
        else if(strcmp(xmlEle->Value(), "level_reward") == 0)
        {
            ReadLevelReward(xmlEle);
        }
        else if(strcmp(xmlEle->Value(), "consts") == 0)
        {
            ReadConst(xmlEle);
        }
    }

    return true;
}

const OnlineReward* OnlineRewardConfig::GetOnlineReward(uint32 id)
{
    std::map<uint32, OnlineReward>::iterator rewardIt;
    rewardIt = m_online_reward_map.find(id);

    if(rewardIt != m_online_reward_map.end())
    {
        return &(rewardIt->second);
    }

    return nullptr;
}

void OnlineRewardConfig::FindFixedReward(REWARD_TABLE& rewardtable, int32 index)
{
    std::map<int32, REWARD_TABLE>::iterator rewardtabeliter;
    rewardtabeliter = m_fixed_reward_map.find(index);
    if(rewardtabeliter != m_fixed_reward_map.end())
        rewardtable = rewardtabeliter->second;
}
void OnlineRewardConfig::FindGrowReward(REWARD_TABLE& rewardtable, int32 index)
{
    std::map<int32, REWARD_TABLE>::iterator rewardtabeliter;
    rewardtabeliter = m_grow_reward_map.find(index);
    if(rewardtabeliter != m_grow_reward_map.end())
        rewardtable = rewardtabeliter->second;
}
void OnlineRewardConfig::FindGrowRewardByOrder(int32 order, int32 index, REWARD_ITEM& curitem)
{
    REWARD_TABLE rewardtable;
    FindGrowReward(rewardtable, index);
    std::vector<REWARD_ITEM>::iterator rewarditer;
    for (rewarditer = rewardtable.items.begin(); rewarditer != rewardtable.items.end(); rewarditer++)
    {
        int32 curorder = rewarditer->flag;
        if(curorder == order)
        {
            curitem = (*rewarditer);
            break;
        }
    }
}
int32 OnlineRewardConfig::FindAppointNextIndex(int32 type, int32 index)
{
    if(type == ONLINE_GROW_REWARD)
    {
        return  FindNextIndex(index, m_grow_reward_map);
    }
    else if (type == ONLINE_FIXED_REWARD)
    {
       return FindNextIndex(index, m_fixed_reward_map);
    }
    CnAssert(false);
    return 1;
}
int32 OnlineRewardConfig::FindNextIndex(int32 index, std::map<int32, REWARD_TABLE>& rewardMap)
{
    std::map<int32, REWARD_TABLE>::iterator indexreward;
    bool b_find = false;

    int32 num = 1;
    for (indexreward = rewardMap.begin(); indexreward != rewardMap.end(); indexreward++)
    {
        if(index == indexreward->first)
        {
            if(num != rewardMap.size())
            {
               b_find = true;
               return num+ 1;
            }
        }
        num = num +1;
    }

    if(!b_find)
    {
        if(num == rewardMap.size())
        {
            indexreward = rewardMap.begin();
            return indexreward->first;
        }
        //CnAssert(false);
    }

    return 1;
}
bool OnlineRewardConfig::ReadFixedReward(TiXmlElement* xmlEle)
{
    if(!xmlEle)
        return false;

    for(TiXmlElement* rewardEle = xmlEle->FirstChildElement(); rewardEle; rewardEle = rewardEle->NextSiblingElement())
    {
         int32 index = 0;
         rewardEle->QueryIntAttribute("num", &index);

        std::string rewardstr = rewardEle->Attribute("rewards");
        REWARD_TABLE rewardTable;
        if(!LoadRewardTable(rewardstr, rewardTable))
        {
            CnWarn("Unknow read Fixedreward reward:\n");
            return false;
        }

        if(m_fixed_reward_map.find(index) == m_fixed_reward_map.end() && index != 0)
        {
            m_fixed_reward_map[index] = rewardTable;
        }
        else
        {
            CnWarn("Duplicate Fixedreward: %u\n", index);
        }
    }

    return true;
}
bool OnlineRewardConfig::ReadLevelReward(TiXmlElement* xmlEle)
{
    if(!xmlEle)
        return false;

    for(TiXmlElement* rewardEle = xmlEle->FirstChildElement(); rewardEle; rewardEle = rewardEle->NextSiblingElement())
    {
        int32 level = 0;
        rewardEle->QueryIntAttribute("level", &level);

        std::string rewardstr = rewardEle->Attribute("rewards");
        REWARD_TABLE rewardtable;
        if(!LoadRewardTable(rewardstr, rewardtable))
        {
            CnWarn("Unknow read Fixedreward reward in readlevelreward:\n");
            return false;
        }

        if(m_level_reward_map.find(level) == m_level_reward_map.end() && level!= 0)
        {
            m_level_reward_map[level] = rewardtable;

        }
        else
        {
            CnWarn("Duplicate ReadLevelReward level: %u\n", level);
        }
    }

    //  level  map从小到大给值排序  
    return true;
}

void OnlineRewardConfig::ReadConst(TiXmlElement* xmlEle)
{
    if(!xmlEle)
        return;

    for(TiXmlElement* constEle = xmlEle->FirstChildElement(); constEle; constEle = constEle->NextSiblingElement())
    {
        if(strcmp(constEle->Value(), "growpercent") == 0)
        {
            m_addGrowPrecent = SDAtoi(constEle->Attribute("value"));
        }
        else if(strcmp(constEle->Value(), "critNum") == 0)
        {
            m_critNum = SDAtoi(constEle->Attribute("value"));
        }
        else if(strcmp(constEle->Value(), "singleprobablity") == 0)
        {
            m_online_singleprobablity = SDAtoi(constEle->Attribute("value"));
        }
        else if(strcmp(constEle->Value(), "cirtprobablity") == 0)
        {
            m_online_cirtproblity = SDAtoi(constEle->Attribute("value"));
        }
        else if (strcmp(constEle->Value(), "gitday") == 0)
        {
            m_gifts_Intervalday = SDAtoi(constEle->Attribute("value"));
        }
        else if (strcmp(constEle->Value(), "maxnextlevel") == 0)
        {
            m_maxnextlevel = SDAtoi(constEle->Attribute("value"));
        }
    }

    m_singleProbablityMax = BASE_RATE_NUM;
}
bool OnlineRewardConfig::ReadGrowReward(TiXmlElement* xmlEle)
{
    if(!xmlEle)
        return false;
    for(TiXmlElement* rewardEle = xmlEle->FirstChildElement(); rewardEle; rewardEle = rewardEle->NextSiblingElement())
    {
        int32 index = 0;
        rewardEle->QueryIntAttribute("num", &index);

        std::string rewardstr = rewardEle->Attribute("rewards");
        REWARD_TABLE rewardTable;
        if(!LoadRewardTable(rewardstr, rewardTable))
        {
            CnWarn("Unknow read Fixedreward reward in readGrowreward:\n");
            return false;
        }

        std::vector<REWARD_ITEM>::iterator rewarditer;
        int order = 1;
        for (rewarditer = rewardTable.items.begin(); rewarditer!= rewardTable.items.end(); rewarditer++)
        {
            rewarditer->flag = order;
            order = order + 1;
        }

        if(m_grow_reward_map.find(index) == m_grow_reward_map.end() && index != 0)
        {
            m_grow_reward_map[index] = rewardTable;
        }
        else
        {
            CnWarn("Duplicate ReadGrowReward: %u\n", index);
        }
    }
    return true;
}
bool OnlineRewardConfig::ReadSingleProbability(TiXmlElement* xmlEle)
{
    if(!xmlEle)
        return false;

    int32 growVauleCount = 0;
    for(TiXmlElement* rewardEle = xmlEle->FirstChildElement(); rewardEle; rewardEle = rewardEle->NextSiblingElement())
    {
        REWARD_ITEM rewarditem;
        int32 order = 0;
        rewardEle->QueryIntAttribute("id", &order);
        rewarditem.flag = order;

        int32 growvalue = 0;
        rewardEle->QueryIntAttribute("growvalue", &growvalue);
        growVauleCount = growVauleCount  + growvalue;
        rewarditem.num = growVauleCount;

        m_single_probability.push_back(rewarditem);
    }

    m_singleProbablityMax = growVauleCount;

    if(m_single_probability.size() <= 0)
    {
        CnAssert(false);
    }

    return true;
}

int32 OnlineRewardConfig::FindIdByMinxLevelReward()
{
   std::map<int32, REWARD_TABLE>::iterator levelmap;
   levelmap = m_level_reward_map.begin();
   return levelmap->first;

   CnAssert(false);
   return 0;
}

int32 OnlineRewardConfig::FindNextLevelReward(int32 level)
{
    std::map<int32, REWARD_TABLE>::iterator levelmap;
    for (levelmap = m_level_reward_map.begin(); levelmap != m_level_reward_map.end(); levelmap++)
    {
        if(level < levelmap->first)
        {
            return levelmap->first;
        }
    }

    return m_maxnextlevel;
}
//从小到大排序
REWARD_TABLE* OnlineRewardConfig::FindIdByLevelReward(int32 level)
{
    int32 curlevel = 0;
    std::map<int32, REWARD_TABLE>::iterator levelmap;
    for (levelmap = m_level_reward_map.begin(); levelmap != m_level_reward_map.end(); levelmap++)
    {
       if(level == levelmap->first)
        {
            return &(levelmap->second);
        }
    }
    return NULL;
}
//根据概率随机物品
REWARD_ITEM* OnlineRewardConfig::RandomSingleProbility(int32 probability)
{
    if(probability <= m_single_probability[0].num)
        return &m_single_probability[0];
    else if(m_single_probability[0].num < m_single_probability[1].num && probability <= m_single_probability[1].num)
        return &m_single_probability[1];
    else if(m_single_probability[1].num < m_single_probability[2].num && probability <= m_single_probability[2].num)
        return &m_single_probability[2];
    else if(m_single_probability[2].num < m_single_probability[3].num && probability <= m_single_probability[3].num)
        return &m_single_probability[3];
    else if(m_single_probability[3].num < m_single_probability[4].num && probability <= m_single_probability[4].num)
        return &m_single_probability[4];

    return NULL;
}