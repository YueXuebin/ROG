#include "gameholder_pch.h"
#include "day_investment_config.h"
#include "player.h"
#include "item_manager.h"
#include "equip_manager.h"
#include "ride_manager.h"
#include "game_util.h"

IMPLEMENT_SINGLETON(DayInvestmentConfig);

DayInvestmentConfig::DayInvestmentConfig()
{
    m_total_day = 0;
    m_investment_magic_stone = 0;
    m_day_investment_map.clear();
}

DayInvestmentConfig::~DayInvestmentConfig()
{

}

bool DayInvestmentConfig::LoadDayInvestmentConfig( const char* path )
{

    TiXmlDocument xmlDoc;

    if (!xmlDoc.LoadFile(path))
    {
        CnError("Load file: %s failed\n", path);
        return false;
    }

    TiXmlElement* xmlRoot = xmlDoc.RootElement();
    if(!xmlRoot) 
        return false;

    for(TiXmlElement* root_element = xmlRoot->FirstChildElement(); root_element; root_element = root_element->NextSiblingElement())
    {
        if(Crown::SDStrcmp(root_element->Value(), "investment") == 0)
        {
            // 当前设计只需要一个
            if(!LoadNode(root_element))
                return false;
        }
        else if(Crown::SDStrcmp(root_element->Value(), "investments") == 0)
        {
            if(!LoadInvestmentType(root_element))
                return false;
        }
    }

    return true;
}

bool DayInvestmentConfig::LoadInvestmentType(TiXmlElement* rootEle)
{
    InvestmentPlanType investmentType;
    InvestmentPlanDayInfo dayInfo;
    REWARD_TABLE rewardTable;

    investmentType.Clear();
    rootEle->QueryUnsignedAttribute("id", &investmentType.id);
    rootEle->QueryUnsignedAttribute("total_day", &investmentType.total_day);
    rootEle->QueryUnsignedAttribute("additional_overdue_day", &investmentType.additional_overdue_day);
    rootEle->QueryUnsignedAttribute("viplevel", &investmentType.vipPrivilege);

    for(TiXmlElement* investmentsEle = rootEle->FirstChildElement(); investmentsEle; investmentsEle = investmentsEle->NextSiblingElement())
    {
        if(Crown::SDStrcmp(investmentsEle->Value(), "day") == 0)
        {
            dayInfo.Clear();
            investmentsEle->QueryUnsignedAttribute("day", &dayInfo.day);

            for(TiXmlElement* dayEle = investmentsEle->FirstChildElement(); dayEle; dayEle = dayEle->NextSiblingElement())
            {
                if ((Crown::SDStrcmp(dayEle->Value(), "rewards") == 0))
                {
                    rewardTable.Clear();
                    uint32 moneypoint = 0;

                    dayEle->QueryUnsignedAttribute("moneypoint", &moneypoint);
                    if(!LoadRewardTable(dayEle, rewardTable))
                        return false;

                    dayInfo.moneypointRewardMap[moneypoint] = rewardTable;
                }
            }

            investmentType.dayMap[dayInfo.day] = dayInfo;
        }
    }

    m_investmentPlanMap[investmentType.id] = investmentType;

    return true;
}

bool DayInvestmentConfig::LoadNode(TiXmlElement* root_element)
{
    if(Crown::SDStrcmp(root_element->Value(), "investment") == 0)
    {
        root_element->QueryUnsignedAttribute("total_day", &m_total_day);
        root_element->QueryUnsignedAttribute("investment_magic_stone", &m_investment_magic_stone);

        for(TiXmlElement* day_element = root_element->FirstChildElement(); day_element; day_element = day_element->NextSiblingElement())
        {
            if ((Crown::SDStrcmp(day_element->Value(), "day") == 0))
            {
                DayInvestmentInfo day_info;
                day_info.Clear();

                uint32 day = 0;
                day_element->QueryUnsignedAttribute("day", &day);
                day_info.day = day;

                for(TiXmlElement* rewards_element= day_element->FirstChildElement(); rewards_element; rewards_element = rewards_element->NextSiblingElement())
                {
                    if ((Crown::SDStrcmp(rewards_element->Value(), "rewards") == 0))
                    {
                        for(TiXmlElement* item_element= rewards_element->FirstChildElement(); item_element; item_element = item_element->NextSiblingElement())
                        {
                            if ((Crown::SDStrcmp(rewards_element->Value(), "rewards") == 0))
                            {
                                REWARD_ITEM rewarditem;
                                rewarditem.Clear();

                                item_element->QueryIntAttribute("num", &rewarditem.num);
                                //item_element->QueryIntAttribute("type", &rewarditem.type);
                                rewarditem.id = 0;

                                // 当前设计只需要一个
                                day_info.reward = rewarditem;
                            }
                        }
                    }
                }

                m_day_investment_map.insert(std::make_pair(day, day_info));
            }
        }
    }

    return true;
}

DayInvestmentInfo* DayInvestmentConfig::GetDayInvestmentInfo( uint32 day )
{
    DayInvestmentInfoMap::iterator info_iter;
    info_iter = m_day_investment_map.find(day);
    if (info_iter != m_day_investment_map.end())
        return &info_iter->second;

    return nullptr;
}

uint32 DayInvestmentConfig::GetInvestmentTotalDay()
{
    return m_total_day;
}

uint32 DayInvestmentConfig::GetInvestmentMagicStoneCount()
{
    return m_investment_magic_stone;
}


uint32 DayInvestmentConfig::GiveDayInvestmentReward(Player* player, uint32 day)
{
    if(!player) 
        return 0;

    DayInvestmentInfo *info = GetDayInvestmentInfo(day);
    if (!info)
        return 0;

    int32 num = 0;

    // 目前只有魔石奖励,如有需求再改成多个
    //switch(info->reward.type)
    //{
    //case PROP_MONEYPOINT:
    //    {
    //        num = player->GetFcmGainValue(info->reward.num);
    //        player->AddMoneyPoint(num, REWARD_REASON_DAY_INVESTMENT);
    //    }
    //    break;
    //default:
    //    {
    //        return 0;
    //    }
    //    break;
    //}

    return num;
}

const InvestmentPlanType* DayInvestmentConfig::GetInvestmentPlanTmp(uint32 id)
{
    std::map<int32, InvestmentPlanType>::iterator planIt = m_investmentPlanMap.find(id);
    if(planIt == m_investmentPlanMap.end())
        return NULL;

    return &planIt->second;
}
