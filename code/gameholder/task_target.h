#ifndef task_target_h__
#define task_target_h__

class RoleTask;
class Player;

// ==================================================
/// 角色的任务目标结构
// ==================================================
class RoleTaskTarget
{
public:
    RoleTaskTarget(RoleTask* pTask);
    ~RoleTaskTarget();

    void FillTaskTargetRecord(TASK_TARGET_RECORD& taskRecord) const;
    void LoadTaskTargetRecord(const TASK_TARGET_RECORD& taskRecord, const TASK_TEMPLATE* m_template);
    void InitByTargetTmpl(const TASK_TARGET& taskTarget);

    const TASK_TARGET*  GetTargetTmpl() {return m_target;}
    uint32  GetTargetType();
    uint32  GetTargetValue();
    uint32  GetTargetMapID();
    uint32  GetTargetNpcID();
    int32   GetTargetRate();

    bool IsEnough() {return m_num >= m_max_num;}
    void Complete() {m_num = m_max_num;}    // 设置该目标完成

    void Refrash();             // 目标重置

    uint32 GetID() {return m_id;}
    int32 GetNum() {return m_num;}
    int32 GetMaxNum() {return m_max_num;}

    bool UpdateTargetState();

    ///////////////////////////////////////////////////////////////
    // 事件触发(返回值表示目标状态是否改变)
    bool OnExchangeItem( uint32 itemID );
    bool OnKillMonster(uint32 monsterID, int32 num, uint32 mapID);

    bool OnTriggerTaskTarget(uint32 arg1, uint32 arg2);
    ///////////////////////////////////////////////////////////////
    

    /*
     *	所有的任务目标触发都可以分为两类
     *  edge trigger(事件型)：只在发生某的事件的时候触发，依赖于事件逻辑的调用
     *  level trigger(状态型): 根据状态触发(达到某等级，充值达到某值), 不依赖于逻辑调用,只要状态符合条件,任何时候做状态检测都可以
     */
    bool IsLevelTriggerType();     // 检测任务目标类型，是否是状态触发型
private:
    void Clear();

    bool InternalTriggerTaskTarget(uint32 arg1, uint32 arg2, int32 arg3 = 0);       // 各目标的计数状态计算
    bool InternalLevelTriggerTaskTarget();          // 状态型目标触发

    void AddTargetValidNum(int32 num);      // 增加目标计数
    void SetTargetValidNum(int32 num);      // 设置目标计数
private:
    uint32              m_id;
    int32               m_num;              // 当前数量
    int32               m_max_num;          // 目标所需数量
    const TASK_TARGET*  m_target;

    RoleTask*       m_pTask;
    Player*         m_player;
    
    bool            m_IsChangeFlag;         // 标记目标状态是否发生改变
};

typedef std::list<RoleTaskTarget>               RoleTaskTargetList;

#endif // task_target_h__
