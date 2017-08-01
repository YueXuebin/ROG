#include "gameholder_pch.h"
#include "ai_condition_lack_mp.h"
#include "ai_controller.h"
#include "battle_unit.h"

AIConditionLackMP::AIConditionLackMP() : 
AIConditionBase(AI_CONDITION_LACK_MP)
{
    Reset();
}

AIConditionLackMP::~AIConditionLackMP()
{

}


void AIConditionLackMP::onEnterState()
{
    AIConditionBase::onEnterState();

    Reset();
}

void AIConditionLackMP::Reset()
{
    m_mp = 0;
    m_probability = 0;
    m_random = (int32( Crown::SDRandomFloat((real)100)) < m_probability) ? true : false;
}

void AIConditionLackMP::ParseXML( TiXmlElement* pDoc )
{
    AIConditionBase::ParseXML(pDoc);

    pDoc->QueryIntAttribute("percent",&m_mp);
    pDoc->QueryIntAttribute("pro",&m_probability);
    if (m_mp > 100) m_mp = 100;
    if (m_mp < 0) m_mp = 0;

}

bool AIConditionLackMP::IsSatisfy()
{
    BattleUnit* pOwner = m_pController->GetOwner();
    if (pOwner->GetMP() / pOwner->GetMaxMp() < m_mp / 100 )
    {
        if (m_random)
            return true;
    }
    return false;
}


