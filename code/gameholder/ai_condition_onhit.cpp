#include "gameholder_pch.h"
#include "ai_condition_onhit.h"
#include "ai_controller.h"
#include "battle_unit.h"
#include "ai_group.h"


AIConditionOnHit::AIConditionOnHit() :
AIConditionBase(AI_CONDITION_ON_HIT)
{
    m_IsHit = false;
}

AIConditionOnHit::~AIConditionOnHit()
{

}

void AIConditionOnHit::onEnterState()
{
    AIConditionBase::onEnterState();
    m_IsHit = false;
}

bool AIConditionOnHit::IsSatisfy()
{
    bool ret = m_IsHit;
    m_IsHit = false;

    if(m_pController->GetAIGroup() && m_pController->GetAIGroup()->GetTargetUnitId())
    {
        m_pController->SetTargetUnitId( m_pController->GetAIGroup()->GetTargetUnitId() );
        return true;
    }

    return ret;
}

void AIConditionOnHit::onHit( BattleUnit* pAttacker )
{
    BattleUnit* pOwner = m_pController->GetOwner();

    m_IsHit = true;
    m_pController->SetTargetUnitId(pAttacker->GetId());
}

void AIConditionOnHit::ParseXML( TiXmlElement* pDoc )
{
    AIConditionBase::ParseXML(pDoc);
}


