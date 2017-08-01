#ifndef state_run_h__
#define state_run_h__

#include "state.h"


/*
 *	跑状态
 */
class StateRun : public State
{
public:
    StateRun(BattleUnit* owner);
    virtual ~StateRun();

    virtual bool CanChangeTo(uint32 state);
};

#endif // state_run_h__
