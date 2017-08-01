#include "gameholder_pch.h"
#include "ai_condition_cd.h"
#include "ai_controller.h"
#include "battle_unit.h"
#include "scene.h"
#include "skill_manager.h"
#include "boss.h"
#include "monster.h"


AIConditionCD::AIConditionCD() :
AIConditionBase(AI_CONDITION_SKILL_IN_CD)
{
    m_skillid = -1;
    m_cd = 1;
}

AIConditionCD::~AIConditionCD()
{

}

void AIConditionCD::ParseXML( TiXmlElement* pDoc )
{
    AIConditionBase::ParseXML(pDoc);

    int32 skillId = 0;
    int32 skillPro = 0;

    BattleUnit* pOwner = m_pController->GetOwner();
    uint32 id = 0;
    std::vector<uint32> skill_list;
    if (pOwner->GetObjectType() == OBJ_BOSS)
    {
        id = ((Boss*)pOwner)->GetCreatureId();
        //skill_list = BossConfig::Instance()->GetBossTemplate(id)->skills;
    }
    if (pOwner->GetObjectType() == OBJ_MONSTER)
    {
        id = ((Monster*)pOwner)->GetCreatureId();
        //skill_list = MonsterConfig::Instance()->GetMonsterTemplate(id)->skills;
    }

    int32 skillCount = skill_list.size();

    // ¼¼ÄÜ
    skillId = skillPro = 0;
    pDoc->QueryIntAttribute("skillid", &skillId);

    if (skillId < 999 && skillId > 0)
    {
        if (skillId <= skillCount)
        {
            skillId = skill_list[skillId - 1];
        }
        else
        {
            skillId = 0;
            skillPro = 0;
        }
    }
    if(skillId > 0)
        m_skillid = skillId;

    int32 rtn = pDoc->QueryIntAttribute("cd",&m_cd);
    if (rtn)
        m_cd = 1;
}

bool AIConditionCD::IsSatisfy()
{
    BattleUnit* pOwner = m_pController->GetOwner();
    if ( m_skillid == -1 )
    {
        if (m_cd == 1)
        {
            if (pOwner->inCD(m_pController->GetSelectSkill()))
            {
                return true;
            }
            return false;
        }
        else if (m_cd == 2)
        {
            if (pOwner->inCD(m_pController->GetSelectSkill()))
            {
                return false;
            }
            return true;
        }
    }
    else
    {
        if (!SkillManager::Instance()->HasSkill(m_skillid))
            return false;
        if (m_cd == 1)
        {
            if( pOwner->inCD( m_skillid ) )
                return true;
            return false;
        }
        else if (m_cd == 2)
        {
            if (pOwner->inCD( m_skillid))
            {
                return false;
            }
            return true;
        }
    }
    return false;
}

