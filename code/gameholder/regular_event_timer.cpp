#include "gameholder_pch.h"
#include "game_util.h"
#include "regular_event_timer.h"
#include "regular_dungeon.h"


RegularEvent_Timer::RegularEvent_Timer( Regular* pRegular ) :
RegularEvent(pRegular, REGULAR_EVENT_TIMER)
{
    m_timer = 0;
}

RegularEvent_Timer::~RegularEvent_Timer()
{

}

void RegularEvent_Timer::ParseEventXml( TiXmlElement* EventElement )
{
    m_timer = 0;
    uint32 time = 0;
    EventElement->QueryUnsignedAttribute("time", &time);
    m_timer = time * 1000;
}

void RegularEvent_Timer::OneSecondUpdate( int32 deltatime )
{
    if(m_pRegular->IsKindOf(&RegularDungeon::ms_RTTI))
    {
        if( ((RegularDungeon*)m_pRegular)->GetState() < RegularDungeon::kStateStart)
            return;

        if( ((RegularDungeon*)m_pRegular)->GetGameTime() < m_timer )
            return;
    }

    if(GetActive())
        return;

    SetActive(true);
    return;
}
