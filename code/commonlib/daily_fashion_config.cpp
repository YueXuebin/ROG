#include "commonlib_pch.h"
#include "daily_fashion_config.h"
#include "config_reader.h"

IMPLEMENT_SINGLETON(DailyFashionConfig)

DailyFashionConfig::DailyFashionConfig(void)
{
}

DailyFashionConfig::~DailyFashionConfig(void)
{
}

bool DailyFashionConfig::LoadConfig( const char* path )
{
    TiXmlDocument xmlDoc;

    if (!xmlDoc.LoadFile(path))
    {
        CnError("Load daily_draw_fashion.xml file: %s failed\n", path);
        return false;
    }

    TiXmlElement* xmlRoot = xmlDoc.RootElement();
    if(!xmlRoot) return false;

    for(TiXmlElement* xmlEle = xmlRoot->FirstChildElement(); xmlEle; xmlEle = xmlEle->NextSiblingElement())
    {
        if(strcmp(xmlEle->Value(), "config_version") == 0)
        {
            xmlEle->QueryIntAttribute("value", &m_dailyDrawFashionConfig.config_version);
        }
        else if(strcmp(xmlEle->Value(), "week_list") == 0)
        {
            for (TiXmlElement* buyCheapEle = xmlEle->FirstChildElement(); buyCheapEle; buyCheapEle = buyCheapEle->NextSiblingElement())
            {
                DAILY_DRAW_FASHION_WEEK weekItem;
                buyCheapEle->QueryIntAttribute("id", &weekItem.id);
                buyCheapEle->QueryIntAttribute("fashion_id", &weekItem.fashion_id);
                buyCheapEle->QueryIntAttribute("ten_pid", &weekItem.ten_pid);
                ConfigReader::Read(buyCheapEle, "show_item", weekItem.show_item);

                std::string  strvalue = "";
                ConfigReader::Read(buyCheapEle, "daylist", strvalue);
                std::vector<std::string> values = Crown::SDSplitStrings(strvalue, '|');
                for(uint32 i=0; i<values.size(); ++i)
                {
                    weekItem.daylist.push_back(SDAtoi(values[i].c_str()));
                }

                m_dailyDrawFashionConfig.weeks.push_back(weekItem);
            }
        }
        else if(strcmp(xmlEle->Value(), "day_list") == 0)
        {
            for (TiXmlElement* buyCheapEle = xmlEle->FirstChildElement(); buyCheapEle; buyCheapEle = buyCheapEle->NextSiblingElement())
            {
                DAILY_DRAW_FASHION_DAY dayItem;
                buyCheapEle->QueryIntAttribute("id", &dayItem.id);
                buyCheapEle->QueryIntAttribute("one_pid", &dayItem.one_pid);
                ConfigReader::Read(buyCheapEle, "show_item", dayItem.show_item);
                m_dailyDrawFashionConfig.days.push_back(dayItem);
            }
        }
        else if(strcmp(xmlEle->Value(), "need") == 0)
        {
            xmlEle->QueryIntAttribute("one_money", &m_dailyDrawFashionConfig.one_money);
            xmlEle->QueryIntAttribute("ten_money", &m_dailyDrawFashionConfig.ten_money);
        }
    }

    return true;
}

bool DailyFashionConfig::DBSetConfig( std::string dailyDrawFashionConfig, Json::Value* jsonValue /*= NULL*/ )
{
    /*
    m_dailyDrawFashionConfig.Clear();
    if(!JsonToStruct(m_dailyDrawFashionConfig, dailyDrawFashionConfig, jsonValue))
    {
        CnError("daily fashion config json to sturct error!");
        return false;
    }
    */
    return true;
} 

uint32 DailyFashionConfig::GetDailyDrawFashionPackageID( uint32 week, uint32 day, uint32 drawType )
{
    uint32 dayId = 0;
    for (std::vector<DAILY_DRAW_FASHION_WEEK>::const_iterator weekIt = m_dailyDrawFashionConfig.weeks.begin(); weekIt != m_dailyDrawFashionConfig.weeks.end(); weekIt++)
    {
        if(weekIt->id == week)
        {
            if(drawType == 2)
            {
                return weekIt->ten_pid;
            }
            else
            {
                dayId = weekIt->daylist[day];
            }
        }
    }

    for (std::vector<DAILY_DRAW_FASHION_DAY>::const_iterator dayIt = m_dailyDrawFashionConfig.days.begin(); dayIt != m_dailyDrawFashionConfig.days.end(); dayIt++)
    {
        if(dayIt->id == dayId)
        {
            return dayIt->one_pid;
        }
    }

    return 0;
}

