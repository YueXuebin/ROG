#include "robot_pch.h"
#include "move_action.h"
#include "robot.h"


MoveAction::MoveAction(Robot* pRobot, int32 x, int32 y) :
ActionBase(pRobot)
{
    m_Name = "Move";

    m_desX = x;
    m_desY = y;

}

MoveAction::~MoveAction()
{

}

void MoveAction::OnStart()
{
    // 计算距离和时间
    m_srcX = m_Robot->GetPropInfo().x;
    m_srcY = m_Robot->GetPropInfo().y;
    //m_srcX = 0;
    //m_srcY = 0;
    int32 dis = (int32)sqrtf((real)(m_desX - m_srcX) * (m_desX - m_srcX) + (m_desY - m_srcY) * (m_desY - m_srcY));
    int32 speed = 600;
    m_Time = dis * 1000 / speed;

    move();
}

void MoveAction::OnRecv( uint32 msgID, void* data )
{
    switch(msgID)
    {
    case MSG_MOVE_ACK:
        RecvMoveAck(data);
        break;
    }
}

void MoveAction::Update( uint32 tickcount, uint32 deltacount )
{
    ActionBase::Update(tickcount, deltacount);

    if(m_MilliSecondTimer > m_Time)
    {
        stop();
        End();
    }
}

void MoveAction::stop()
{
    // 停止消息
    MOVE_REQ sendmsg;
    sendmsg.is_moving = false;
    sendmsg.pos_x = m_desX;
    sendmsg.pos_y = m_desY;
    sendmsg.angle = (int32)( Crown::Angle((real)m_srcX, (real)m_srcY, (real)m_desX, (real)m_desY) * 180 / M_PI);

    SendMoveReq(&sendmsg);

    m_Robot->GetPropInfo().x = m_desX;
    m_Robot->GetPropInfo().y = m_desY;
}

void MoveAction::move()
{
    // 移动消息
    MOVE_REQ sendmsg;
    sendmsg.is_moving = true;
    sendmsg.pos_x = m_srcX;
    sendmsg.pos_y = m_srcY;
    sendmsg.angle = (int32)( Crown::Angle((real)m_srcX, (real)m_srcY, (real)m_desX, (real)m_desY) * 180 / M_PI);

    SendMoveReq(&sendmsg);
}

void MoveAction::SendMoveReq(void* data)
{
    MOVE_REQ* sendmsg = (MOVE_REQ*) data;

    //CnInfo("%s will move %d,%d\n",this->GetAccount().c_str(),m_prop.x,m_prop.y);
    m_Robot->Send(MSG_MOVE_REQ, sendmsg);
}

void MoveAction::RecvMoveAck(void* data)
{
    MOVE_ACK* recvmsg = (MOVE_ACK*)data;
    if(!recvmsg) 
        return;

    m_desX = recvmsg->x;
    m_desY = recvmsg->y;

    //m_Robot->GetPropInfo().x = recvmsg->x;
    //m_Robot->GetPropInfo().y = recvmsg->y;



    //CnInfo("%s move %d,%d\n", m_Robot->GetAccount().c_str(), m_Robot->GetPropInfo().x, m_Robot->GetPropInfo().y);
}

