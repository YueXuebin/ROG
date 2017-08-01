#include "robot_pch.h"
#include "online_reward_action.h"
#include "robot.h"

OnlineRewardAction::OnlineRewardAction( Robot* pRobot ) : 
ActionBase(pRobot)
{
    m_Name = "OnlineRewardAction";
}

OnlineRewardAction::~OnlineRewardAction()
{

}

void OnlineRewardAction::OnStart()
{
    ActionBase::OnStart();

    NONE none;
    m_Robot->Send(MSG_ONLINE_REWARD_REQ, &none);

    End();

}

void OnlineRewardAction::OnRecv( uint32 msgID, void* data )
{
    ActionBase::OnRecv(msgID, data);

    switch(msgID)
    {
        case MSG_ONLINE_REWARD_ACK:
        {
            ONLINE_REWARD_ACK* ack = (ONLINE_REWARD_ACK*)data;
            if(ack->errcode == ERR_SUCCEED)
            {
                printf("领取在线奖励成功!\n");
            }
            else
            {
                printf("领取在线奖励失败!\n");
            }

            break;
        }



    }
}
