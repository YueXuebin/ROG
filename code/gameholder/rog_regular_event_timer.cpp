#include "gameholder_pch.h"
#include "rog_regular_event_timer.h"
#include "event_define.h"
#include "regular_action.h"
#include "scene.h"
#include "regular_dungeon.h"

ROGRegularEvent_Timer::ROGRegularEvent_Timer(RegularDungeon* pRegular, bool startActive) :
ROGRegularEvent(pRegular, ROG_REGULAR_EVENT_TIMER)
{
    m_active = m_startActive = startActive;
}

ROGRegularEvent_Timer::~ROGRegularEvent_Timer()
{

}

void ROGRegularEvent_Timer::OneSecondUpdate(int32 deltatime)
{
    if(GetActive() != m_startActive)
        return;     // 已经完成判断,不再判断

    if( GetRegular()->GetState() < RegularDungeon::kStateStart)
        return;

    m_param2 = GetRegular()->GetGameTime();

    if( m_param2 < (int32)GetTime() )
        return;

    GetRegular()->SetConditionChanged();
    SetActive(!m_startActive);
}
