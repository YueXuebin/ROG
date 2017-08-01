#include "robot_pch.h"
#include "gm_action.h"
#include "robot.h"



GM_Action::GM_Action( Robot* pRobot, const std::string& command ) :
ActionBase(pRobot)
{
    m_command = command;
    m_Timer = 25;
}

GM_Action::~GM_Action( void )
{

}

void GM_Action::OnStart()
{
    ActionBase::OnStart();

    CHAT_MSG msg;
    msg.chat = m_command;
    msg.channel = CHAT_CHANNEL_GMCMD;
    m_Robot->Send(MSG_CHAT_REQ, &msg);

    CnDbg("CallGM:%s\n", m_command.c_str());
}

void GM_Action::Update( uint32 tickcount, uint32 deltacount )
{
    ActionBase::Update(tickcount, deltacount);

    m_Timer -= deltacount;
    if(m_Timer <= 0)
    {
        End();
    }

}