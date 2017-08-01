#include "robot_pch.h"
#include "touch_start_action.h"

#include "robot.h"


TouchStartAction::TouchStartAction(Robot* pRobot,int32 touchId):ActionBase(pRobot)
{
    m_Name = "TouchStart";
    m_touchId = touchId;
}

TouchStartAction::~TouchStartAction(void)
{

}

void TouchStartAction::OnStart()
{
    ActionBase::OnStart();

    TOUCH_START_REQ req;
    req.touch_id = m_touchId;
    m_Robot->Send(MSG_TOUCH_START_REQ,&req);
    End();
}

void  TouchStartAction::OnRecv(uint32 msgID, void* data)
{
    ActionBase::OnRecv(msgID, data);
    switch(msgID)
    {
    case MSG_TOUCH_START_ACK:
        {
            TOUCH_START_ACK* pData = (TOUCH_START_ACK*)data;
            if(pData->err ==  ERR_SUCCEED)
            {
                CnDbg(" 找到目标成功 timer=%d touchId=%d \n",pData->remainingTime,pData->touch_id);
                End();
            }
        }
        break;
    }
}

