#ifndef state_dance_h__
#define state_dance_h__

#include "state.h"

/*
 *	跳舞状态
 */
class StateDance : public State
{
public:
    StateDance(BattleUnit* owner);
    virtual ~StateDance();

    virtual bool CanChangeTo(uint32 state);
};

#endif // state_dance_h__
