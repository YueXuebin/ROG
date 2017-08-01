#include "gameholder_pch.h"
#include "point_trigger.h"
#include "scene.h"
#include "trigger.h"


CnImplementRTTI(PointTrigger, PointBase);

PointTrigger::PointTrigger(RegularBuilder* pRogRegular, uint32 CheckPointID) : 
PointBase(pRogRegular, CheckPointID, POINT_TYPE_TRIGGER)
{
    m_showID = 1001;

    m_TriggerID = m_pRogRegular->GenerateValidTriggerID();
}

PointTrigger::~PointTrigger()
{

}

bool PointTrigger::Build()
{
    if(m_EventId == 0)
    {
        BuildTrigger();
    }

    return true;
}

void PointTrigger::OnEventTrigger()
{
    BuildTrigger();
}

void PointTrigger::BuildTrigger()
{
    TriggerData triggerData;

    triggerData.x = (int32)m_position.x;
    triggerData.y = (int32)m_position.y;
    triggerData.angle = m_angle;

    triggerData.triggerId = m_TriggerID;
    triggerData.name = "";
    triggerData.showId = m_showID;                      // 外形ID
    triggerData.radius = 100;                            // 触摸半径
    triggerData.isTouch = true;
    triggerData.funcId = TRIGGER_REGULAR_EVENT;         // 触发关卡逻辑
    triggerData.funcType = TRIGGER_TYPE_CLIENT_SERVER;

    m_pTrigger = m_pRogRegular->CreateTrigger(triggerData);
    CnAssert(m_pTrigger);
    m_pRogRegular->GetScene()->AddTrigger(m_pTrigger);
}

void PointTrigger::ReceiveEvent(uint32 frameCount, uint32 eventId, int32 arg1, int32 arg2, int32 arg3)
{
    PointBase::ReceiveEvent(frameCount, eventId, arg1, arg2, arg3);

    if(eventId == TRIGGER_REGULAR_EVENT)
    {   // 玩家触发触发器
        int32 triggerID = arg1;
        if(triggerID == GetTriggerID() && !m_pTrigger->IsTriggered() && m_pTrigger->IsEnable())
        {
            // 触发逻辑


            // 状态改变
            m_pTrigger->SetNetState(false, true);
        }
    }
}
