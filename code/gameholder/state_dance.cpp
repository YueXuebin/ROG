#include "gameholder_pch.h"
#include "state_dance.h"
#include "state_define.h"


StateDance::StateDance(BattleUnit* owner) :
State(STATE_DANCE, owner)
{

}

StateDance::~StateDance()
{

}

bool StateDance::CanChangeTo( uint32 state )
{
    return true;
}
