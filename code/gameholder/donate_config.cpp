
#include "gameholder_pch.h"
#include "player.h"
#include "game_util.h"
#include "donate_config.h"
#include "config_reader.h"

IMPLEMENT_SINGLETON(DonateConfig);


DonateConfig::DonateConfig()
{

}

DonateConfig::~DonateConfig()
{

}

bool DonateConfig::LoadConfig( const char* path )
{
    TiXmlDocument xmlDoc;

    if (!xmlDoc.LoadFile(path))
    {
        CnError("Load file: %s failed\n", path);
        return false;
    }

    TiXmlElement* xmlRoot = xmlDoc.RootElement();
    if(!xmlRoot) 
        return false;

    m_donate_template.Clear();

    for (TiXmlElement *pElement = xmlRoot->FirstChildElement(); pElement; pElement = pElement->NextSiblingElement())
    {
        if (Crown::SDStrcmp(pElement->Value(), "autotime") == 0)
        {
            ConfigReader::Read(pElement, "value", m_donate_template.autotime);
        }
        else if (Crown::SDStrcmp(pElement->Value(), "title_id") == 0)
        {
            ConfigReader::Read(pElement, "value", m_donate_template.title_id);
        }
        else if (Crown::SDStrcmp(pElement->Value(), "money_point") == 0)
        {
            ConfigReader::Read(pElement, "value", m_donate_template.money_point);
            ConfigReader::Read(pElement, "reward_percent", m_donate_template.reward_percent);
        }
    }

    return true;
}

const DonateTemplate& DonateConfig::GetDonateTemplate()
{
    return m_donate_template;
}




