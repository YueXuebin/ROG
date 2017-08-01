#include "robot_pch.h"
#include "friend_action.h"
#include "robot.h"
Friend_Action::Friend_Action(Robot* pRobot,int32 _name,int32 type,uint32 player_id_h):
ActionBase(pRobot),m_type(type),m_player_id_h(player_id_h)
{
    m_Name = "DoFriend";
    char buff[1024];
    m_name = Crown::SDItoa(buff, 1024, _name);
}

Friend_Action::~Friend_Action(void)
{
}

void Friend_Action::OnStart()
{
    ActionBase::OnStart();

    if(m_type == FRIEND_ADD)
    {
        FRIEND_ADD_REQ req;
        req.dst_name = m_name;
        m_Robot->Send(MSG_FRIEND_ADD_REQ, &req);
    }
    else if(m_type == FRIEND_DELETE)
    {
        FRIEND_DEL_REQ req;
        req.player_id_h = m_player_id_h;
        m_Robot->Send(MSG_FRIEND_DEL_REQ, &req);
    }
    else if(m_type == FRIEND_ADD_ANS )
    {
        FRIEND_ADD_ACK req;
        req.dst_name = m_name;
        m_Robot->Send(MSG_FRIEND_ADD_ANS, &req);
    }

}
void Friend_Action::OnRecv(uint32 msgID, void* data)
{
    ActionBase::OnRecv(msgID, data);

    switch(msgID)
    {
        //Ìí¼ÓºÃÓÑÓ¦´ð
    case MSG_FRIEND_ADD_ACK:
        {
            FRIEND_ADD_ACK* pData =(FRIEND_ADD_ACK*)data;
            std::string anme = pData->dst_name;
        }
        break;
    }
}
