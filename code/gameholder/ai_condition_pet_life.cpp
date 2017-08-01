#include "gameholder_pch.h"
#include "ai_condition_pet_life.h"
#include "battle_unit.h"
#include "ai_condition_base.h"
#include "pet_controller.h"
#include "ai_controller.h"

AIConditionPetLife::AIConditionPetLife() :
AIConditionBase(AI_CONDITION_BATTLE_PET_ALIVE)
{
    m_petLife = false;
    m_alive = false;
}

AIConditionPetLife::~AIConditionPetLife()
{

}

void AIConditionPetLife::ParseXML(TiXmlElement* pDoc)
{
    AIConditionBase::ParseXML(pDoc);

    uint32 alive = 0;
    pDoc->QueryUnsignedAttribute("", &alive);
    if(alive == 0 || alive == 1)
    {
        m_alive = false;
    }
    else
    {
        m_alive = true;
    }
}

void AIConditionPetLife::onEnterState()
{
    AIConditionBase::onEnterState();

    // 默认宠物是活着的
    BattleUnit* pOwer = m_pController->GetOwner();
    if(pOwer->GetPetController()->GetPetNumber() > 0)
    {
        m_petLife = true;
    }
    else 
    {
        m_petLife = false;
    }
}

bool AIConditionPetLife::IsSatisfy()
{
    if(m_petLife == m_alive)
    {
        return true;
    }
    return false;
}

void AIConditionPetLife::Update(int32 deltaTime)
{
    AIConditionBase::Update(deltaTime);

    BattleUnit* pOwer = m_pController->GetOwner();
    if(pOwer->GetPetController()->GetPetNumber() <= 0)
    {
        m_petLife = false;
    }
    else
    {
        m_petLife = true;
    }
}

