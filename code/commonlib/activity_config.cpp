#include "commonlib_pch.h"
#include "activity_config.h"

IMPLEMENT_SINGLETON(ActivityConfig)

ActivityConfig::ActivityConfig()
{

}

ActivityConfig::~ActivityConfig()
{

}

bool ActivityConfig::LoadActivitySchedule(const char* path)
{
    TiXmlDocument xmlDoc;
    if (!xmlDoc.LoadFile(path))
    {
        CnError("Load activity schedule file: %s failed\n", path);
        return false;
    }

    TiXmlElement* xmlRoot = xmlDoc.RootElement();
    if(!xmlRoot)
        return false;

    ActivitySchedule activitySchedule;
    activitySchedule.Clear();

    for(TiXmlElement* periodEle = xmlRoot->FirstChildElement(); periodEle; periodEle = periodEle->NextSiblingElement())
    {
        // 天为周期的活动
        if(strcmp(periodEle->Value(), "day") == 0)
        {
            for(TiXmlElement* activityEle = periodEle->FirstChildElement(); activityEle; activityEle = activityEle->NextSiblingElement())
            {
                ParseSchedule(activityEle, activitySchedule, PERIOD_DAY);
                m_scheduleList.push_back(activitySchedule);
            }
        }
        // 周为周期的活动
        if(strcmp(periodEle->Value(), "week") == 0)
        {
            for(TiXmlElement* activityEle = periodEle->FirstChildElement(); activityEle; activityEle = activityEle->NextSiblingElement())
            {
                ParseSchedule(activityEle, activitySchedule, PERIOD_WEEK);
                m_scheduleList.push_back(activitySchedule);
            }
        }
        // 月为周期的活动
        if(strcmp(periodEle->Value(), "month") == 0)
        {
            for(TiXmlElement* activityEle = periodEle->FirstChildElement(); activityEle; activityEle = activityEle->NextSiblingElement())
            {
                ParseSchedule(activityEle, activitySchedule, PERIOD_MONTH);
                m_scheduleList.push_back(activitySchedule);
            }
        }
    }

    return true;
}

// ==================================================
// 加载活动规则
// ==================================================
bool ActivityConfig::LoadActivityRegular(const char* path)
{
    ActivityRegular activityRegular;

    TiXmlDocument xmlDoc;
    if (!xmlDoc.LoadFile(path))
    {
        CnError("Load activity regular file: %s failed\n", path);
        return false;
    }

    TiXmlElement* xmlRoot = xmlDoc.RootElement();
    if(!xmlRoot)
        return false;

    
    for(TiXmlElement* activityEle = xmlRoot->FirstChildElement(); activityEle; activityEle = activityEle->NextSiblingElement())
    {
        activityRegular.Clear();
        activityEle->QueryUnsignedAttribute("type", &activityRegular.type);
        activityEle->QueryIntAttribute("level", &activityRegular.level);
        activityEle->QueryIntAttribute("count", &activityRegular.count);
        activityEle->QueryIntAttribute("dungeon_id", &activityRegular.dungeon_id);

        if(m_regularMap.find(activityRegular.type) != m_regularMap.end())
        {
            CnError("Duplicate activity regular type:%u\n", activityRegular.type);
        }
        else
        {
            m_regularMap[activityRegular.type] = activityRegular;
        }
    }

    return true;
}

ActivityRegular* ActivityConfig::GetActivityRegular(uint32 activityType)
{
    ActivityRegular* activityRegular = 0;
    std::map<uint32, ActivityRegular>::iterator it;

    it = m_regularMap.find(activityType);
    if(it == m_regularMap.end())
    {
        return NULL;
    }

    return &it->second;
}

void ActivityConfig::ParseSchedule(TiXmlElement* activityEle, ActivitySchedule& schedule, uint32 PeriodType)
{
    activityEle->QueryUnsignedAttribute("activity_id", &schedule.activity_id);
    activityEle->QueryUnsignedAttribute("activity_type", &schedule.activity_type);
    activityEle->QueryUnsignedAttribute("arg_id", &schedule.arg_id);

    // 读取活动的开始时间
    std::string startTimeString;
    startTimeString = activityEle->Attribute("start_time");
    schedule.start_time.FromString(startTimeString);

    // 读取活动的结束时间
    std::string endTimeString;
    endTimeString = activityEle->Attribute("end_time");
    schedule.end_time.FromString(endTimeString);

    schedule.period_type = PeriodType;
}

ActivityRegular* ActivityConfig::GetActivityrRegularByDungeonId(uint32 dungeon_id)
{
    std::map<uint32, ActivityRegular>::iterator regularIt = m_regularMap.begin();
    for(; regularIt != m_regularMap.end(); ++regularIt)
    {
        if(regularIt->second.dungeon_id == dungeon_id)
        {
            return &regularIt->second;
        }
    }

    return NULL;
}
