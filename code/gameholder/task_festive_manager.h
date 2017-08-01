#ifndef TASK_FESTIVE_MANAGER_H
#define TASK_FESTIVE_MANAGER_H

#include "task_define.h"

class Player;
class TaskManager;

// ==================================================
/// 角色的活动任务管理系统
// ==================================================
class TaskFestiveManager
{
public:
    TaskFestiveManager(Player* player);
    ~TaskFestiveManager();

    void SetTaskManager(TaskManager* taskMgr) {m_task_mgr = taskMgr;}

    void        Clear();                                                    // (只由TaskManager调用)

    void        LoadTaskFestivalInfo(const TASK_FESTIVAL_INFO& info);       // 加载任务信息
    void        FillTaskFestivalInfo(TASK_FESTIVAL_INFO& info) const;       // 填充任务信息

    void        AddCanTakeTaskFestival(RoleTaskList* add_list);             // 添加可以接受的活动任务

    void        AddTask(RoleTask* pTask);                                   // 添加任务(只由TaskManager调用)
    void        DelTask(RoleTask* pTask);                                   // 删除任务(只由TaskManager调用)
    void        ResetTask(RoleTask* pTask);                                 // 重置任务(只由TaskManager调用)

    void        OnFestivalRewardReq(void* data);                            // 请求节日奖励

    void        AddFestivalTask(RoleTaskList* add_list);                    // 添加活动任务

    RoleTask*   GetFestivalTask(uint32 taskID);                             // 获得活动任务

private:
    TaskFestiveManager( const TaskFestiveManager& );
    TaskFestiveManager& operator=( const TaskFestiveManager& );
    TaskFestiveManager* operator&();
    const TaskFestiveManager* operator&() const;

    void        SendFestivalRewardAck(uint16 errcode, const FESTIVAL_RECORD& festivalRecord);   // 发送节日奖励应答

    FESTIVAL_RECORD* GetFestivalRecord(uint32 festivalID);                  // 获得节日记录

private:
    Player*             m_player;                                           // 玩家
    TaskManager*        m_task_mgr;                                         // 任务管理器

    RoleTaskList        m_tasks;                                            // 已接的活动任务列表
    FestivalRecordList  m_festivals;                                        // 活动任务记录
};

#endif
