#include "gameholder_pch.h"
#include "regular_action_attackable.h"
#include "regular.h"
#include "scene.h"
#include "battle_unit.h"
#include "creature.h"

RegularAction_Attackable::RegularAction_Attackable(Regular* pRegular) :
RegularAction(pRegular)
{
    m_attackable    = TRUE;
    m_sid           = 0;
    m_creatureGroup = ALL_GROUP;
}

RegularAction_Attackable::~RegularAction_Attackable()
{

}

void RegularAction_Attackable::ParseActionXml(TiXmlElement* ActionElement)
{
    ActionElement->QueryUnsignedAttribute("sid", &m_sid);
    int32 attackable = 1;
    ActionElement->QueryIntAttribute("attackable", &attackable);
    m_attackable = (attackable == 1) ? TRUE : FALSE;
    m_creatureGroup = ALL_GROUP;
    ActionElement->QueryIntAttribute("creature_group", &m_creatureGroup);
}

void RegularAction_Attackable::Do()
{
    Scene::BattleUnitMap& battleUnitMap = m_pRegular->GetScene()->GetBattleUnits();

    for(Scene::BattleUnitMap::iterator battleUnitIt =  battleUnitMap.begin(); battleUnitIt != battleUnitMap.end(); ++battleUnitIt)
    {
        if(battleUnitIt->second->IsKindOf(&Creature::ms_RTTI))
        {
            Creature* pCreature = (Creature*)battleUnitIt->second;
            if(pCreature->GetCreatureId() == m_sid && (pCreature->GetGroup() == m_creatureGroup || m_creatureGroup == ALL_GROUP))
            {
                pCreature->SetAttackable(m_attackable);
            }
        }
    }
}

