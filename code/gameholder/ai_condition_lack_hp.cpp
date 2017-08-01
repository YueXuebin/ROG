#include "gameholder_pch.h"
#include "ai_condition_lack_hp.h"
#include "ai_controller.h"
#include "battle_unit.h"

AIConditionLackHP::AIConditionLackHP() : 
AIConditionBase(AI_CONDITION_LACK_HP)
{
    Reset();
}

AIConditionLackHP::~AIConditionLackHP()
{
    
}


void AIConditionLackHP::onEnterState()
{
    AIConditionBase::onEnterState();

    Reset();
}

void AIConditionLackHP::Reset()
{
    m_hp = 0;
    m_probability = 0;
    m_random = (int32( Crown::SDRandomFloat((real)100)) < m_probability) ? true : false;
}

void AIConditionLackHP::ParseXML( TiXmlElement* pDoc )
{
    AIConditionBase::ParseXML(pDoc);

    pDoc->QueryIntAttribute("percent",&m_hp);
    pDoc->QueryIntAttribute("pro",&m_probability);
    if (m_hp > 100) m_hp = 100;
    if (m_hp < 0) m_hp = 0;

}

bool AIConditionLackHP::IsSatisfy()
{
    BattleUnit* pOwner = m_pController->GetOwner();
    if (pOwner->GetHP() / pOwner->GetMaxHp() < m_hp / 100 )
    {
        if (m_random)
            return true;
    }
    return false;
}


