#include "gameholder_pch.h"
#include "ai_condition_master_onhit.h"
#include "ai_controller.h"
#include "battle_unit.h"

AIConditionMasterOnhit::AIConditionMasterOnhit() :
AIConditionBase(AI_CONDITION_MASTER_ON_HIT)
{
    m_IsHit = false;
}

AIConditionMasterOnhit::~AIConditionMasterOnhit()
{

}


bool AIConditionMasterOnhit::IsSatisfy()
{
    bool ret = m_IsHit;
    m_IsHit = false;
    return ret;
}

void AIConditionMasterOnhit::onEnterState()
{
    AIConditionBase::onEnterState();
    m_IsHit = false;
}

void AIConditionMasterOnhit::onMasterHit( BattleUnit* pAttacker )
{
    BattleUnit* pOwner = m_pController->GetOwner();

    m_IsHit = true;
    m_pController->SetTargetUnitId(pAttacker->GetId());
}

