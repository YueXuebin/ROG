#ifndef task_data_h__
#define task_data_h__


#include "rtti.h"
#include "task_target.h"
#include "task_define.h"

class Player;
class TaskManager;

// ==================================================
// 角色的任务数据结构
// ==================================================
class RoleTask
{
    CnDeclareRootRTTI(RoleTask)
public:
    RoleTask(const TASK_TEMPLATE* task_template, Player* player);
    virtual ~RoleTask() = 0;

    Player* GetPlayer() {return m_player;}

    void FillTaskRecord(TASK_RECORD& taskRecord) const;
    void LoadTaskRecord(const TASK_RECORD& taskRecord);

    uint32  GetTaskID() const {return m_taskID;}

    uint8   GetState() {return m_state;}
    void    SetState(uint8 state);              // 设置任务状态

    void    MakeTaskComplete();                 // 使任务完成
    bool    CheckTaskComplete();                // 检测任务是否完成，如果完成则变状态为完成

    int32   GetLoopNum() {return m_loop_num;}
    void    SetLoopNum(int32 num) {m_loop_num = num;}
    bool    CanLoop();                          // 任务是否再次循环
    void    DoLoop();                           // 使任务循环
    void    Cancel();                           // 放弃任务

    bool IsDispose() {return m_IsDispose;}      // 调试用

    bool IsRaid() {return m_template->raids != 0;}          // 任务是否可以扫荡
    bool NeedTriggerTaskTarget();                           // 该任务是否可以触发

    bool CheckTaskPreCondition(uint32* err = NULL);         // 任务前置条件是否满足

    uint32 GetFestivalID() {return m_template->festival_id;}
    uint8 GetSubType(){ return m_template->sub_type; }

    uint32 GetDungeonIDByTrigger(uint32 triggerID);
    uint32 GetChangeDungeonID();
    int32  GetTaskTargetNum(uint32 targetIndex);

    uint8  GetQuality() {return m_quality;}
    void   IncQuality() {m_quality++;}

    uint8  GetPosition() {return m_position;}                 // 任务位置
    void   SetPosition(uint8 pos){ m_position = pos;}         // 设置任务位置 

    const REWARD_TABLE& GetGiveReward() {return m_template->gives;}
    const REWARD_TABLE& GetReward();

    void OpenDungeonByTask();           // 开启任务对应的副本

    bool RefreshLoopTaskState();
    void SetTaskHaveTakeState();

    //////////////////////////////////////////////////////
    // 任务触发(返回值表示任务状态是否发生改变)
    bool OnExchangeItem(uint32 itemID);                                                     // 物品变化触发任务
    bool OnKillMonster(uint32 monsterID, int32 num, uint32 mapID, bool isRadis=false);      // 杀死怪物触发任务
    
    bool CheckTaskAutoReward();                 // 判断是否自动获得奖励
    void ResetTask();                           // 重置任务

    //////////////////////////////////////////////////////
    // 任务逻辑
    virtual void OnAdd();                       // 添加新任务
    virtual void OnPreRewards() {}              // 获取任务奖励前处理
    virtual void OnRewards() = 0;               // 获取任务奖励

    // 子类重载
    virtual uint8 GetType() = 0;

    virtual bool IsInValidTime() = 0;                       // 任务是否在有效时间内
    virtual bool IsNeedOnlineTime() = 0;                    // 是否需要记录在线时长

    virtual bool InitTaskLinkage() = 0;          // 状态触发的任务目标初始化
    virtual bool OnTriggerTaskTarget(uint32 targetType, uint32 arg1 = 0, uint32 arg2 = 0) = 0;      // 任务目标触发

protected:
    void Clear();
    bool HasTargetByType(uint32 targetType);
    bool HasTargetByID(uint32 targetID);
    void RemoveTarget(uint32 targetID);
    bool IsEmptyTemplate();

protected:
    Player* m_player;               // 任务的主人
    TaskManager*    m_TaskManager;  // 任务管理器

    const TASK_TEMPLATE*            m_template;         // 任务模版
    uint8                           m_state;            // 任务状态
    uint8                           m_position;         // 任务位置
    
    uint32                          m_taskID;           // 任务ID
    uint32                          m_rate_of_complete; // 完成万分比
    uint8                           m_quality;          // 任务品质
    int32                           m_loop_num;         // 任务循环做的次数
    RoleTaskTargetList              m_targets;          // 任务目标
private:
    bool    m_IsDispose;            // 是否被释放了(调试用)
    bool    m_IsInitTarget;         // 初始化任务目标
};




#endif // task_data_h__
