#ifndef state_walk_h__
#define state_walk_h__

#include "state.h"


/*
 *	走状态
 */
class StateWalk : public State
{
public:
    StateWalk(BattleUnit* owner);
    virtual ~StateWalk();

    virtual bool CanChangeTo(uint32 state);
};

#endif // state_walk_h__
