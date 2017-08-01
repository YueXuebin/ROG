#include "robot_pch.h"
#include "dance_start.h"


#include "move_action.h"
#include "robot.h"

DanceStart::DanceStart(Robot* pRobot) :
ActionBase(pRobot)
{
    m_Name = "Dance";
}

DanceStart::~DanceStart(void)
{
}

void DanceStart::OnRecv( uint32 msgID, void* data )
{
    switch(msgID)
    {
    //case MSG_DANCE_START_ACK:
    //    RecvDanceAck(data);
    //    break;
    }
}

void DanceStart::OnStart()
{
    //DANCE_STAR_REQ sendmsg;
    //sendmsg.dance_type = 1;
    //SendDanceReq(&sendmsg);
}

void DanceStart::SendDanceReq(void* data)
{
    //DANCE_STAR_REQ * sendmsg = (DANCE_STAR_REQ *) data;
    //m_Robot->Send(MSG_DANCE_START_REQ, sendmsg);
}

void DanceStart::RecvDanceAck(void * data)
{
    //DANCE_STAR_ACK* recvmsg = (DANCE_STAR_ACK*)data;
    //if(!recvmsg)
    //    return;
    //CnDbg("dance\n");
    //End();
}

