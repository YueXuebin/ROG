#include "gameholder_pch.h"
#include "ai_condition_in_skill_range.h"
#include "ai_controller.h"
#include "skill_manager.h"
#include "battle_unit.h"
#include "scene.h"
#include "game_util.h"
#include "skill_data.h"
#include "soldier.h"

AIConditionInSkillRange::AIConditionInSkillRange() :
AIConditionBase(AI_CONDITION_IN_SKILL_RANGE)
{
    m_interval = 0;
    m_count = 0;
    m_inout = 1;
    m_DefaultSkillId = 0;
}

AIConditionInSkillRange::~AIConditionInSkillRange()
{
}

void AIConditionInSkillRange::ParseXML( TiXmlElement* pDoc )
{
    AIConditionBase::ParseXML(pDoc);

    pDoc->QueryIntAttribute("skillid", &m_DefaultSkillId);
    if (m_DefaultSkillId < 0)
    {
        m_DefaultSkillId = 0;
    }

    int32 inout;
    pDoc->QueryIntAttribute("inout",&inout);
    if (inout == 2)
    {
        m_inout = false;
    }
    m_inout = true;

    pDoc->QueryIntAttribute("count", &m_count);
    pDoc->QueryIntAttribute("interval", &m_interval);
}

bool AIConditionInSkillRange::IsSatisfy()
{
    BattleUnit* pOwner = m_pController->GetOwner();

    uint32 skillId = m_pController->GetSelectSkill();
    if(skillId == 0)
    {
        skillId = m_DefaultSkillId;
    }

    if (!SkillManager::Instance()->HasSkill(skillId))
    {
        return false;
    }

    SkillData* pSkillData = SkillManager::Instance()->GetSkillData(skillId);
    if(!pSkillData)
    {
        CnFatal("ai skill %d not exist\n", skillId);
        return false;
    }

    uint32 targetId = m_pController->GetTargetUnitId();
    BattleUnit* pTarget = pOwner->GetScene()->GetBattleUnit(targetId);
    if(!pTarget)
    {
        return false;
    }

    if (pTarget->IsDead())
    {
        return false;
    }

    real dis = DistanceBySceneObject(pOwner, pTarget);
    
    if((dis < pSkillData->m_TargetData.m_Range) == m_inout)
    {
        return true;
    }

    return false;
}


