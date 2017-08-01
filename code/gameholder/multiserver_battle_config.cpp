#include "gameholder_pch.h"
#include "multiserver_battle_config.h"
#include "item_config_manager.h"
#include "game_setting_manager.h"

IMPLEMENT_SINGLETON(MultiserverBattleConfig)

MultiserverBattleConfig::MultiserverBattleConfig(void)
{
}

MultiserverBattleConfig::~MultiserverBattleConfig(void)
{
}

// 分数比较函数
bool SortConfigMultiserverLeve(const MultiserverLevelItem& left, const MultiserverLevelItem& right) 
{
    return (left.lv < right.lv);
}

bool MultiserverBattleConfig::LoadConfig( const char* path )
{
    char file_path[SD_MAX_PATH];
    SDSprintf(file_path, "%s%s", path, MULTISERVER_BATTLE_FILE);
    if(!LoadBattleConfig(file_path))
    {
        CnFatal("load %s failed\n", file_path);
        return false;
    }

    return true;
}

bool MultiserverBattleConfig::LoadBattleConfig( const char* path )
{
    TiXmlDocument xmlDoc;
    if (!xmlDoc.LoadFile(path))
    {
        CnError("Load npc file: %s failed\n", path);
        return false;
    }

    TiXmlElement* xmlRoot = xmlDoc.RootElement();
    if(!xmlRoot) 
        return false;

    std::string  strvalue = "";
    for(TiXmlElement* tableEle = xmlRoot->FirstChildElement(); tableEle; tableEle = tableEle->NextSiblingElement())
    {
        if(strcmp(tableEle->Value(), "integral") == 0)
        {
            MultiserverLevelItem lv;
            for(TiXmlElement* fashionsEle = tableEle->FirstChildElement(); fashionsEle; fashionsEle = fashionsEle->NextSiblingElement())
            {
                lv.Clear();

                lv.lv = SDAtoi(fashionsEle->Attribute("lv"));
                lv.point = SDAtoi(fashionsEle->Attribute("point"));
                lv.exploit = SDAtoi(fashionsEle->Attribute("exploit"));
                lv.alliance = SDAtoi(fashionsEle->Attribute("alliance"));
                m_levelList.push_back(lv);
            }
        }
        if(strcmp(tableEle->Value(), "mode") == 0)
        {
            MultiserverBatttleItem item;
            for(TiXmlElement* synergismEle = tableEle->FirstChildElement(); synergismEle; synergismEle = synergismEle->NextSiblingElement())
            {
                item.Clear();

                item.type = SDAtoi(synergismEle->Attribute("type"));
                item.win_integral = SDAtoi(synergismEle->Attribute("win_integral"));
                item.win_exploit = SDAtoi(synergismEle->Attribute("win_exploit"));
                item.fail_integral = SDAtoi(synergismEle->Attribute("fail_integral"));
                item.fail_exploit = SDAtoi(synergismEle->Attribute("fail_exploit"));
                item.wait = SDAtoi(synergismEle->Attribute("wait"));
                item.need_level = SDAtoi(synergismEle->Attribute("need_level"));
                item.enter_num = SDAtoi(synergismEle->Attribute("enter_num"));

                std::string open_time = synergismEle->Attribute("open_time");
                LoadOpenTime(open_time, item);

                std::string range_integral = synergismEle->Attribute("range_integral");
                LoadRangeIntegral(range_integral, item, 1);
                range_integral = synergismEle->Attribute("fail_range_integral");
                LoadRangeIntegral(range_integral, item, 2);

                if(item.type == MULTISERVER_BATTLE_APPLY_ONE)
                {
                    GameSettingManager::Instance()->SetMultiserverEnterNum(item.enter_num);
                }
                
                m_battleList.push_back(item);
            }
        }

        uint32 itemid = 0;
        MultiserverSaleInfo saleInfo;
        if(strcmp(tableEle->Value(), "sale") == 0)
        {
            for(TiXmlElement* fashionsEle = tableEle->FirstChildElement(); fashionsEle; fashionsEle = fashionsEle->NextSiblingElement())
            {
                saleInfo.Clear();
                saleInfo.id = Crown::SDAtou(fashionsEle->Attribute("id"));
                saleInfo.level = Crown::SDAtou(fashionsEle->Attribute("level"));
                saleInfo.limit = Crown::SDAtou(fashionsEle->Attribute("limit"));
                saleInfo.product_id = Crown::SDAtou(fashionsEle->Attribute("product_id"));
                saleInfo.exploit = Crown::SDAtou(fashionsEle->Attribute("exploit"));
                m_saleList.push_back(saleInfo); 
            }
        }

        if(strcmp(tableEle->Value(), "alliancesale") == 0)
        {
            for(TiXmlElement* fashionsEle = tableEle->FirstChildElement(); fashionsEle; fashionsEle = fashionsEle->NextSiblingElement())
            {
                saleInfo.Clear();
                saleInfo.id = Crown::SDAtou(fashionsEle->Attribute("id"));
                saleInfo.level = Crown::SDAtou(fashionsEle->Attribute("level"));
                saleInfo.limit = Crown::SDAtou(fashionsEle->Attribute("limit"));
                saleInfo.product_id = Crown::SDAtou(fashionsEle->Attribute("product_id"));
                saleInfo.exploit = Crown::SDAtou(fashionsEle->Attribute("exploit"));
                m_allianceList.push_back(saleInfo);
            }
        }

        if(strcmp(tableEle->Value(), "seasonsale") == 0)
        {
            for(TiXmlElement* fashionsEle = tableEle->FirstChildElement(); fashionsEle; fashionsEle = fashionsEle->NextSiblingElement())
            {
                saleInfo.Clear();
                saleInfo.id = Crown::SDAtou(fashionsEle->Attribute("id"));
                saleInfo.level = Crown::SDAtou(fashionsEle->Attribute("level"));
                saleInfo.limit = Crown::SDAtou(fashionsEle->Attribute("limit"));
                saleInfo.product_id = Crown::SDAtou(fashionsEle->Attribute("product_id"));
                saleInfo.exploit = Crown::SDAtou(fashionsEle->Attribute("exploit"));
                m_seasonList.push_back(saleInfo); 
            }
        }

        if(strcmp(tableEle->Value(), "yesterday") == 0)
        {
            m_yesterdayCount = Crown::SDAtou(tableEle->Attribute("count"));
            m_yesterday3V3Count = Crown::SDAtou(tableEle->Attribute("three_count"));

            GameSettingManager::Instance()->SetMultiserverYesterdayNum(m_yesterdayCount);
            GameSettingManager::Instance()->SetMultiserverYesterday3V3Num(m_yesterday3V3Count);
        }
    }

    std::sort(m_levelList.begin(), m_levelList.end(), SortConfigMultiserverLeve);

    return true;
}

