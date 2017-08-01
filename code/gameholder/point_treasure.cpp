#include "gameholder_pch.h"
#include "point_treasure.h"
#include "scene.h"
#include "trigger.h"

CnImplementRTTI(PointTreasure, PointBase);

PointTreasure::PointTreasure(RegularBuilder* pRogRegular, uint32 CheckPointID) : 
PointBase(pRogRegular, CheckPointID, POINT_TYPE_TREASURE)
{
    m_showID = 1001;

    m_TriggerID = m_pRogRegular->GenerateValidTriggerID();
}

PointTreasure::~PointTreasure()
{

}

bool PointTreasure::Build()
{
    if(m_EventId == 0)
    {
        BuildTreasure();
    }
    
    return true;
}

void PointTreasure::OnEventTrigger()
{
    BuildTreasure();
}

void PointTreasure::BuildTreasure()
{
    TriggerData triggerData;

    triggerData.x = (int32)m_position.x;
    triggerData.y = (int32)m_position.y;
    triggerData.angle = m_angle;

    triggerData.triggerId = m_TriggerID;
    triggerData.name = "";
    triggerData.showId = m_showID;                          // 外形ID
    triggerData.radius = 100;                               // 触摸半径
    triggerData.isTouch = true;
    triggerData.funcId = TRIGGER_REGULAR_EVENT;             // 触发关卡逻辑
    triggerData.funcType = TRIGGER_TYPE_CLIENT_SERVER;

    m_pTrigger = m_pRogRegular->CreateTrigger(triggerData);
    CnAssert(m_pTrigger);
    m_pRogRegular->GetScene()->AddTrigger(m_pTrigger);
}

void PointTreasure::ReceiveEvent(uint32 frameCount, uint32 eventId, int32 arg1, int32 arg2, int32 arg3)
{
    PointBase::ReceiveEvent(frameCount, eventId , arg1 , arg2 ,arg3);

    if(eventId == TRIGGER_REGULAR_EVENT)
    {   // 玩家触发触发器
        int32 triggerID = arg1;
        if(triggerID == GetTriggerID() && !m_pTrigger->IsTriggered() && m_pTrigger->IsEnable())
        {
            // 触发逻辑
            int32 num = Crown::SDRandom(4, 8);

            std::vector<RandomDropData> data_list;
            for(int32 i=0; i<num; i++)
            {
                RandomDropData data;
                int32 type = Crown::SDRandom(2);
                switch(type)
                {
                case 0:     // 装备
                    {
                        data.firstType = ITEM_TYPE_EQUIP;
                        data.secondType = Crown::SDRandom(EQUIP_SUB_TYPE_1, EQUIP_SUB_TYPE_14+1);
                        data.quality = Crown::SDRandom(MAX_EQUIP_QUALITY);
                        break;
                    }
                case 1:     // 石头
                    {
                        data.firstType = ITEM_TYPE_STONE;
                        data.secondType = 0;
                        break;
                    }
                }

                data_list.push_back(data);
            }

            m_pRogRegular->DropItem(data_list, m_pTrigger->GetPosition());

            // 状态改变
            m_pTrigger->SetNetState(false, true);
        }
    }
}
