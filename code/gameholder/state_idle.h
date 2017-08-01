#ifndef state_idle_h__
#define state_idle_h__

#include "state.h"


/*
 *	待机状态(idle)
 */
class StateIdle : public State
{
public:
    StateIdle(BattleUnit* owner);
    virtual ~StateIdle();

    virtual bool CanChangeTo(uint32 state);

};



#endif // state_idle_h__
