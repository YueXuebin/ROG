#include "gameholder_pch.h"
#include "state_singing.h"
#include "state_define.h"

/*
StateSinging::StateSinging( BattleUnit* owner ) :
State(STATE_SINGING, owner)
{
    m_CanBreak = true;
}

StateSinging::~StateSinging()
{

}

bool StateSinging::CanChangeTo( uint32 state )
{
    if(m_CanBreak)
    {
        return true;
    }
    else
    {
        if(state == STATE_STUN || state == STATE_DIE)
            return true;
        else
            return false;
    }
}
*/