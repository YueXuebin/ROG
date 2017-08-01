#include "gameholder_pch.h"
#include "touch_controller.h"
#include "touch.h"
#include "battle_unit.h"
#include "state_define.h"
#include "scene.h"
#include "player.h"
#include "action_controller.h"

TouchController::TouchController(BattleUnit* owner) :
m_Owner(owner)
{
    m_touchID = 0;
}

TouchController::~TouchController()
{

}

void TouchController::TouchStart(uint32 touchID)
{
    uint16 err = ERR_FAILED;
    int32 remainingTime = 0;
    Touch* touch = NULL;
    TOUCH_START_ACK sendMsg;
    sendMsg.touch_id = touchID;

    // 采集点不存在
    touch = m_Owner->GetScene()->GetTouch(touchID);
    if(!touch)
    {
        err = ERR_TOUCH_NO_EXIST;
        goto _END;
    }

    // 采集状态转换是否可用
    if(!m_Owner->CanChangeTo(STATE_TOUCH))
    {
        err = ERR_FAILED;
        goto _END;
    }

    err = touch->TouchStart(m_Owner, remainingTime);
    if(err != ERR_SUCCEED)
    {
        goto _END;
    }

    // 进入采集状态
    if(!m_Owner->GetActionController()->Touch())
    {
        CnAssert(false);
        err = ERR_FAILED;
        goto _END;
    }

    m_touchID = touch->GetTouchID();

_END:
    if(m_Owner->GetObjectType() == OBJ_PLAYER)
    {
        sendMsg.err = err;
        sendMsg.remainingTime = remainingTime;
        ((Player*)m_Owner)->SendToGate(MSG_TOUCH_START_ACK, &sendMsg);
    }
}

void TouchController::TouchEnd()
{
    // 状态不对
    ActionController* actionController = m_Owner->GetActionController();
    CnAssert(actionController);
    actionController->Stop();

    m_touchID = 0;
}

void TouchController::OnTouchBreak()
{
    if(!m_Owner->GetScene())
        return;

    Touch* touch = m_Owner->GetScene()->GetTouch(m_touchID);
    if(!touch)
        return;

    touch->TouchBreak();

    //if(m_Owner->GetObjectType() == OBJ_PLAYER)
    //{
    //    TOUCH_BREAK_NTF sendMsg;
    //    sendMsg.touch_id = m_touchID;
    //    ((Player*)m_Owner)->SendToGate(MSG_TOUCH_BREAK_NTF, &sendMsg);
    //}

    m_touchID = 0;
}

void TouchController::OnStateChange(uint32 oldState, uint32 newState)
{
    if(oldState == STATE_TOUCH /*&& newState != STATE_TOUCH*/)
    {
        OnTouchBreak();
    }
}
