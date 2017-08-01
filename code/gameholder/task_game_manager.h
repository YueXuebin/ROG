#ifndef TASKGAME_MANAGER_H
#define TASKGAME_MANAGER_H

#include "task_define.h"

class Player;
class TaskManager;

// ==================================================
/// 角色的标准任务管理系统
// ==================================================
class TaskGameManager
{
public:
    TaskGameManager(Player* player);
    ~TaskGameManager();

    void    SetTaskManager(TaskManager* taskMgr) {m_task_mgr = taskMgr;}    // 设置任务管理器

    void    Clear();                                                        // 清除数据(只由TaskManager调用)

    void    LoadTaskGameInfo(const TASK_GAME_INFO& info);                   // 加载任务信息
    void    FillTaskGameInfo(TASK_GAME_INFO& info) const;                   // 填充任务信息

    //void    SetGuildID(uint32 guildID);                                     // 设置引导任务ID

    uint32  GetCompleteMainTaskID();                                        // 获得上个完成的主线任务ID
    void    SetCompleteMainTaskID(uint32 taskID);                           // 设置上个完成的主线任务ID

    void    CalDailyRefresh();                                              // 每日刷新

    void    RefreshDailyTask();                                             // 24小时重置日常等计数
    bool    RefreshCrusadeTask(std::vector<uint32>& del_list);              // 按设置的循环时间刷新一次讨伐令任务
    bool    RefreshAchiveTask(std::vector<uint32>& del_list);               // 刷新每日成就
    void    RefreshGuildTask();                                             // 刷新公会任务

    void    AddCanGameTakeTask(RoleTaskList* add_list);                    // 添加可以接受的任务
    void    AddTask(RoleTask* pTask);                                       // 添加任务(只由TaskManager调用)
    void    DelTask(RoleTask* pTask);                                       // 删除任务(只由TaskManager调用)

    // ====================================================================================================
    bool    IsBranchTaskFlag(uint32 taskID);                                // 分支任务是否被置位
    void    SetBranchTaskFlag(uint32 taskID);                               // 分支任务置位
    void    ResetBranchTaskFlag(uint32 taskID);                             // 分支任务复位

    // ====================================================================================================
    uint32  GetDailyTaskCount();                                            // 获得日常任务记数
    void    IncDailyTaskCount();                                            // 增加日常任务记数

    uint32  GetCrusadeTaskCount();                                          // 获得护卫任务次数
    void    IncCrusadeTaskCount();                                          // 增加护卫任务次数

    uint32  GetGuardTaskNum();                                              // 护卫任务次数

    uint32  GetGuardFreeNum();                                              // 获得护卫免费次数
    void    DecGuardFreeNum();                                              // 减少护卫免费次数

    uint32  GetGuradBuyNum();                                               // 获得护卫购买次数
    void    DecGuardBuyNum();                                               // 减少护卫购买次数

    uint32  GetGuildTaskTakeCount();                                        // 得到公会任务接受次数
    void    IncGuildTaskCount();                                            // 增加公会任务次数

    uint32  GetGuildScore() {return m_guild_info.guild_sroces;}             // 得到公会积分
    void    IncGuildScore(int32 score);                                     // 添加公会积分
    
    // 任务列表中:
    int32   GetDailyTaskTotal();                                            // 得到日常任务数
    int32   GetCrusadeCanTakeTotal();                                       // 得到讨伐令可接任务数
    int32   GetCrusadeHaveTaskTotal();                                      // 得到讨伐令已接任务数
    int32   GetGuardTaskTotal();                                            // 得到护卫任务数
    int32   GetGuildTaskTotal();                                            // 得到公会任务数
    int32   GetGuildHaveTaskTotal();                                        // 得到公会任务已接数
    int32   GetGuildCompleteTaskTotal();                                    // 得到公会任务已完成数

 
    bool    IsOpenGuildTaskBox(int32 boxScore);                             // 公会宝箱是否被领过
    // ====================================================================================================
    void    OnTaskCrusadeRefreshReq();                                      // 刷新讨伐任务
    void    OnTaskCrusadeGiveUpReq(void* data);                             // 放弃讨伐任务
    void    OnTaskReceiveBoxReq(void* data);                                // 领取宝箱
    void    OnTaskRefreshTimeReq();                                         // 刷新任务时间
    void    OnTaskGuardRefreshReq();                                        // 变更护卫任务
    void    OnTaskDailyResetReq(void* data);                                // 日常任务重置请求
    void    OnTaskGuardBuyReq(void* data);                                  // 护卫任务次数购买请求
    void    OnTaskGuildResetReq(void* data);                                // 公会任务重置请求
    void    OnTaskGuildBoxRewardReq(void* data);                            // 公会任务领取宝箱

    // ====================================================================================================
    void    NotifyGameTaskNum();                                            // 通知任务剩余数量
    void    NotifyGameTaskRefresh();                                        // 通知任务刷新时间

    // ====================================================================================================
    void    CBDailyReset();                                                 // 任务重置应答
    void    CBGuardBuy(int32 num);                                                   // 护卫任务购买应答

    // ====================================================================================================
    void    AddMainTask(RoleTaskList* add_list);                           // 添加主线任务
    void    AddBranchTask(RoleTaskList* add_list);                         // 添加支线任务
    void    AddDailyTask(RoleTaskList* add_list);                          // 添加日常任务
    void    AddCrusadeTask(RoleTaskList* add_list);                        // 添加讨伐令任务
    void    AddGuardTask(RoleTaskList* add_list);                          // 添加护卫任务
    void    AddGuildTask(RoleTaskList* add_list);                          // 添加公会任务
    void    AddAchiveTask(RoleTaskList* add_list);                         // 添加成就任务

    bool CardByTaskGuardNum(int32 num);

private:
    TaskGameManager( const TaskGameManager& );
    TaskGameManager& operator=( const TaskGameManager& );


    void    SaveYesterdayData();                                            // 保存昨天数据

     // ====================================================================================================
    void    SendTaskBoxAck(uint16 errcode, uint32 box_id);                  // 领取宝箱奖励
    void    SendTaskDailyResetAck(uint16 errcode);                          // 发送日常任务重置应答
    void    SendTaskGuardBuyAck(uint16 errcode);                            // 护卫购买应答

    int32   GetTaskTotal(uint8 task_type, uint8 task_state = 0);            // 获取任务数量, task_state=0则忽略任务状态

private:
    Player*         m_player;                                               // 玩家
    TaskManager*    m_task_mgr;                                             // 任务管理器

    // ========== 游戏任务数据 ============================================================================
    //uint32          m_guide_id;                                             // 引导ID
    uint32          m_CompleteMainTaskID;                                   // 已完成的主线任务ID

    TASK_DAILY_INFO     m_daily_info;                                       // 日常任务信息
    TASK_CRUSADE_INFO   m_crusade_info;                                     // 讨伐令任务信息
    TASK_GUARD_INFO     m_guard_info;                                       // 护卫任务信息
    TASK_GUILD_INFO     m_guild_info;                                       // 公会任务信息
    TASK_ACHIVE_INFO    m_achive_info;                                      // 成就任务信息
    uint16              m_open_box;                                         // 箱子是否开过 按位存储

    RoleTaskList    m_tasks;                                                // 已接的标准任务列表(从数据库中捞出来的)
    uint32          m_branch_task_flag[TASK_BRANCH_TASK_FLAG_LEN];          // 分支任务标记
};

#endif
