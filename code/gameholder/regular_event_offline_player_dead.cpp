#include "gameholder_pch.h"
#include "regular_event_offline_player_dead.h"
#include "event_define.h"


RegularEvent_OfflinePlayerDead::RegularEvent_OfflinePlayerDead(Regular* pRegular) :
RegularEvent(pRegular, REGULAR_EVENT_OFFLINE_PLAYER_DEAD)
{

}

RegularEvent_OfflinePlayerDead::~RegularEvent_OfflinePlayerDead()
{

}

void RegularEvent_OfflinePlayerDead::ReceiveEvent( uint32 frameCount, uint32 eventId, int32 arg1, int32 arg2, const std::string& arg3 )
{
    switch(eventId)
    {
    case BATTLE_EVENT_OFFLINE_PLAYER_DEAD:
        
        SetActive(true);
        break;
    }
}
