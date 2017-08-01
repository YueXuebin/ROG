#ifndef skill_node_h__
#define skill_node_h__

#include "skill_data.h"
#include "skill_object.h"

/*
 *	技能的逻辑节点基类
 */
class SkillLogicBase
{
public:
    SkillLogicBase();
    virtual ~SkillLogicBase();

    virtual void Init(SkillObject* pSkill);

    virtual uint32  GetLogicType() = 0;

    uint32  GetNodeId() {return m_NodeId;}

    virtual void Update() = 0;      // 逻辑节点的执行
    virtual void OnStateChange( uint32 state ) = 0;     // 主人状态发生改变
    
    virtual void Active() {m_Active = true;}        // 激活执行
    virtual void End() {m_Active = false;}          // 结束执行
    bool IsActive() {return m_Active;}      // 逻辑节点是否执行


    SkillObject*    GetSkill() {return m_Skill;}
    const SkillData*      GetSkillData() {return m_SkillData;}
    const SkillArgumentData&    GetSkillArgument() {return m_Skill->GetSkillArgument();}

protected:
    SkillObject*     m_Skill;
    const SkillData*    m_SkillData;
    const SkillArgumentData*    m_SkillArgs;

    bool            m_Active;       // 是否激活执行
    uint32          m_NodeId;       // 该逻辑节点的id(在技能对象内唯一)
};


#endif // skill_node_h__
