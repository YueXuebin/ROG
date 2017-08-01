#include "gameholder_pch.h"
#include "state_idle.h"
#include "state_define.h"

StateIdle::StateIdle(BattleUnit* owner) : 
State(STATE_IDLE, owner)
{

}

StateIdle::~StateIdle()
{

}

bool StateIdle::CanChangeTo( uint32 state )
{
    return true;            // idle状态可以进入任何状态
}