void MultiserverBattleConfig::LoadOpenTime(const std::string& base_value, MultiserverBatttleItem &item)
{
    std::vector<std::string> values = Crown::SDSplitStrings(base_value, '|');
    for(uint32 i = 0; i < values.size(); ++i)
    {
        std::vector<std::string> key_value = Crown::SDSplitStrings(values[i], '-');

        Property curproperty;
        curproperty.Clear();
        if(key_value.size() == 2)
        {
            // 读取活动的开始时间
            std::string startTimeString;
            startTimeString = key_value[0].c_str();
            curproperty.type = Crown::TimeToUint(startTimeString);

            startTimeString = key_value[1].c_str();
            curproperty.value = Crown::TimeToUint(startTimeString);
        }
        item.open_timer_list.push_back(curproperty);
    }
}

const MultiserverLevelItem* MultiserverBattleConfig::GetLevelInfo( int32 integral )
{
    /*
    MultiserverLevelItemList::iterator item = m_levelList.begin();
    if(id >= int32(item->point))
        return &(*item);

    for (; item != m_levelList.end(); item++)
    {
        if(id > int32(item->point))
            return &(*item);
    }
    return NULL;
    */

    MultiserverLevelItemList::iterator item = m_levelList.begin();
    if(integral <= int32(item->point))
        return &(*item);

    for (; item != m_levelList.end(); item++)
    {
        if(int32(item->point) >= integral )
            return &(*item);
    }

    return &(*item);

}

const MultiserverBatttleItem* MultiserverBattleConfig::GetBattleInfo( int32 type )
{
    for (MultiserverBatttleItemList::iterator item = m_battleList.begin(); item != m_battleList.end(); item++)
    {
        if(item->type == type)
            return &(*item);
    }
    return NULL;
}

