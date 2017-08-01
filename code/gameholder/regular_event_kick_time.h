/*
 *	进入踢人状态事件
 */
#ifndef regular_event_kick_time_h__
#define regular_event_kick_time_h__

#include "regular_event.h"

class RegularEvent_KickTime : public RegularEvent
{
public:
    RegularEvent_KickTime(Regular* pRegular);
    virtual ~RegularEvent_KickTime();


};

#endif // regular_event_kick_time_h__
