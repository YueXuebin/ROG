#include "gameholder_pch.h"
#include "state_define.h"
#include "state_walk.h"


StateWalk::StateWalk( BattleUnit* owner ) :
State(STATE_WALK, owner)
{

}

StateWalk::~StateWalk()
{

}

bool StateWalk::CanChangeTo( uint32 state )
{
    return true;
}