const MultiserverSaleInfo* MultiserverBattleConfig::GetSaleItem(uint32 itemID)
{
    for (SaleList::iterator itemIt = m_saleList.begin(); itemIt != m_saleList.end(); itemIt++)
    {
        if(itemIt->product_id == itemID)
            return &(*itemIt);
    }
    return NULL;
}

int32 MultiserverBattleConfig::GetYesterdayCount()
{
    return m_yesterdayCount;
} 

uint32 MultiserverBattleConfig::GetMaxIntegral()
{
    if(m_levelList.size() == 0)
        return 0;

    MultiserverLevelItem item = m_levelList[m_levelList.size() - 1];
    return item.point;
}

void MultiserverBattleConfig::LoadRangeIntegral( const std::string& base_value, MultiserverBatttleItem &item, uint32 loadType)
{
    std::vector<std::string> values = Crown::SDSplitStrings(base_value, '-');
    if(values.size() >= 2)
    {
        if(loadType == 1)
        {
            item.start_range_integral = Crown::SDAtou(values[0].c_str());
            item.stop_range_integral = Crown::SDAtou(values[1].c_str());
        }
        else
        {
            item.fail_start_range_integral = Crown::SDAtou(values[0].c_str());
            item.fail_stop_range_integral = Crown::SDAtou(values[1].c_str());
        }
    }
    else
    {
        if(loadType == 1)
        {
            item.start_range_integral = 0;
            item.stop_range_integral = 0;
        }else{
            item.fail_start_range_integral = 0;
            item.fail_stop_range_integral = 0;
        }
    }
}

int32 MultiserverBattleConfig::GetRangeIntegral( int32 type , uint32 resultType)
{
    for(MultiserverBatttleItemList::iterator item = m_battleList.begin(); item != m_battleList.end(); item++)
    {
        if(item->type == type)
        {
            int32 interval = 0;
            int32 probability = 0;
            int32 currentIntegral = 0;
            //输赢类型取不同的值
            if(resultType == 1)
            {
                interval = item->stop_range_integral - item->start_range_integral;
                probability = Crown::SDRandom(interval);
                currentIntegral = item->start_range_integral + probability;
            }
            else
            {
                interval = item->fail_stop_range_integral - item->fail_start_range_integral;
                probability = Crown::SDRandom(interval);
                currentIntegral = item->fail_start_range_integral + probability;
            }
            return currentIntegral;
        }
    }
    return 0;
}

bool MultiserverBattleConfig::IsOpenTimer( int32 type )
{
    for (MultiserverBatttleItemList::iterator item = m_battleList.begin(); item != m_battleList.end(); item++)
    {
        if(item->type == type)
        {
            uint32 todayTime = GetTodayTime();
            
            bool openState = false;
            for (PropertyList::const_iterator it = item->open_timer_list.begin(); it != item->open_timer_list.end(); it++)
            {
                if(todayTime > (uint32)it->type && todayTime < (uint32)it->value)
                {
                    // 在开启时间之内
                    openState = true;
                    break;
                }
            }
            return openState;
        }
    }

    return false;
}

int32 MultiserverBattleConfig::GetTodayTime()
{
    Crown::CSDDateTime today_time = Crown::SDNow();// 今日凌晨时间点
    today_time.SetTime(0, 0, 0);

    int32 curTime = (int32)Crown::SDNow().GetTimeValue();
    return curTime - (int32)today_time.GetTimeValue();
}

const struct MultiserverSaleInfo* MultiserverBattleConfig::GetAllianceSaleItem( uint32 itemID )
{
    for (SaleList::iterator itemIt = m_allianceList.begin(); itemIt != m_allianceList.end(); itemIt++)
    {
        if(itemIt->product_id == itemID)
            return &(*itemIt);
    }
    return NULL;
}

const struct MultiserverSaleInfo* MultiserverBattleConfig::GetSeasonSaleItem( uint32 itemID )
{
    for (SaleList::iterator itemIt = m_seasonList.begin(); itemIt != m_seasonList.end(); itemIt++)
    {
        if(itemIt->product_id == itemID)
            return &(*itemIt);
    }
    return NULL;
}
