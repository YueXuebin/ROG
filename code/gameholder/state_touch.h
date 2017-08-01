#ifndef state_patch_h__
#define state_patch_h__

#include "state.h"

/*
 *	采集状态
 */
class StateTouch : public State
{
public:
    StateTouch(BattleUnit* owner);
    virtual ~StateTouch();

    virtual bool CanChangeTo(uint32 state);

    virtual void onLeaveState();
private:
};

#endif // state_patch_h__
