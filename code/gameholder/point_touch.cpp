#include "gameholder_pch.h"
#include "point_touch.h"
#include "scene.h"
#include "event_define.h"
#include "touch.h"

CnImplementRTTI(PointTouch, PointBase);

PointTouch::PointTouch(RegularBuilder* pRogRegular, uint32 CheckPointID) :
PointBase(pRogRegular, CheckPointID, POINT_TYPE_TOUCH)
{
    m_TouchData.x = (int32)m_position.x;
    m_TouchData.y = (int32)m_position.y;
    m_TouchData.angle = m_angle;
    m_TouchData.radius = 100;

    m_TouchData.touchId = m_pRogRegular->GenerateValidTriggerID();
    m_TouchData.showId = 1001;
    m_TouchData.time = 3000;
}

PointTouch::~PointTouch()
{

}

bool PointTouch::Build()
{
    if (m_EventId != 0) 
    {
        return true;
    }

    // 创建采集点
    m_pTouch = m_pRogRegular->CreateTouch(m_TouchData);
    if(!m_pTouch)
    {
        CnAssert(false);
        return false;
    }
    m_pRogRegular->GetScene()->AddTouch(m_pTouch);

    return true;
}

void PointTouch::ReceiveEvent(uint32 frameCount, uint32 eventId, int32 arg1, int32 arg2, int32 arg3)
{
    PointBase::ReceiveEvent(frameCount, eventId, arg1, arg2, arg3);

    if(eventId == EVENT_TOUCH_ACTIVATE)
    {   // 玩家触发触发器
        uint32 battleUnitId = arg1;
        uint32 touchID = arg2;
        
        if(m_TouchData.touchId == touchID)
        {
            Scene* pScene = m_pRogRegular->GetScene();
            Touch* pTouch = pScene->GetTouch(m_TouchData.touchId);
            pTouch->SetDestory();
        }
    }
}

void PointTouch::SetTouchTime(uint32 time)
{
    m_TouchData.time = time;
}

void PointTouch::SetShowID(uint32 showId)
{
    m_TouchData.showId = showId;
}

void PointTouch::OnEventTrigger()
{
    // 创建采集点
    m_pTouch = m_pRogRegular->CreateTouch(m_TouchData);
    if(!m_pTouch)
    {
        CnAssert(false);
    }

    m_pRogRegular->GetScene()->AddTouch(m_pTouch);
}