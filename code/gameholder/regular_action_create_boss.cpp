#include "gameholder_pch.h"
#include "regular_action_create_boss.h"
#include "regular.h"

RegularAction_CreateBoss::RegularAction_CreateBoss(Regular* pRegular) :
RegularAction(pRegular)
{
    m_BossCount = 0;
    m_BossRadius = 0;
}

RegularAction_CreateBoss::~RegularAction_CreateBoss()
{

}

void RegularAction_CreateBoss::ParseActionXml( TiXmlElement* ActionElement )
{
    m_BossCount = 0;
    ActionElement->QueryUnsignedAttribute("boss_num", &m_BossCount);
    m_BossRadius = 0;
    ActionElement->QueryUnsignedAttribute("radius",&m_BossRadius);

    TiXmlElement* bossElement = ActionElement->FirstChildElement("boss");
    if(bossElement)
        m_BossData = ParseBossData(bossElement);
}

void RegularAction_CreateBoss::Do()
{
    // ´´½¨Boss
    for(uint32 i=0; i<m_BossCount; i++)
    {
        BossData bossData = m_BossData;
        if(m_BossRadius != 0)
        {
            int32 Xran = Crown::SDRandom(m_BossRadius) - m_BossRadius/2;
            int32 Yran = Crown::SDRandom(m_BossRadius) - m_BossRadius/2;
            bossData.x += Xran;
            bossData.y += Yran;
        }

        m_pRegular->AddBoss(bossData);
    }
}
