/*
 *	Íæ¼ÒËÀÍöÊÂ¼ş
 */
#ifndef regular_event_all_player_dead_h__
#define regular_event_all_player_dead_h__


#include "regular_event.h"

class RegularEvent_AllPlayerDead : public RegularEvent
{
public:
    RegularEvent_AllPlayerDead(Regular* pRegular);
    virtual ~RegularEvent_AllPlayerDead();

    virtual void OneSecondUpdate(int32 deltatime);
};

#endif // regular_event_all_player_dead_h__