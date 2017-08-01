#include "gameholder_pch.h"
#include "regular_action_clear_all_monsters.h"
#include "regular.h"
#include "scene.h"
#include "spawn.h"

RegularAction_ClearAllMosters::RegularAction_ClearAllMosters(Regular* pRegular) :
RegularAction(pRegular)
{
    m_spawn = 0;
}

RegularAction_ClearAllMosters::~RegularAction_ClearAllMosters()
{

}

void RegularAction_ClearAllMosters::ParseActionXml( TiXmlElement* ActionElement )
{
    ActionElement->QueryUnsignedAttribute("spawn", &m_spawn);
    if (!m_pRegular->GetScene()->GetSpawn(m_spawn))
        CnFatal("spawn id %d not exist\n", m_spawn);
}

void RegularAction_ClearAllMosters::Do()
{
    m_pRegular->GetScene()->GetSpawn(m_spawn)->SetClearAllMosters();
}
