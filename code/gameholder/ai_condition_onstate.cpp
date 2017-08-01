#include "gameholder_pch.h"
#include "ai_condition_onstate.h"
#include "ai_controller.h"
#include "battle_unit.h"
#include "ai_state.h"


AIConditionOnState::AIConditionOnState() :
AIConditionBase(AI_CONDITION_ON_STATE)
{
    m_InStateId = 0;
}

AIConditionOnState::~AIConditionOnState()
{

}

void AIConditionOnState::ParseXML( TiXmlElement* pDoc )
{
    AIConditionBase::ParseXML(pDoc);

    pDoc->QueryUnsignedAttribute("state", &m_InStateId);
}

bool AIConditionOnState::IsSatisfy()
{
    if(GetAIState()->GetPassTime() == 0)
        return false;       // 状态判断等到下一个心跳才起作用

    BattleUnit* pOwner = m_pController->GetOwner();
    return pOwner->GetState() == m_InStateId;
}


