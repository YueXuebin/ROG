#include "gameholder_pch.h"
#include "ai_condition_master_attack.h"
#include "ai_controller.h"
#include "battle_unit.h"

AIConditionMasterAttack::AIConditionMasterAttack():
AIConditionBase(AI_CONDITION_MASTER_ATTACK)
{
    m_attack = false;
}

AIConditionMasterAttack::~AIConditionMasterAttack()
{

}

bool AIConditionMasterAttack::IsSatisfy()
{
    bool ret = m_attack;
    m_attack = false;
    return ret;
}

void AIConditionMasterAttack::onEnterState()
{
    AIConditionBase::onEnterState();
    m_attack = false;
}

void AIConditionMasterAttack::onMasterAttack( BattleUnit* pDefender )
{
    BattleUnit* pOwner = m_pController->GetOwner();

    m_attack = true;
    m_pController->SetTargetUnitId(pDefender->GetId());
}

