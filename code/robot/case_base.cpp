#include "robot_pch.h"
#include "case_base.h"
#include "action_base.h"
#include "robot.h"


CaseBase::CaseBase(Robot* pRobot) :
m_Robot(pRobot)
{
    m_CurrentAction = NULL;
    m_AllEnd = false;
}

CaseBase::~CaseBase()
{
    for(std::vector<ActionBase*>::iterator iter = m_ActionList.begin(); iter != m_ActionList.end(); iter++)
    {
        ActionBase* pAction = *iter;
        CnDelete pAction;
    }

    if(m_CurrentAction)
        CnDelete m_CurrentAction;
}

void CaseBase::PushAction( ActionBase* action )
{
    CnAssert(action);
    m_ActionList.push_back(action);
   int mm =  m_ActionList.size();
}

void CaseBase::Update( uint32 tickcount, uint32 deltacount )
{
    if(m_CurrentAction)
    {
        if(m_CurrentAction->IsEnd())
        {
            OnActionEnd(m_CurrentAction);
            CnDelete m_CurrentAction;
            m_CurrentAction = NULL;
        }
        else
        {
            m_CurrentAction->Update(tickcount, deltacount);
        }

    }

    if(!m_CurrentAction && m_ActionList.size() > 0)
    {
        m_CurrentAction = m_ActionList[0];
        m_ActionList.erase(m_ActionList.begin());
        OnActionStart(m_CurrentAction);
    }
    else if(m_ActionList.size() == 0 && !m_AllEnd)
    {
        OnAllActionEnd();
        m_AllEnd = true;
    }
}

void CaseBase::OnRecv( uint32 msgID, void* data )
{
    if(m_CurrentAction)
        m_CurrentAction->OnRecv(msgID, data);
}

void CaseBase::OnEnterGame()
{

}

void CaseBase::OnActionStart( ActionBase* action )
{
    //CnDbg("action %s start\n", action->name());
    action->OnStart();
}

void CaseBase::OnActionEnd(ActionBase* action)
{
    //CnDbg("action %s end\n", action->name());
    action->OnEnd();
}

void CaseBase::OnAllActionEnd()
{

}




