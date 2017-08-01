#ifndef state_stun_h__
#define state_stun_h__

#include "state.h"

/*
 *	眩晕状态
 */
class StateStun : public State
{
public:
    StateStun(BattleUnit* owner);
    virtual ~StateStun();

    virtual bool CanChangeTo(uint32 state);

    virtual void Update();

    virtual void onEnterState();

};

#endif // state_stun_h__
