/*
 *	´¥·¢Æ÷ÊÂ¼þ
 */
#ifndef regular_event_trigger_h__
#define regular_event_trigger_h__

#include "regular_event.h"

class RegularEvent_Trigger : public RegularEvent
{
public:
    RegularEvent_Trigger(Regular* pRegular);
    virtual ~RegularEvent_Trigger();

    virtual void ParseEventXml(TiXmlElement* EventElement);
    virtual void ReceiveEvent(uint32 frameCount, uint32 eventId, int32 arg1, int32 arg2, const std::string& arg3);
private:
    uint32 m_triggerID;
    uint32 m_number;
    //uint32 m_time;
};

#endif // regular_event_trigger_h__
