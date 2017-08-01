#include "gameholder_pch.h"
#include "regular.h"
#include "regular_event.h"
#include "regular_action.h"
#include "regular_event_player_dead.h"
#include "regular_event_offline_player_dead.h"
#include "regular_event_monster_dead.h"
#include "regular_event_monster_clear.h"
#include "regular_event_timer.h"
#include "regular_event_end_time.h"
#include "regular_event_start_time.h"
#include "regular_event_kick_time.h"
#include "regular_event_trigger.h"
#include "regular_event_all_player_dead.h"
#include "regular_event_player_resurgence_req.h"
#include "regular_event_spawn_end.h"

CnImplementRootRTTI(RegularEvent);

RegularEvent* ParseRegularEvent( TiXmlElement* EventElement, Regular* pRegular )
{
    uint32 eventId = 0;
    EventElement->QueryUnsignedAttribute("id", &eventId);

    RegularEvent* pRegularEvent = NULL;

    switch(eventId)
    {
    case REGULAR_EVENT_PLAYER_DEAD:
        {
            pRegularEvent = CnNew RegularEvent_PlayerDead(pRegular);
        }
        break;
    case REGULAR_EVENT_OFFLINE_PLAYER_DEAD:
        {
            pRegularEvent = CnNew RegularEvent_OfflinePlayerDead(pRegular);
        }
        break;
    case REGULAR_EVENT_MONSTER_CLEAR:
        {
            pRegularEvent = CnNew RegularEvent_MonsterClear(pRegular);
        }
        break;
    case REGULAR_EVENT_MONSTER_DEAD:
        {
            pRegularEvent = CnNew RegularEvent_MonsterDead(pRegular);
        }
        break;
    case REGULAR_EVENT_TIMER:
        {
            pRegularEvent = CnNew RegularEvent_Timer(pRegular);
        }
        break;
    case REGULAR_EVENT_TRIGGER:
        {
            pRegularEvent = CnNew RegularEvent_Trigger(pRegular);
        }
        break;
    case REGULAR_EVENT_ALL_PLAYER_DEAD:
        {
            pRegularEvent = CnNew RegularEvent_AllPlayerDead(pRegular);
        }
        break;
    case REGULAR_EVENT_PLAYER_RESURGENCE_REQ:
        {
            pRegularEvent = CnNew RegularEvent_PlayerResurgence(pRegular);
        }
        break;
    case REGULAR_EVENT_START_TIME:
        {
            pRegularEvent = CnNew RegularEvent_StartTime(pRegular);
        }
        break;
    case REGULAR_EVENT_END_TIME:
        {
            pRegularEvent = CnNew RegularEvent_EndTime(pRegular);
        }
        break;
    case REGULAR_EVENT_KICK_TIME:
        {
            pRegularEvent = CnNew RegularEvent_KickTime(pRegular);
        }
        break;
    case REGULAR_EVENT_SPAWN_END:
        {
            pRegularEvent = CnNew RegularEvent_SpawnEnd(pRegular);
        }
        break;
    default:
        CnAssert(false);        // 未定义的关卡事件
        break;
    }

    if(pRegularEvent)
    {
        // 解析事件的参数
        pRegularEvent->ParseEventXml(EventElement);
        
        // 解析事件中的行为
        TiXmlElement* actionElement = EventElement->FirstChildElement();
        for(; actionElement; actionElement = actionElement->NextSiblingElement())
        {
            std::string nodeValue = actionElement->Value();
            if (nodeValue != "action")
                continue;

            RegularAction* pRegularAction = ParseRegularAction(actionElement, pRegular);
            pRegularEvent->m_ActionList.push_back(pRegularAction);
        }
    }


    return pRegularEvent;
}

RegularEvent::RegularEvent(Regular* pRegular, int32 type) :
m_pRegular(pRegular)
{
    m_type      = type;
    m_active    = false;
}

RegularEvent::~RegularEvent()
{
    for(RegularActionList::iterator iter = m_ActionList.begin(); iter != m_ActionList.end(); iter++)
    {
        RegularAction* pAction = *iter;
        CnDelete pAction;
    }
    m_ActionList.clear();
}

Scene* RegularEvent::GetScene()
{
    return m_pRegular->GetScene();
}

void RegularEvent::SetActive(bool active)
{
    m_active = active;

    // 激活事件，触发行为
    if(active)
    {
        for(RegularActionList::iterator iter = m_ActionList.begin(); iter != m_ActionList.end(); iter++)
        {
            RegularAction* pAction = *iter;
            pAction->Do();
        }
    }
}
