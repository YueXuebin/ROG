#include "gameholder_pch.h"
#include "ai_condition_base.h"


AIConditionBase::AIConditionBase(uint32 id) :
m_id(id),
m_pController(NULL)
{
    m_nextStateId = 0;
    m_count = 0;
    m_interval = 0;
    
    m_intervalTimer = 0;
    m_countTimer = 0;
}

AIConditionBase::~AIConditionBase()
{

}

void AIConditionBase::onEnterState()
{
    m_intervalTimer = m_interval;
    m_countTimer = m_count;
}

void AIConditionBase::Update( int32 deltaTime )
{
    m_intervalTimer += deltaTime;
}

void AIConditionBase::ParseXML(TiXmlElement* pDoc)
{
    pDoc->QueryIntAttribute("count", &m_count);             // 次数
    pDoc->QueryIntAttribute("interval", &m_interval);       // 间隔
}

bool AIConditionBase::IntervalAndCount()
{
    if(m_intervalTimer < m_interval)
        return false;

    if(m_count == 0)
        return true;        // 无限循环

    if(m_countTimer <= 0)
        return false;       // 循环次数用完
    
    m_intervalTimer -= m_interval;
    m_countTimer--;
    return true;
}
