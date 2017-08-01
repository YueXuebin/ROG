#ifndef rog_regular_event_monster_clear_h__
#define rog_regular_event_monster_clear_h__

#include "rog_regular_event.h"

class RegularDungeon;

/*
 *	杀死所有怪
 */
class ROGRegularEvent_MonsterClear : public ROGRegularEvent
{
    CnDeclareRTTI
public:
    ROGRegularEvent_MonsterClear(RegularDungeon* pRegular);
    virtual ~ROGRegularEvent_MonsterClear();

    virtual void OneSecondUpdate(int32 deltatime);

private:
    bool IsAllDead();
};

#endif // rog_regular_event_monster_clear_h__
