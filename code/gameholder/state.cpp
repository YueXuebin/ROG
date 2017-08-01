#include "gameholder_pch.h"
#include "state.h"
#include "game_define.h"
#include "state_define.h"


State::State(uint32 type, BattleUnit* owner) :
m_type(type),
m_Owner(owner),
m_EndTime(STATE_FOREVER_TIME),
m_NextState(STATE_NONE)
{
    m_UpdateTimer = 0;
    
}

State::~State()
{

}

void State::onEnterState()
{
    m_UpdateTimer = 0;
}

void State::onLeaveState()
{

}

void State::Update()
{
    m_UpdateTimer += g_ServerUpdateTime;
}

bool State::isEnd()
{
    if(m_EndTime < 0)
        return false;

    return m_UpdateTimer >= m_EndTime;
}
