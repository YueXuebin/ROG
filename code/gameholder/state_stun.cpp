#include "gameholder_pch.h"
#include "state_stun.h"
#include "state_define.h"


StateStun::StateStun( BattleUnit* owner ) :
State(STATE_STUN, owner)
{
}

StateStun::~StateStun()
{

}

bool StateStun::CanChangeTo( uint32 state )
{
    if(state == STATE_DIE || state == STATE_MOVIE)
        return true;    // 眩晕状态只能->死亡状态或演绎状态

    return false;
}

void StateStun::onEnterState()
{
    State::onEnterState();

}

void StateStun::Update()
{
    State::Update();
}

