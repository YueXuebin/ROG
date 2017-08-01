#include "gameholder_pch.h"
#include "ai_action_base.h"



AIActionBase::AIActionBase(uint32 id) : 
m_id(id)
{
    m_pController = NULL;

    m_intervalTimer = 0;
    m_countTimer = 0;
    m_run = true;
}

AIActionBase::~AIActionBase()
{
    
}

void AIActionBase::ParseXML( TiXmlElement* pDoc )
{
    pDoc->QueryIntAttribute("count", &m_count);
    pDoc->QueryIntAttribute("interval", &m_interval);
    pDoc->QueryBoolAttribute("run",&m_run);
}

void AIActionBase::onEnterState()
{
    m_intervalTimer = 0;
    m_countTimer = m_count;

    Do();
    m_countTimer --;
}

void AIActionBase::Update( int32 deltaTime )
{
    m_intervalTimer += deltaTime;
    if (IntervalAndCount())
    {
        Do();
    }
}


void AIActionBase::AddSkill(uint32 skillId, int32 probability)
{
    AISkillData skillData;
    skillData.skillId = skillId;
    skillData.probability = probability;
    m_SkillList.push_back(skillData);
}

void AIActionBase::ClearSkill()
{
    m_SkillList.clear();
}


bool AIActionBase::IntervalAndCount()
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

