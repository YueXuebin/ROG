#ifndef TASK_MANAGER_H
#define TASK_MANAGER_H

#include "task_define.h"
#include "task_game_manager.h"
#include "task_festive_manager.h"


class Player;
class RoleTaskTarget;
class TaskManageAgent;

// ==================================================
/// 角色的任务管理系统
// ==================================================
class TaskManager
{
    friend class TaskManageAgent;
    friend class TaskGameManager;
    friend class TaskFestiveManager;
public:
    TaskManager(Player* player);
    ~TaskManager();

    void    Clear();

    void    LoadTaskInfo(const TASK_INFO& task_info);                       // 加载任务信息
    void    FillTaskInfo(TASK_INFO& task_info) const;                       // 填充任务信息

    void    LoadRoleTask(const std::vector<TASK_RECORD>& in_list);          // 加载任务记录
    void    ReloadTask();                                                   // 重新加载节日任务

    void    OnRecv(uint32 msgID, void* data);

    void    SendTaskEvent(uint32 event_id, int32 arg1, int32 arg2, int32 arg3);                    // 发送任务事件
    void    ReceiveEvent( uint32 frameCount, uint32 eventId, int32 arg1, int32 arg2, int32 arg3);  //任务接收处理

    void    CalDailyRefresh();                                              // 刷新每日任务

    void    CalShowTriggerFlag();                                           // 重新计算显示Trigger的标记
    uint32  GetDungeonIDByTrigger(uint32 triggerID);                        // 通过TriggerID获得副本ID
    bool    NeedDisappearTrigger(uint32 triggerID);                         // 检查是否需要隐藏此触发器

    void    NotifyGameTaskNum();                                            // 通知任务剩余数量

    void    SetTaskHaveTakeState(RoleTask* task);                           // 设置任务没有完成状态

    bool    HasTask(uint32 taskID);                                         // 在已有的任务列表中，是否有此任务
    bool    HasFriendTask();                                                // 是否已接好友任务

    bool    CheckTaskPreCondition(const TASK_TEMPLATE* taskTmpl, uint32* err = NULL);  // 判定任务的前置条件

    void    LinkageCanTakeTask();                                           // 联动可添加任务
    void    AddCanTakeTask(RoleTaskList* add_list);                        // 添加可以接受的任务（主线和支线任务）

    void    GmAddTask(uint32 taskID);                                       ///< 通过GM命令添加任务
    void    GmCompleteTask();                                               ///< 强制完成任务
    void    GmTakeTask(uint32 taskID);                                      ///< 接受任务
    void    GmTaskChangeDugeon(uint32 taskID);                              ///< 切换副本
    void    GmTaskReward(uint32 taskID);                                    ///< 请求任务奖励
    void    GmTaskQualityReq(uint32 taskID);                                ///< 提高任务品质
    void    GmTaskComplete(uint32 taskID);                                  ///< 立即完成任务
    void    GmTaskRefresh();                                                ///< 刷新讨伐令任务
    void    GmTaskReceiveBox(uint32 boxID);                                 ///< 领取宝箱
    void    GmTaskRefreshTimeReq();                                         ///< 刷新任务时间
    void    GmTaskGuardRefresh();                                           ///< 变更护卫任务

    // ========== 任务功能 ==========
    void    AddTask(const TASK_TEMPLATE* taskTmpl, RoleTaskList* add_list);    // 添加任务
    void    ResetTask(std::vector<uint32>& taskID_list);                        // 重置任务

    // ========== 任务触发 ==========
    void    OnExchangeItem(uint32 itemID);                                                  // 物品数量发生改变触发
    void    OnSettleDungeon(uint32 dungeonID, bool success, bool raids = false);            // 结算副本触发任务
    // ==============================

    void    DelTaskByTypeAndState(std::vector<uint32>& del_list, uint8 task_type, uint8 task_state = 0, uint8 task_nostate = 0);    // 删除一类任务, task_state=0则忽略任务状态

    bool    IsStartCountOnlineTime();                                       // 是否需要统计在线总时长

