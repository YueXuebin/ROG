#include "robot_pch.h"
#include "action_base.h"

uint32 g_ActionIDCounter = 1;   // Ψһid

ActionBase::ActionBase( Robot* pRobot ) :
m_Robot(pRobot), m_end(false)
{
    m_ActionId = g_ActionIDCounter++;

    m_MilliSecondTimer = 0;
}

ActionBase::~ActionBase()
{


}


void ActionBase::OnRecv( uint32 msgID, void* data )
{
    switch(msgID)
    {
    case MSG_ERROR_CODE:
        {
            ERROR_CODE* pErr = (ERROR_CODE*)data;
            printf("error code: %d\n", pErr->errcode);
            break;
        }
    }
}


void ActionBase::Update( uint32 tickcount, uint32 deltacount )
{
    m_MilliSecondTimer += deltacount;

}

void ActionBase::OnStart()
{

}

void ActionBase::OnEnd()
{

}


