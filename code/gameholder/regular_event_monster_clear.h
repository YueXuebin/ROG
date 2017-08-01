/*
 *	¹ÖÎïËÀ¹â
 */
#ifndef regular_event_monster_clear_h__
#define regular_event_monster_clear_h__

#include "regular_event.h"

class RegularEvent_MonsterClear : public RegularEvent
{
public:
    RegularEvent_MonsterClear(Regular* pRegular);
    virtual ~RegularEvent_MonsterClear();

    virtual void OneSecondUpdate(int32 deltatime);
};

#endif // regular_event_monster_clear_h__
