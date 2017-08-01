#include "gameholder_pch.h"
#include "state_movie.h"
#include "state_define.h"
#include "battle_unit.h"


StateMovie::StateMovie( BattleUnit* owner ) :
State(STATE_MOVIE, owner),
m_AttackAble(true)
{

}

StateMovie::~StateMovie()
{

}

bool StateMovie::CanChangeTo( uint32 state )
{
    if(state == STATE_DIE)
        return true;
    return false;
}

void StateMovie::SetAttackable(bool b)
{
    m_AttackAble = b;
    m_Owner->UpdateAttackable();
}

void StateMovie::onEnterState()
{
    State::onEnterState();

    m_Owner->UpdateAttackable();
}

void StateMovie::onLeaveState()
{
    State::onLeaveState();

    m_Owner->UpdateAttackable();
}
