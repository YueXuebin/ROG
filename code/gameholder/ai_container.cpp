#include "gameholder_pch.h"
#include "ai_factory.h"
#include "ai_container.h"
#include "ai_state.h"
#include "ai_manager.h"
#include "ai_controller.h"

AIFactory   g_AIFactory;            // AI工厂


AIContainer::AIContainer(uint32 ClassType) : 
m_ClassType(ClassType)
{

}

AIContainer::~AIContainer()
{
    for(AIStateMap::iterator iter = m_AllStateMap.begin(); iter != m_AllStateMap.end(); iter++)
    {
        CnDelete iter->second;
    }
}

void AIContainer::SetAIState(uint32 state)
{
    m_AIStateId = state;
}

bool AIContainer::HasState(uint32 state)
{
    return m_AllStateMap.find(state) != m_AllStateMap.end();
}

AIState* AIContainer::GetState(uint32 state)
{
    AIStateMap::iterator iter = m_AllStateMap.find(state);
    if(iter == m_AllStateMap.end())
        return NULL;
    else
        return iter->second;
}

bool AIContainer::LoadAI(const std::string& aiName)
{
    GetController()->RecordAIName(aiName);          // 检查AI状态机不要自包含

    ClearAI();

    m_AIName = aiName;

    return g_AIFactory.LoadAIStates(aiName, this);
}

void AIContainer::ClearAI()
{
    for(AIStateMap::iterator iter = m_AllStateMap.begin(); iter != m_AllStateMap.end(); iter++)
    {
        CnDelete iter->second;
    }
    m_AllStateMap.clear();

    m_lastAIStateId = m_AIStateId = 0;
    m_pAIState = NULL;

    m_AIName.clear();
}

void AIContainer::Update(uint32 delta_time)
{
    if(!m_pAIState)
        return;

    // 切换AI状态
    if(m_AIStateId != m_pAIState->GetID())
    {
        if(HasState(m_AIStateId))
        {
            m_lastAIStateId = m_pAIState->GetID();

            m_pAIState->onLeaveState();

            m_pAIState = m_AllStateMap[m_AIStateId];

            m_pAIState->onEnterState();

            // ai debug
            AIManager::Instance()->OnAIChangeState(GetController()->GetOwner(), m_AIName, m_lastAIStateId, m_AIStateId);
        }
        else
        {
            m_AIStateId = m_lastAIStateId;      // 要切换的状态不存在，则状态回复到上一个状态
        }
    }

    // AI心跳
    if(m_pAIState)
        m_pAIState->Update(AI_UPDATE_TIME);
    else
        CnAssert(m_AIStateId == 0);
}

