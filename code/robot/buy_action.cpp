#include "robot_pch.h"
#include "buy_action.h"
#include "robot.h"


Buy_Action::Buy_Action(Robot* pRobot,uint32 itemid,int32 num):
ActionBase(pRobot)
{
    m_Name = "BuyObject";
    m_curitemid = itemid;
    m_num = num;
}

Buy_Action::~Buy_Action(void)
{
}

void Buy_Action::OnStart()
{
    ActionBase::OnStart();
    MALL_BUY_REQ req;
    req.product_id = m_curitemid;
    req.num = m_num;
    m_Robot->Send(MSG_MALL_BUY_REQ,&req);
    End();
}

void Buy_Action::OnRecv(uint32 msgID, void* data)
{
    ActionBase::OnRecv(msgID, data);

    switch(msgID)
    {
        //¹ºÂòÎïÆ·
    case MSG_MALL_BUY_ACK:
        {
            MALL_BUY_ACK * pData = (MALL_BUY_ACK*)data;
            printf("¹ºÂòÁË item=%d num=%d\n", pData->product_info.product_id, pData->num);
        }
        break;

    }
}