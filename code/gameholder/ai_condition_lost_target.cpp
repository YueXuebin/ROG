#include "gameholder_pch.h"
#include "ai_condition_lost_target.h"
#include "ai_controller.h"
#include "battle_unit.h"
#include "scene.h"
#include "battle_unit.h"

AIConditionLostTarget::AIConditionLostTarget() : 
AIConditionBase(AI_CONDITION_LOST_TARGET)
{
    m_range = 0;
}

AIConditionLostTarget::~AIConditionLostTarget()
{

}

void AIConditionLostTarget::ParseXML( TiXmlElement* pDoc )
{
    AIConditionBase::ParseXML(pDoc);

    pDoc->QueryIntAttribute("radius",&m_range);
}

bool AIConditionLostTarget::IsSatisfy()
{
    BattleUnit* pOwner = m_pController->GetOwner();
    Scene::BattleUnitMap uints;
    uints.clear();

    if(m_pController->GetTargetUnitId() == 0)
    {
        return false;
    }

    BattleUnit* pTarget =  pOwner->GetScene()->GetBattleUnit(m_pController->GetTargetUnitId());
    if (!pTarget)
    {
        m_pController->SetTargetUnitId(0);
        return true;
    }

    // 范围内查找这个目标
    int32 ret = pOwner->GetScene()->FindBattleUnitInCircle(pOwner->GetPosition(), m_range+pOwner->GetRadius(), uints, pOwner->GetId());
    Scene::BattleUnitMap::iterator battleIt = uints.begin();
    for (; battleIt != uints.end(); battleIt++)
    {
        // 死亡也当成目标丢失
        if(battleIt->second->IsDead())
        {
            continue;
        }
        if (battleIt->first == pTarget->GetId())
        {
            break;
        }
    }

    if(battleIt == uints.end())
    {
        m_pController->SetTargetUnitId(0);
        return true;
    }

    return false;
}

