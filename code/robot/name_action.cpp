#include "robot_pch.h"
#include "name_action.h"
#include "robot.h"


NameAction::NameAction( Robot* pRobot, const std::string& name ) :
ActionBase(pRobot)
{
    m_Name = "Name";
    m_NewName = name;
}

NameAction::~NameAction()
{

}

void NameAction::OnStart()
{
    ActionBase::OnStart();

    CHANGE_PLAYER_NAME_REQ req;
    req.name = m_NewName;
    m_Robot->Send(MSG_CHANGE_PLAYER_NAME_REQ, &req);
}

void NameAction::OnRecv( uint32 msgID, void* data )
{
    ActionBase::OnRecv(msgID, data);

    switch(msgID)
    {
    case MSG_CHANGE_PLAYER_NAME_ACK:
        {
            CHANGE_PLAYER_NAME_ACK* pData = (CHANGE_PLAYER_NAME_ACK*)data;
            if(pData->errcode != ERR_SUCCEED)
            {
                CnDbg("change name %s failed\n", m_NewName.c_str());
            }
            End();
        }
        break;
    }
}
