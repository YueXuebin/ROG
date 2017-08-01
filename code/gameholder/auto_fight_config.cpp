#include "gameholder_pch.h"
#include "auto_fight_config.h"

IMPLEMENT_SINGLETON(AutoFightConfig)

AutoFightConfig::AutoFightConfig()
{
    Clear();
}

AutoFightConfig::~AutoFightConfig()
{

}

void AutoFightConfig::Clear()
{
    m_hp            = 0;
    m_hp_order      = 0;
    m_mp            = 0;
    m_mp_order      = 0;
    m_skills        = 0;
    m_auto_revival  = 0;
    m_auto_fight    = 0;
    m_auto_buy_hp_drug = 0;
    m_auto_buy_mp_drug = 0;
    m_auto_buy_attack_drug  = 0;
    m_auto_buy_defence_drug = 0;
    m_auto_buy_speed_drug   = 0;
    m_auto_use_attack_drug  = 0;
    m_auto_use_defence_drug = 0;
    m_auto_use_speed_drug   = 0;
}

bool AutoFightConfig::LoadConfig(const char* path)
{
    TiXmlDocument xmlDoc;

    if (!xmlDoc.LoadFile(path))
    {
        CnError("Load item file: %s failed\n", path);
        return false;
    }

    TiXmlElement* xmlRoot = xmlDoc.RootElement();
    if(!xmlRoot) return false;

    //ÅäÖÃÐÅÏ¢
    for(TiXmlElement* xmlEle = xmlRoot->FirstChildElement(); xmlEle; xmlEle = xmlEle->NextSiblingElement())
    {
        if(strcmp(xmlEle->Value(), "hp") == 0)
        {
            m_hp = Crown::SDAtou(xmlEle->Attribute("value"));
        }
        else if(strcmp(xmlEle->Value(), "hp_order") == 0)
        {
            m_hp_order = Crown::SDAtou(xmlEle->Attribute("value"));
        }
        else if(strcmp(xmlEle->Value(), "mp") == 0)
        {
            m_mp = Crown::SDAtou(xmlEle->Attribute("value"));
        }
        else if(strcmp(xmlEle->Value(), "mp_order") == 0)
        {
            m_mp_order = Crown::SDAtou(xmlEle->Attribute("value"));
        }
        else if(strcmp(xmlEle->Value(), "skills") == 0)
        {
            m_skills = Crown::SDAtou(xmlEle->Attribute("value"));
        }
        else if(strcmp(xmlEle->Value(), "auto_revival") == 0)
        {
            m_auto_revival = Crown::SDAtou(xmlEle->Attribute("value"));
        }
        else if(strcmp(xmlEle->Value(), "auto_fight") == 0)
        {
            m_auto_fight = Crown::SDAtou(xmlEle->Attribute("value"));
        }
        else if(strcmp(xmlEle->Value(), "auto_buy_hp_drug") == 0)
        {
            m_auto_buy_hp_drug = Crown::SDAtou(xmlEle->Attribute("value"));
        }
        else if(strcmp(xmlEle->Value(), "auto_buy_mp_drug") == 0)
        {
            m_auto_buy_mp_drug = Crown::SDAtou(xmlEle->Attribute("value"));
        }
        else if(strcmp(xmlEle->Value(), "auto_buy_attack_drug") == 0)
        {
            m_auto_buy_attack_drug = Crown::SDAtou(xmlEle->Attribute("value"));
        }
        else if(strcmp(xmlEle->Value(), "auto_buy_defence_drug") == 0)
        {
            m_auto_buy_defence_drug = Crown::SDAtou(xmlEle->Attribute("value"));
        }
        else if(strcmp(xmlEle->Value(), "auto_buy_speed_drug") == 0)
        {
            m_auto_buy_speed_drug = Crown::SDAtou(xmlEle->Attribute("value"));
        }
        else if(strcmp(xmlEle->Value(), "auto_use_attack_drug") == 0)
        {
            m_auto_use_attack_drug = Crown::SDAtou(xmlEle->Attribute("value"));
        }
        else if(strcmp(xmlEle->Value(), "auto_use_defence_drug") == 0)
        {
            m_auto_use_defence_drug = Crown::SDAtou(xmlEle->Attribute("value"));
        }
        else if(strcmp(xmlEle->Value(), "auto_use_speed_drug") == 0)
        {
            m_auto_use_speed_drug = Crown::SDAtou(xmlEle->Attribute("value"));
        }
    }

    return true;
}
