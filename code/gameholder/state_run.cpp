#include "gameholder_pch.h"
#include "state_run.h"
#include "state_define.h"


StateRun::StateRun(BattleUnit* owner) :
State(STATE_RUN, owner)
{

}

StateRun::~StateRun()
{

}

bool StateRun::CanChangeTo( uint32 state )
{
    return true;
}
