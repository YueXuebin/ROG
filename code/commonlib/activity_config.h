/*
*	活动时间表
*/
#ifndef activity_config_h__
#define activity_config_h__

class ActivitySchedule
{
public:
    void Clear()
    {
        activity_id     = 0;
        activity_type   = 0;
        period_type     = 0;
        arg_id          = 0;
    }
    uint32  activity_id;                            // 活动ID
    uint32  activity_type;                          // 活动类型
    uint32  period_type;                            // 周期类型
    uint32  arg_id;                                 // 世界BossID
    Crown::CSDDateTime start_time;                  // 开始时间
    Crown::CSDDateTime end_time;                    // 结束时间
};

class ActivityRegular
{
public:
    void Clear()
    {
        type = 0;
        level = 0;
        count = 0;
        dungeon_id = 0;
    }

    uint32  type;                           // 活动类型
    int32   level;                          // 活动等级
    int32   count;                          // 可进入次数
    int32   dungeon_id;                     // 副本ID
};


class TiXmlElement;

class ActivityConfig
{
public:
    ActivityConfig();
    ~ActivityConfig();

    DECLARE_SINGLETON(ActivityConfig)

    bool LoadActivitySchedule(const char* path);    // 加载活动时间表

    bool LoadActivityRegular(const char* path);     // 加载活动规则

    std::vector<ActivitySchedule>& GetScheduleList() {return m_scheduleList;}

    ActivityRegular* GetActivityRegular(uint32 activityType);

    ActivityRegular* GetActivityrRegularByDungeonId(uint32 dungeon_id);
private:
    void ParseSchedule(TiXmlElement* activityEle, ActivitySchedule& schedule, uint32 PeriodType);

    std::vector<ActivitySchedule>       m_scheduleList;
    std::map<uint32, ActivityRegular>   m_regularMap;
};

#endif // activity_config_h__
