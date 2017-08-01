#include "gameholder_pch.h"
#include "regular_event_monster_dead.h"
#include "event_define.h"


RegularEvent_MonsterDead::RegularEvent_MonsterDead(Regular* pRegular) :
RegularEvent(pRegular, REGULAR_EVENT_MONSTER_DEAD)
{
    m_CreatureId = 0;
    m_creatureGroup         = -1;
    m_CreatureNum   = 1;
    m_CreatureIndex = 0;
}

RegularEvent_MonsterDead::~RegularEvent_MonsterDead()
{

}

void RegularEvent_MonsterDead::ReceiveEvent( uint32 frameCount, uint32 eventId, int32 arg1, int32 arg2, const std::string& arg3 )
{
    uint32 creatureId = 0;
    int32 group     = 0;
    switch(eventId)
    {
    case BATTLE_EVENT_MONSTER_DEAD:
        {
            creatureId = arg1;
            std::vector<std::string> strList = Crown::SDSplitStrings(arg3, '&');
            group = Crown::SDAtou(strList[1].c_str());
            OnRecvCreatureDead(creatureId, group);
        }
        break;
    case BATTLE_EVENT_BOSS_DEAD:
        {
            creatureId = (uint32)arg1;
            std::vector<std::string> strList = Crown::SDSplitStrings(arg3, '&');
            group = SDAtoi(strList[1].c_str());
            OnRecvCreatureDead(creatureId, group);
        }
        break;
    }
}

void RegularEvent_MonsterDead::ParseEventXml( TiXmlElement* EventElement )
{
    m_CreatureId = 0;
    EventElement->QueryUnsignedAttribute("creature_id", &m_CreatureId);
    m_creatureGroup = -1;
    EventElement->QueryIntAttribute("creature_group", &m_creatureGroup);
    m_CreatureNum = 1;
    EventElement->QueryUnsignedAttribute("creature_num", &m_CreatureNum);
}

void RegularEvent_MonsterDead::OnRecvCreatureDead(uint32 creatureId, int32 group)
{
    if(m_creatureGroup != ALL_GROUP && group != m_creatureGroup)
        return;

    if(creatureId == m_CreatureId)
        ++m_CreatureIndex;

    if(m_CreatureIndex >= m_CreatureNum)
    {
        SetActive(true);
        m_CreatureIndex = 0;
    }
}
