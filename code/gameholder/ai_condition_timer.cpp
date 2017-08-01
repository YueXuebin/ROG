#include "gameholder_pch.h"
#include "ai_condition_timer.h"
#include "ai_define.h"



AIConditionTimer::AIConditionTimer() :
AIConditionBase(AI_CONDITION_TIMER)
{
    Reset();
}

AIConditionTimer::~AIConditionTimer()
{

}

void AIConditionTimer::onEnterState()
{
    AIConditionBase::onEnterState();

    Reset();
}

void AIConditionTimer::Reset()
{
    m_Timer = 0;
    m_Time = int32( Crown::SDRandomFloat((real)m_MaxTime - m_MinTime) + m_MinTime );
}

bool AIConditionTimer::IsSatisfy()
{

    if(m_Timer >= m_Time)
        return true;
    else 
        return false;
}

void AIConditionTimer::Update( int32 deltaTime )
{
    m_Timer += deltaTime;
}

void AIConditionTimer::ParseXML( TiXmlElement* pDoc )
{
    AIConditionBase::ParseXML(pDoc);

    pDoc->QueryIntAttribute("min_time", &m_MinTime);
    pDoc->QueryIntAttribute("max_time", &m_MaxTime);
}
