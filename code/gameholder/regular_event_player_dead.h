/*
 *	Íæ¼ÒËÀÍöÊÂ¼ş
 */
#ifndef regular_event_player_dead_h__
#define regular_event_player_dead_h__

#include "regular_event.h"

class RegularEvent_PlayerDead : public RegularEvent
{
public:
    RegularEvent_PlayerDead(Regular* pRegular);
    virtual ~RegularEvent_PlayerDead();

    virtual void ReceiveEvent(uint32 frameCount, uint32 eventId, int32 arg1, int32 arg2, const std::string& arg3);

    virtual void OneSecondUpdate(int32 deltatime);
};

#endif // regular_event_player_dead_h__
