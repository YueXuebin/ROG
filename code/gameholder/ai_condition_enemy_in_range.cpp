#include "gameholder_pch.h"
#include "ai_condition_enemy_in_range.h"
#include "ai_controller.h"
#include "battle_unit.h"
#include "scene.h"
#include "game_util.h"
#include "ai_group_register.h"
#include "ai_group.h"
#include "regular.h"


AIConditionEnemyInRange::AIConditionEnemyInRange() :
AIConditionBase(AI_CONDITION_ENEMY_IN_RANGE)
{
    m_Range = 0;
    m_type = 1;
    m_existence = true;
}

AIConditionEnemyInRange::~AIConditionEnemyInRange()
{

}

void AIConditionEnemyInRange::ParseXML( TiXmlElement* pDoc )
{
    AIConditionBase::ParseXML(pDoc);

    pDoc->QueryIntAttribute("range", &m_Range);
    pDoc->QueryIntAttribute("type", &m_type);
    
    int32 existence = 1;
    pDoc->QueryIntAttribute("inout",&existence);
    if (existence == 1)
    {
        m_existence = true;
    }
    else
    {
        m_existence = false;
    }
}

bool AIConditionEnemyInRange::IsSatisfy()
{
    BattleUnit* pOwner = m_pController->GetOwner();
    
    BattleUnit* pTarget = NULL;

    // 搜索范围内的敌人
    BattleUnit* uints[10];
    int32 ret = pOwner->GetScene()->FindBattleUnitInCircle(pOwner->GetPosition(), m_Range+pOwner->GetRadius(), uints, 10, pOwner->GetId());

    // 找最近的敌人
    real minDistance = 9999999.f;
    for(int32 i=0; i<ret; i++)
    {
        BattleUnit* pUnit = uints[i];
        if(pUnit == pOwner)
        {
            continue;
        }

        if (pUnit->IsDead())
        {
            continue;
        }

        real dis = DistanceBySceneObject(pOwner, pUnit);
        if(pOwner->CanAttack(pUnit) && m_type == 1)             // 敌人
        {
            if(dis < minDistance)
            {
                minDistance = dis;
                pTarget = pUnit;
            }
        }
        else if (pUnit->GetObjectType() == OBJ_PLAYER && m_type == 2 )                                 // 玩家
        {
            if(dis < minDistance)
            {
                minDistance = dis;
                pTarget = pUnit;
            }
        }
    }

    if((pTarget != NULL) == m_existence)
    {
        if(m_existence) // 进范围
        {
            m_pController->SetTargetUnitId(pTarget->GetId());
        }
        else        // 出范围
        {
            // 出范围满足也会设置最近目标
            real minRaduis = 100000;
            Scene::BattleUnitMap::iterator pBUnit = pOwner->GetScene()->GetBattleUnits().begin();
            for(; pBUnit != pOwner->GetScene()->GetBattleUnits().end(); ++pBUnit)
            {
                real unitRaduis = DistanceBySceneObject(pOwner, pBUnit->second);

                if(unitRaduis > minRaduis)
                    continue;

                if(pOwner->IsEnemy(pBUnit->second) && m_type == 1)
                {
                    m_pController->SetTargetUnitId(pBUnit->second->GetId());
                }
                else if (pBUnit->second->GetObjectType() == OBJ_PLAYER && m_type == 2 )      // 玩家
                {
                    m_pController->SetTargetUnitId(pBUnit->second->GetId());
                }
                else
                    continue;

                minRaduis = unitRaduis;
            }
        }

        return true;
    }
    else
    {
        // 设置群目标
        if(m_pController->GetAIGroup() && m_pController->GetAIGroup()->GetTargetUnitId())
        {
            m_pController->SetTargetUnitId( m_pController->GetAIGroup()->GetTargetUnitId() );
            return m_existence;
        }
    }

    return false;
}



