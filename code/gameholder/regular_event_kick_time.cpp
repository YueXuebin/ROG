#include "gameholder_pch.h"
#include "game_util.h"
#include "regular_event_kick_time.h"
#include "regular.h"

RegularEvent_KickTime::RegularEvent_KickTime( Regular* pRegular ) :
RegularEvent(pRegular, REGULAR_EVENT_KICK_TIME)
{
}

RegularEvent_KickTime::~RegularEvent_KickTime()
{

}

