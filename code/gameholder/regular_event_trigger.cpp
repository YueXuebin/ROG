#include "gameholder_pch.h"
#include "game_util.h"
#include "regular_event_trigger.h"
#include "regular.h"
#include "scene.h"
#include "event_define.h"

RegularEvent_Trigger::RegularEvent_Trigger( Regular* pRegular ) :
RegularEvent(pRegular, REGULAR_EVENT_TRIGGER)
{
    m_triggerID = 0;
    m_number = 0;
    //m_time = 999999999;
}

RegularEvent_Trigger::~RegularEvent_Trigger()
{

}

void RegularEvent_Trigger::ParseEventXml( TiXmlElement* EventElement )
{
    EventElement->QueryUnsignedAttribute("trigger", &m_triggerID);
    if (!m_pRegular->GetScene()->GetTriggerByID(m_triggerID))
        CnFatal("triggerID %d not exist\n", m_triggerID);
    EventElement->QueryUnsignedAttribute("number", &m_number);

    //int32 rt = EventElement->QueryUnsignedAttribute("time", &m_time);
    //if (rt == TIXML_NO_ATTRIBUTE)
    //    CnFatal("not interval time\n");
    //else
    //{
    //    CnAssert((int32)m_time >= 0);
    //    m_time *= 1000;
    //}
}

void RegularEvent_Trigger::ReceiveEvent( uint32 frameCount, uint32 eventId, int32 arg1, int32 arg2, const std::string& arg3 )
{
    if (m_number == 0)
        return;

    if (arg1 != m_triggerID)
    {
        return;
    }

    switch (eventId)
    {
    case TRIGGER_REGULAR_EVENT:
        {
            SetActive(true);
            m_number--;
        }
        break;
    }
}






