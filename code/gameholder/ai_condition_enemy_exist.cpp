#include "gameholder_pch.h"
#include "ai_condition_enemy_exist.h"
#include "ai_controller.h"
#include "battle_unit.h"
#include "scene.h"

AIConditionEnemyExist::AIConditionEnemyExist() :
AIConditionBase(AI_CONDITION_ENEMY_EXIST)
{
    m_radius = 0;
    m_number = 0;
    m_cond = 0;
}

AIConditionEnemyExist::~AIConditionEnemyExist()
{

}

void AIConditionEnemyExist::ParseXML( TiXmlElement* pDoc )
{
    AIConditionBase::ParseXML(pDoc);
    pDoc->QueryIntAttribute("radius",&m_radius);
    pDoc->QueryIntAttribute("number",&m_number);
    pDoc->QueryIntAttribute("cond",&m_cond);
}

bool AIConditionEnemyExist::IsSatisfy()
{
    BattleUnit* pOwner = m_pController->GetOwner();

    // ËÑË÷·¶Î§ÄÚµÄµÐÈË
    BattleUnit* uints[20];
    int32 ret = pOwner->GetScene()->FindBattleUnitInCircle(pOwner->GetPosition(), m_radius, uints, 20, pOwner->GetId());
    BattleUnit* pTarget = NULL;
    if (m_cond == 1 && ret <= m_number)  return true;
    if (m_cond == 2 && ret == m_number)  return true;
    if (m_cond == 3 && ret >= m_number)  return true;
    return false;
}

