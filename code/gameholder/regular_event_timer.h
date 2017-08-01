/*
 *	计时器事件
 */
#ifndef regular_event_timer_h__
#define regular_event_timer_h__

#include "regular_event.h"

class RegularEvent_Timer : public RegularEvent
{
public:
    RegularEvent_Timer(Regular* pRegular);
    virtual ~RegularEvent_Timer();

    virtual void ParseEventXml(TiXmlElement* EventElement);
    virtual void OneSecondUpdate(int32 deltatime);
private:
    int32  m_timer;
};


#endif // regular_event_timer_h__
