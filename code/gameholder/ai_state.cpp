#include "gameholder_pch.h"
#include "ai_state.h"
#include "ai_action_base.h"
#include "ai_condition_base.h"
#include "ai_controller.h"
#include "performance_analysis.h"


AIState::AIState( uint32 id, AIContainer* pContainer ) :
m_id(id),
m_pAction(NULL),
m_pController(NULL),
m_pContainer(pContainer),
m_PassTime(0)
{

}

AIState::~AIState()
{
    if (m_pAction)
    {
        CnDelete m_pAction;
    }

    for(ConditionList::iterator iter = m_AllCondition.begin(); iter != m_AllCondition.end(); iter++)
    {
        CnDelete (*iter);
    }
}

void AIState::onEnterState()
{
    m_PassTime = 0;

    if(m_pAction)
    {
        m_pAction->onEnterState();
    }

    for(ConditionList::iterator iter = m_AllCondition.begin(); iter != m_AllCondition.end(); iter++)
    {
        AIConditionBase* pCondition = *iter;
        pCondition->onEnterState();
    }
}

void AIState::onLeaveState()
{
    for(ConditionList::iterator iter = m_AllCondition.begin(); iter != m_AllCondition.end(); iter++)
    {
        AIConditionBase* pCondition = *iter;
        pCondition->onLeaveState();
    }
    if (m_pAction)
    {
        m_pAction->onLeaveState();
    }
}

void AIState::onHit(BattleUnit* pAttacker)
{
    for(ConditionList::iterator iter = m_AllCondition.begin(); iter != m_AllCondition.end(); iter++)
    {
        AIConditionBase* pCondition = *iter;
        pCondition->onHit(pAttacker);
    }
}

void AIState::onMasterHit( BattleUnit* pAttacker )
{
    for(ConditionList::iterator iter = m_AllCondition.begin(); iter != m_AllCondition.end(); iter++)
    {
        AIConditionBase* pCondition = *iter;
        pCondition->onMasterHit(pAttacker);
    }
}

void AIState::onMasterAttack( BattleUnit* pDefender )
{
    for(ConditionList::iterator iter = m_AllCondition.begin(); iter != m_AllCondition.end(); iter++)
    {
        AIConditionBase* pCondition = *iter;
        pCondition->onMasterAttack(pDefender);
    }
}

void AIState::Update(int32 deltaTime)
{
    if(m_pAction)
    {
        // 行为
        PERFORM_NAME_BEGIN("m_pAction::Update");
        m_pAction->Update(deltaTime);
        PERFORM_NAME_END("m_pAction::Update");
    }

    PERFORM_NAME_BEGIN("Condition::Update");
    // 条件更新
    for(ConditionList::iterator iter = m_AllCondition.begin(); iter != m_AllCondition.end(); iter++)
    {
        AIConditionBase* pCondition = *iter;
        pCondition->Update(deltaTime);
    }

    // 条件检测
    for(ConditionList::iterator iter = m_AllCondition.begin(); iter != m_AllCondition.end(); iter++)
    {
        AIConditionBase* pCondition = *iter;
        if(pCondition->IntervalAndCount())
        {
            if(pCondition->IsSatisfy())
            {
                uint32 nextId = pCondition->GetNextStateId();
                if(nextId == 0)
                    CnFatal("state %d change to state 0\n", m_id);
                else
                    m_pContainer->SetAIState(nextId);
                break;
            }
        }
    }

    m_PassTime += deltaTime;
    PERFORM_NAME_END("Condition::Update");
}



