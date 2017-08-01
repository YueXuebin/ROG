#include "gameholder_pch.h"
#include "game_util.h"
#include "regular_event_end_time.h"
#include "regular.h"

RegularEvent_EndTime::RegularEvent_EndTime( Regular* pRegular ) :
RegularEvent(pRegular, REGULAR_EVENT_END_TIME)
{
}

RegularEvent_EndTime::~RegularEvent_EndTime()
{

}

