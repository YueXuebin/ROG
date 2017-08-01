/*
 *	一个技能中的状态时间节点
 */
#ifndef skill_time_node_h__
#define skill_time_node_h__

#include "skill_logic_base.h"
#include "skill_data.h"

class SkillObject;

const uint32 MAX_ATTACK_NODE = 3;           // 最多5个攻击逻辑

class SkillLogicCast : public SkillLogicBase
{
public:
    SkillLogicCast();
    virtual ~SkillLogicCast();
    virtual uint32  GetLogicType() {return SKILL_LOGIC_CAST;}

    virtual void Init(SkillObject* pSkill);

    void Clear();

    void BreakOff();                    // 打断施法

    virtual void Update();
    virtual void OnStateChange( uint32 state );


    void SetTime(int32 time) {m_DurTime = time;}
    int32 GetTime() {return m_DurTime;}

   
    bool    m_IsFirst;      // 标记第一个cast节点
    bool    m_IsLast;       // 标记最后一个cast节点

    int32   m_StartTime;

private:
    void UpdateLogic();

    void CreateShoot(uint32 shootId);

    void MoveMe(const SkillMoveData* pMoveData);

    bool CanCast();

    void OnEnter();
    void OnLeave();

private:
    const SkillCastData*      m_CastData;

    int32   m_LaunchStartTime;  // 发射逻辑的触发时间

    int32   m_DurTime;          // 总持续时长(ms)
    int32   m_PassTime;         // 已经经过的时间(ms)
    
    
    bool    m_InLogic;          // 是否进入到施法逻辑中

    SkillLogicLaunch*   m_LaunchLogic;      // 发射逻辑

    

    

};



#endif // skill_time_node_h__
