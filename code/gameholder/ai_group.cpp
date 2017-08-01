#include "gameholder_pch.h"
#include "ai_group.h"
#include "ai_group_register.h"
#include "battle_unit.h"
#include "regular.h"
#include "scene.h"


AIGroup::AIGroup(AIGroupRegister* pManager, uint32 id) :
m_pManager(pManager),
m_id(id)
{

}

AIGroup::~AIGroup()
{
    m_AIControllers.clear();
}

void AIGroup::Update()
{
    if(m_TargetUnitId)
    {
        BattleUnit* pBattleUnit = m_pManager->GetRegular()->GetScene()->GetBattleUnit(m_TargetUnitId);
        if(!pBattleUnit)
        {
            m_TargetUnitId = 0;
            return;
        }

        if(pBattleUnit->IsDead() || pBattleUnit->isDestory())
        {
            m_TargetUnitId = 0;
            return;
        }
    }
}

void AIGroup::AddAIController(AIController* pAIController)
{
    if(!pAIController)
    {
        CnAssert(false);
        return;
    }

    CnAssert( !IsExist(pAIController) );

    m_AIControllers.push_back(pAIController);
}


void AIGroup::RemoveAIController(AIController* pAIController)
{
    CnAssert( IsExist(pAIController) );
    
    for(std::vector<AIController*>::iterator iter = m_AIControllers.begin(); iter != m_AIControllers.end(); iter++)
    {
        if(pAIController == *iter)
        {
            m_AIControllers.erase(iter);
            break;
        }
    }
}

bool AIGroup::IsExist(AIController* pAIController)
{
    for(std::vector<AIController*>::iterator iter = m_AIControllers.begin(); iter != m_AIControllers.end(); iter++)
    {
        if(pAIController == *iter)
            return true;
    }
    return false;
}


