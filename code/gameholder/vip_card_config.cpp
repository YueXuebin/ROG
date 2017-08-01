#include "gameholder_pch.h"
#include "vip_card_config.h"

IMPLEMENT_SINGLETON(VipCardConfig)


VipCardConfig::VipCardConfig()
{

}

VipCardConfig::~VipCardConfig()
{

}

bool VipCardConfig::LoadConfig( const char* path )
{
    VipCardTemplate vipTmpl;
    TiXmlDocument xmlDoc;

    if (!xmlDoc.LoadFile(path))
    {
        CnError("Load vip card file: %s failed\n", path);
        return false;
    }

    TiXmlElement* xmlRoot = xmlDoc.RootElement();
    if(!xmlRoot) return false;

    for(TiXmlElement* vipEle = xmlRoot->FirstChildElement(); vipEle; vipEle = vipEle->NextSiblingElement())
    {
        vipTmpl.Clear();

        vipTmpl.id = SDAtoi(vipEle->Attribute("id"));
        vipTmpl.duration = SDAtoi(vipEle->Attribute("duration"));
        vipTmpl.money_point = SDAtoi(vipEle->Attribute("money_point"));
        vipTmpl.vip = SDAtoi(vipEle->Attribute("vip"));
        vipTmpl.name = vipEle->Attribute("name");

        if(m_vip_cards.find(vipTmpl.id) == m_vip_cards.end())
        {
            m_vip_cards[vipTmpl.id] = vipTmpl;
        }
        else
        {
            CnWarn("Duplicate vip card id : %u\n", vipTmpl.id);
        }
    }

    return true;
}

const VipCardTemplate*  VipCardConfig::GetVipCardTemplate(uint32 cardId)
{
    if( m_vip_cards.find(cardId) == m_vip_cards.end() )
        return NULL;        // 该id不存在

    return &m_vip_cards[cardId];
}

uint32 VipCardConfig::GetVipCardDuration(uint32 cardId)
{
    if( m_vip_cards.find(cardId) == m_vip_cards.end() )
        return 0;        // 该id不存在

    return m_vip_cards[cardId].duration;
}

uint32 VipCardConfig::GetVipCardMoneyPoint(uint32 cardId)
{
    if( m_vip_cards.find(cardId) == m_vip_cards.end() )
        return 0;        // 该id不存在

    return m_vip_cards[cardId].money_point;
}

uint32 VipCardConfig::GetVipCardDirectVIP(uint32 cardId)
{
    if( m_vip_cards.find(cardId) == m_vip_cards.end() )
        return 0;        // 该id不存在

    return m_vip_cards[cardId].vip;
}