    uint32  GetMainTaskID();                                                ///< 返回主线任务ID
    uint32  GetGuardTaskNum();                                              ///< 获得护卫任务次数

    uint32  GetAchiveCount();                                                ///< 获取成功任务个数

    void    CBDailyReset();                                                 ///< 任务重置应答
    void    CBGuardBuy();                                                   ///< 护卫任务购买应答

    void    SendTaskRewardAck(uint16 errcode, uint32 task_id);              ///< 发送任务奖励信息
    void    SendTaskItemAck(uint16 errcode, uint32 item_id);                ///< 发送物品信息

    TaskTemplateList GetCanAddTaskList(const TaskTemplateMap* taskList);         ///< 得到所有可添加不重复的日常任务列表

    void    RefreshLoopTaskState();                                         // 每日刷新跟新相关的任务数

    void    InitTaskState();                                             // 检查任务的联动状态

    void    AddGuildScore(int32 score);                                     // 添加公会积分

    void    OnTaskFestivalTemplateListReq();                                    // 运营活动模板列表

    void    NotifyTaskChange(std::vector<uint32>& del_list, RoleTaskList& add_list);   // 通知任务添加或删除
    
    TaskGameManager*    GetTaskGameManager() {return &m_task_game_mgr;}
    RoleTaskList GetRoleTasks(){ return m_pTasks;}                             // 获取玩家身上的已接任务列表

    bool ResetTaskFestival( uint32 centerID, uint32 festivalID );                     // 重置任务 

private:
    Player* GetPlayer() { return m_player; }                                // 获得角色指针

    void    FillTaskRecordList(const RoleTaskList& in_list, std::vector<TASK_RECORD>& out_list) const; // 填充任务记录列表

    void    AddRoleTask(RoleTask* pTask);                                   // 添加角色任务
    void    DelRoleTask(RoleTask* pTask);                                   // 删除角色任务

    RoleTask*   GetTaskByIdAndState(uint32 taskId, uint32 state);
    RoleTask*   GetTaskById(uint32 taskId);

    void    OnTaskChangeDungeonReq(void* data);                             // 任务切换副本
    void    OnTakeTaskReq(void* data);                                      // 接任务请求
    void    OnTalkNpc(void* data);                                          // 与NPC对话
    void    OnMicroClient(void* data);                                      // 微端登陆
    void    OnTaskRewardReq(void* data);                                    // 请求任务奖励
    void    OnTaskQualityReq(void* data);                                   // 提升品质请求
    void    OnTaskCompleteReq(void* data);                                  // 立即完成任务
    void    OnTaskMicroClientReq(void* data);                               // 微端状态

    void    SendTaskInfo();                                                 // 发送任务信息

    void    NotifyTaskUpdate(RoleTask* task);                               // 通知任务内容更新
    void    NotifyTaskUpdate(RoleTaskList& list);                           // 通知任务内容更新
    
    bool    CanAddGive(const REWARD_TABLE& givesReward);                    // 检查是否可以给予物品
    bool    CanAddReward(const REWARD_TABLE& taskReward);                   // 检查是否可以赠与物品
    void    GetGives(const REWARD_TABLE& givesRewards);                     // 给予物品
    void    GetRewards(RoleTask* taskIt);                                   // 获得奖励
    bool    ValidateVigor(RoleTask* taskIt);                                // 验证体力是否够用
    

    void    CompleteTask(RoleTask* task);                                   // 完成任务

    void    OnSendTaskLog(uint32 agreementid , RoleTask* roleTask);         // 任务行为日志

    bool    CanTakeTaskByTaskCount(uint8 type);                             // 是否能接受任务(任务数超过最大值)


private:
    Player*             m_player;                                           // 玩家

    TaskGameManager     m_task_game_mgr;                                    // 游戏任务数据管理器
    TaskFestiveManager  m_task_festive_mgr;                                 // 活动任务数据管理器

    TaskManageAgent*    m_task_manage_agent;                                // 任务触发管理器

    // 任务数据
    RoleTaskList        m_pTasks;                                           // 已接的任务列表

};

#endif
