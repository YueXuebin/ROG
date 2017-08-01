#include "gameholder_pch.h"
#include "regular_action_create_monster.h"
#include "regular.h"
#include "sdrandom.h"
#include "regular_data.h"



RegularAction_CreateMonster::RegularAction_CreateMonster(Regular* pRegular) :
RegularAction(pRegular)
{
    m_MonsterNum = 0;
}

RegularAction_CreateMonster::~RegularAction_CreateMonster()
{

}

void RegularAction_CreateMonster::ParseActionXml( TiXmlElement* ActionElement )
{
    m_MonsterNum = 0;
    ActionElement->QueryUnsignedAttribute("monster_num", &m_MonsterNum);
    m_MonsterRadius = 0;
    ActionElement->QueryUnsignedAttribute("radius",&m_MonsterRadius);

    TiXmlElement* pawnElement = ActionElement->FirstChildElement("pawn");
    if(pawnElement)
        m_MonsterData = ParseMonsterAndNpcData(pawnElement);
}

void RegularAction_CreateMonster::Do()
{
    // ¥¥Ω®π÷ŒÔ
    for (uint32 i=0; i<m_MonsterNum; i++)
    {
        int32 Xran = Crown::SDRandom(m_MonsterRadius) - m_MonsterRadius/2;
        int32 Yran = Crown::SDRandom(m_MonsterRadius) - m_MonsterRadius/2;
        MonsterData monsterData = m_MonsterData;
        monsterData.x += Xran;
        monsterData.y += Yran;
        m_pRegular->AddMonster(monsterData);
    }
}
