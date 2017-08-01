#include "gameholder_pch.h"
#include "point_base.h"
#include "scene.h"
#include "checkpoint.h"
#include "point_monster.h"
#include "point_treasure.h"
#include "point_barrel.h"
#include "point_trigger.h"
#include "point_touch.h"
#include "event_define.h"

CnImplementRootRTTI(PointBase);

PointBase::PointBase(RegularBuilder* pRogRegular, uint32 CheckPointID, uint32 type) :
m_pRogRegular(pRogRegular),
m_CheckPointID(CheckPointID),
m_type(type)
{
    m_pRogRegular->AddPoint(this);
    SetPostionByCheckPoint(m_CheckPointID);

    m_EventPointType = 0;
    m_EventId = 0;
    m_arg1 = 0;
    m_arg2 = 0;
}

PointBase::~PointBase()
{

}

void PointBase::SetPostionByCheckPoint(uint32 CheckPointID)
{
    Checkpoint* pCheckPoint = m_pRogRegular->GetScene()->GetCheckPoint(CheckPointID);
    if(pCheckPoint)
    {
        m_position = pCheckPoint->GetPosition();
        m_angle = pCheckPoint->GetAngle();
    }
    else
    {
        m_position.x = 0;
        m_position.y = 0;
        m_angle = 0;
        CnWarn("dungeon %s checkpoint %d not exist\n", m_pRogRegular->m_FileName.c_str(), CheckPointID);
    }
}


void PointBase::ReceiveEvent(uint32 frameCount, uint32 eventId, int32 arg1, int32 arg2, int32 arg3)
{
    if(eventId == m_EventId && arg1 == m_arg1 && arg2 == m_arg2)
    {
        // 触发一次，就给重置掉
        m_EventPointType = 0;
        m_EventId = 0;
        m_arg1 = 0;
        m_arg2 = 0;

        OnEventTrigger();
    }
}

void PointBase::SetEventTrigger(PointBase* triggerPoint, uint32 eventId, uint32 arg1, uint32 arg2)
{
    CnAssert(eventId);
    m_EventPointType = triggerPoint->GetType();
    m_EventId = eventId;
    m_arg1 = arg1;
    m_arg2 = arg2;
}

void PointBase::LinkTrigger(PointBase* pPointBase)
{
    // 防止配任务的时候，没有初始化触发对象
    if(!pPointBase)
    {
        CnAssert(false);
        return;
    }

    if(pPointBase->IsKindOf(&PointBarrel::ms_RTTI))
    {
        PointBarrel* pPointBarrel = CnDynamicCast(PointBarrel, pPointBase);
        if(!pPointBarrel)
        {
            CnAssert(false);
            return;
        }

        SetEventTrigger(pPointBase, SCENE_EVENT_SPAWN_ALL_DEAD, pPointBarrel->GetSpawn()->GetSpawnId(), 0);
    }
    else if(pPointBase->IsKindOf(&PointMonster::ms_RTTI))
    {
        PointMonster* pPointMonster = CnDynamicCast(PointMonster, pPointBase);
        if(!pPointMonster)
        {
            CnAssert(false);
            return;
        }

        SetEventTrigger(pPointBase, SCENE_EVENT_SPAWN_ALL_DEAD, pPointMonster->GetSpawnID() ,0);
    }
    else if(pPointBase->IsKindOf(&PointTreasure::ms_RTTI))
    {
        PointTreasure* pPointTreasure = CnDynamicCast(PointTreasure, pPointBase);
        if(!pPointTreasure)
        {
            CnAssert(false);
            return;
        }

        SetEventTrigger(pPointBase, TRIGGER_REGULAR_EVENT, pPointTreasure->GetTriggerID(), 0);
    }
    else if(pPointBase->IsKindOf(&PointTrigger::ms_RTTI))
    {
        PointTrigger* pPointTrigger = CnDynamicCast(PointTrigger, pPointBase);
        if(!pPointTrigger)
        {
            CnAssert(false);
            return;
        }

        SetEventTrigger(pPointBase, TRIGGER_REGULAR_EVENT, pPointTrigger->GetTriggerID(), 0);
    }
    else if(pPointBase->IsKindOf(&PointTouch::ms_RTTI))
    {

    }
}

