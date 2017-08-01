#include "gameholder_pch.h"
#include "skill_logic_base.h"
#include "skill_object.h"


SkillLogicBase::SkillLogicBase()
{
    
}

SkillLogicBase::~SkillLogicBase()
{

}

void SkillLogicBase::Init(SkillObject* pSkill)
{
    m_Active = false;
    m_Skill = pSkill;
    m_SkillData = pSkill->m_pSkillData;
    m_SkillArgs = &pSkill->m_SkillArgs;

    m_NodeId = m_Skill->GenerateNodeId();

}
