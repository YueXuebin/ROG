#include "gameholder_pch.h"
#include "state_attack.h"
#include "state_define.h"
#include "battle_unit.h"


StateAttack::StateAttack(BattleUnit* owner) :
State(STATE_ATTACK, owner),
m_CanBreak(false)
{

}

StateAttack::~StateAttack()
{

}

bool StateAttack::CanChangeTo( uint32 state )
{
    if(state == STATE_STUN || state == STATE_DIE || state == STATE_MOVIE)
        return true;

    if(m_Owner->GetObjectType() == OBJ_PLAYER && (state == STATE_ATTACK))
        return true;        // 玩家可以技能中断技能

    return false;
}

void StateAttack::onEnterState()
{
    State::onEnterState();
    
}

void StateAttack::Update()
{
    State::Update();
}

