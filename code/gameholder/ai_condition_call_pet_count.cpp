#include "gameholder_pch.h"
#include "ai_condition_call_pet_count.h"
#include "ai_controller.h"
#include "battle_unit.h"
#include "pet_controller.h"


AIConditionCallPetCount::AIConditionCallPetCount() :
AIConditionBase(AI_CONDITION_SUMMON_PET_NUM)
{
    m_number = 0;
    m_cond = 1;
}

AIConditionCallPetCount::~AIConditionCallPetCount()
{

}

void AIConditionCallPetCount::ParseXML( TiXmlElement* pDoc )
{
    AIConditionBase::ParseXML(pDoc);

    pDoc->QueryUnsignedAttribute("number",&m_number);
    pDoc->QueryIntAttribute("cond", &m_cond);
}

bool AIConditionCallPetCount::IsSatisfy()
{
    BattleUnit* pOwer = m_pController->GetOwner();
    uint32 number = pOwer->GetPetController()->GetPetNumber();
    if (number < 0)
    {
        return false;
    }
    // 数量小于
    if ( m_cond == 1 )
    {
        if ( number <= m_number)
        {
            return true;
        }
        return false;
    }
    // 数量等于
    if ( m_cond == 2 )
    {
        if ( number == m_number )
        {
            return true;
        }
        return false;
    }
    // 数量大于
    if ( m_cond == 3 )
    {
        if ( number >= m_number)
        {
            return true;
        }
        return false;
    }
    return false;
}

