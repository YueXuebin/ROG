#include "gameholder_pch.h"
#include "touch.h"
#include "battle_unit.h"
#include "scene.h"
#include "touch_controller.h"
#include "event_define.h"

CnImplementRTTI(Touch, SceneObject);

Touch::Touch() :
SceneObject(OBJ_TOUCH)
{
    m_touchID = 0;              // 采集点ID
    m_time = 0;                 // 采集时间

    m_ToucherID = 0;            // 采集者单位ID
    m_touchingTime = 0;         // 采集结束时间点
    m_state = kInit;            // 状态
}

Touch::~Touch()
{

}

void Touch::Update(uint32 frame_count)
{
    if(m_state == kStart)
    {
        m_touchingTime += g_ServerUpdateTime;

        if(m_touchingTime > m_time)
        {
            // 采集成功处理
            TouchSuccess();
        }
    }
}

int16 Touch::TouchStart(BattleUnit* battleUnit, int32& remainingTime)
{
    if(!battleUnit)
    {
        return ERR_FAILED;
    }

    if(m_state != kInit)
    {
        // 采集状态不可能没有采集对象
        if(m_state == kStart)
            CnAssert(m_ToucherID != 0);
        else if(m_state == kEnd)
            CnAssert(m_ToucherID == 0);
        else
            CnAssert(false);
        return ERR_TOUCH_START;
    }

    // 超出范围无法采集(距离可能有误差)
    if((battleUnit->GetPosition() - GetPosition()).Length() > (GetRadius() + battleUnit->GetRadius() + 200) )
    {
        return ERR_TOUCH_OUT_RADIUS;
    }

    if(m_ToucherID != 0 || m_ToucherID == battleUnit->GetId())
    {
        return ERR_TOUCH_START;
    }

    m_state = kStart;
    m_touchingTime = 0;
    remainingTime = m_time / 1000;
    m_ToucherID = battleUnit->GetId();

    return ERR_SUCCEED;
}

void Touch::TouchSuccess()
{
    CnAssert(m_state == kStart);

    BattleUnit* battleUnit = GetScene()->GetBattleUnit(m_ToucherID);
    if(!battleUnit)
    {
        return;
    }

    battleUnit->GetTouchController()->TouchEnd();

    // 通知场景玩家采集成功
    TOUCH_END_NTF sendMsg;
    sendMsg.touch_id = m_touchID;
    sendMsg.arg1 = 0;
    GetScene()->NotifyMessageToAll(MSG_TOUCH_END_NTF, &sendMsg);

    m_state = kEnd;
    m_ToucherID = 0;
    m_touchingTime = 0;

    GetScene()->SendEvent(0, EVENT_TOUCH_ACTIVATE, battleUnit->GetId(), m_touchID, 0);
}

void Touch::TouchBreak()
{
    if(m_state != kStart)
    {
        return;
    }

    BattleUnit* battleUnit = GetScene()->GetBattleUnit(m_ToucherID);
    if(!battleUnit)
    {
        return;
    }

    battleUnit->GetTouchController()->TouchEnd();

    // 通知场景玩家采集中断
    TOUCH_END_NTF sendMsg;
    sendMsg.touch_id = m_touchID;
    sendMsg.arg1 = 1;
    GetScene()->NotifyMessageToAll(MSG_TOUCH_END_NTF, &sendMsg);

    m_state = kInit;
    m_touchingTime = 0;
    m_ToucherID = 0;
}
