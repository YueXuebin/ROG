/*
 *	进入开始状态事件
 */
#ifndef regular_event_start_time_h__
#define regular_event_start_time_h__

#include "regular_event.h"

class RegularEvent_StartTime : public RegularEvent
{
public:
    RegularEvent_StartTime(Regular* pRegular);
    virtual ~RegularEvent_StartTime();

private:
};

#endif // regular_event_start_time_h__