REWARD_ITEM DailyFashionConfig::GetDailyDrawFashionWillItem( uint32 week )
{
    REWARD_ITEM item;
    for (std::vector<DAILY_DRAW_FASHION_WEEK>::const_iterator weekIt = m_dailyDrawFashionConfig.weeks.begin(); weekIt != m_dailyDrawFashionConfig.weeks.end(); weekIt++)
    {
        if(weekIt->id == week)
        {
           
           LoadItemToString(weekIt->show_item, item);
        }
    }
    return item;
}

uint32 DailyFashionConfig::GetConsumeMoney(uint32 type)
{
    if(type == 2)
    {
        return m_dailyDrawFashionConfig.ten_money;
    }
    else
    {
        return m_dailyDrawFashionConfig.one_money;
    }
}

uint32 DailyFashionConfig::GetNextDayId( DAILY_DRAW_FASHION_INFO* info )
{
    Crown::CSDDateTime now = Crown::SDNow();
    uint32 cweek = 0;
    CnDbg("next daily fashion init %d \n", (uint32)now.GetTimeValue());
    if(info->cur_week == 0)
    {
        CnDbg("cur_week = 0 %d \n", (uint32)now.GetTimeValue());
       info->cur_week = m_dailyDrawFashionConfig.weeks[0].id;
       
       cweek = now.GetWeek();
       if( cweek == 0 )
           cweek = 7;

       info->cur_day = cweek;

       info->next_week = m_dailyDrawFashionConfig.weeks[1].id;
       info->last_time = (uint32)now.GetTimeValue();
        return 1;
    }
    Crown::CSDDateTime lastSignTime;

    lastSignTime.SetTimeValue(info->last_time);
    lastSignTime.SetTime(0,0,0);
    lastSignTime.IncDay(1);

    CnDbg("judge time now= %d last= %d \n", (uint32)now.GetTimeValue() ,lastSignTime.GetTimeValue());
    if(now.GetTimeValue() > lastSignTime.GetTimeValue() )
    {
        CnDbg("judge day cur_day= %d now_day= %d \n", info->cur_day , now.GetWeek());

        // ³¬¹ýÒ»Ìì
        if(info->cur_day == 7)
        {
            
            info->cur_week = info->next_week;

            uint32 nextWeekIndex = 0;
            /*
            for(int i = 0; i < m_dailyDrawFashionConfig.weeks.size(); i++)
            {
                if((uint32)m_dailyDrawFashionConfig.weeks[i].id == (uint32)info->cur_week)
                {
                    nextWeekIndex = i;
                    break;
                }
            }
            */
            for (std::vector<DAILY_DRAW_FASHION_WEEK>::const_iterator weekIt = m_dailyDrawFashionConfig.weeks.begin(); weekIt != m_dailyDrawFashionConfig.weeks.end(); weekIt++)
            {
                if(weekIt->id == info->cur_week)
                {
                    break;
                }
                nextWeekIndex++;
            }
            
            
            if(nextWeekIndex + 1 >= m_dailyDrawFashionConfig.weeks.size())
            {
                nextWeekIndex = 0;
            }
            else
            {
                nextWeekIndex = nextWeekIndex + 1;
            }

            cweek = now.GetWeek();
            if( cweek == 0 )
                cweek = 7;
            CnDbg("judge cur_day == 7 next day cur_week %d next_week= %d \n", info->cur_week, info->next_week);
            info->cur_day = cweek;
            info->next_week =  m_dailyDrawFashionConfig.weeks[nextWeekIndex].id;
            info->last_time = (uint32)now.GetTimeValue();
        }
        else
        {
            cweek = now.GetWeek();
            if( cweek == 0 )
                cweek = 7;

            info->cur_day = cweek;
            info->last_time = (uint32)now.GetTimeValue();
        }
        return 1;
    }

    return 0;
}



void DailyFashionConfig::LoadItemToString( const std::string& base_value, REWARD_ITEM& reward_item )
{
    std::vector<std::string> key_value = Crown::SDSplitStrings(base_value, '-');

    //reward_item.Clear();
    //if(key_value.size() == 3)
    //{
    //    reward_item.type = SDAtoi(key_value[0].c_str());
    //    reward_item.id = SDAtoi(key_value[1].c_str());
    //    reward_item.num = SDAtoi(key_value[2].c_str());
    //}
    //else if(key_value.size() == 2)
    //{
    //    reward_item.type = SDAtoi(key_value[0].c_str());
    //    reward_item.id = SDAtoi(key_value[0].c_str());
    //    reward_item.num = SDAtoi(key_value[1].c_str());
    //}
}
