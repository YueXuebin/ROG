#include "robot_pch.h"
#include "case_lua.h"
#include "script.h"
#include "CLuaRobot.h"



CaseLua::CaseLua( Robot* pRobot ) :
CaseBase(pRobot)
{
    m_pLuaRobot = CnNew CLuaRobot(this);

}

CaseLua::~CaseLua()
{
    m_pLuaRobot->UnInit();
    CnDelete m_pLuaRobot;
}

void CaseLua::OnRecv( uint32 msgID, void* data )
{
    CaseBase::OnRecv(msgID, data);
}

void CaseLua::Update( uint32 tickcount, uint32 deltacount )
{
    CaseBase::Update(tickcount, deltacount);
}

void CaseLua::OnEnterGame()
{
    CaseBase::OnEnterGame();

    m_pLuaRobot->Init();
}

void CaseLua::OnActionStart( ActionBase* action )
{
    CaseBase::OnActionStart(action);
}

void CaseLua::OnActionEnd( ActionBase* action )
{
    CaseBase::OnActionEnd(action);
    m_pLuaRobot->OnActionEnd(action);
}

void CaseLua::OnAllActionEnd()
{
    CaseBase::OnAllActionEnd();
    m_pLuaRobot->OnALLActionEnd();
}
