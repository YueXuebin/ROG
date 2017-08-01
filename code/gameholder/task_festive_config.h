#ifndef task_activity_config_h__
#define task_activity_config_h__

#include "task_define.h"

class Player;


// 活动任务配置(数据从CenterServer获得)
class TaskFestiveConfig
{
public:
    TaskFestiveConfig();
    ~TaskFestiveConfig();

    DECLARE_SINGLETON(TaskFestiveConfig)

    void    OnRecvFromCenter(uint32 centerID, uint32 msgID, void* data);                                // 接收消息

    void    FillTaskFestivalList(uint32 centerID, TASK_FESTIVAL_TEMPLATE_LIST& festivalList);           // 填充任务活动列表

    const TASK_FESTIVAL_TEMPLATE*   GetFestivalTmpl(uint32 centerID, uint32 festivalID);                 // 获得活动模板
    const TASK_TEMPLATE*            GetTaskTmpl(uint32 centerID, uint32 taskID);                         // 获得任务模板
    const TaskFestiveMap*           GetFestivalMap(uint32 centerID);

    void GetCanTakeList(uint32 centerID, const std::set<uint32>& idSet, TaskTemplateList& tmplList);      // 获得可接任务模板列表

    bool GetFestivalLoop(uint32 centerID, uint32 festivalID);                                           // 是否循环活动

    bool IsInValidTime( uint32 centerID, uint32 festivalID );                                           // 是否在有效时间
    bool IsInActivityValidTime( uint32 centerID, uint32 festivalID, uint32 time );                      // 时间是否在有效时间内

    uint32 GetActivityBeginTime( uint32 centerID, uint32 festivalID );                                  // 获取游戏的开始时候

private:
    void    OnLoadFestivalTmplList(uint32 centerID, void* data);                                        // 加载活动数据

    void    AddDBFestivalInfo(TaskFestiveMap& festivalMap, const TASK_FESTIVAL_TEMPLATE& festivalTmpl); // 添加活动信息

    bool    HasCenterIDFestive(uint32 centerID);

private:

    std::map<uint32, TaskFestiveMap>         m_dbCenterFestivalMap;                                     // 每个CenterServer上的活动数据 <centerID, TaskFestiveMap>
};

#endif // task_festival_config_h__
