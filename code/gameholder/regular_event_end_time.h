/*
 *	进入结束状态事件
 */
#ifndef regular_event_end_time_h__
#define regular_event_end_time_h__

#include "regular_event.h"

class RegularEvent_EndTime : public RegularEvent
{
public:
    RegularEvent_EndTime(Regular* pRegular);
    virtual ~RegularEvent_EndTime();

};

#endif // regular_event_end_time_h__
