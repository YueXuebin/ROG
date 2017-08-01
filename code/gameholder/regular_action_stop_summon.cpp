#include "gameholder_pch.h"
#include "regular_action_stop_summon.h"
#include "regular.h"
#include "spawn.h"
#include "scene.h"

RegularAction_StopSummon::RegularAction_StopSummon( Regular* pRegular ) :
RegularAction(pRegular)
{
    m_spawn = 0;
    m_stop = false;
}

RegularAction_StopSummon::~RegularAction_StopSummon()
{

}

void RegularAction_StopSummon::ParseActionXml( TiXmlElement* ActionElement )
{
    ActionElement->QueryUnsignedAttribute("spawn",&m_spawn);
    if (!m_pRegular->GetScene()->GetSpawn(m_spawn))
        CnFatal("spawn id %d not exist\n", m_spawn);
    int32 bl = 0;
    ActionElement->QueryIntAttribute("stop",&bl);
    if(bl == 0)
        m_stop = false;
    else if(bl == 1)
        m_stop = true;
}

void RegularAction_StopSummon::Do()
{
    Spawn* pSpawn = m_pRegular->GetScene()->GetSpawn(m_spawn);

    if(!pSpawn)
    {
        CnWarn("spawn %d not exist\n", m_spawn);
        return;
    }

    pSpawn->SetSpawning(!m_stop);    
}

