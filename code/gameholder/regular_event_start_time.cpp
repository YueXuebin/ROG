#include "gameholder_pch.h"
#include "game_util.h"
#include "regular_event_start_time.h"
#include "regular.h"

RegularEvent_StartTime::RegularEvent_StartTime( Regular* pRegular ) :
RegularEvent(pRegular, REGULAR_EVENT_START_TIME)
{
}

RegularEvent_StartTime::~RegularEvent_StartTime()
{

}

