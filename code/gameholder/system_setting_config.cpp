#include "gameholder_pch.h"
#include "system_setting_config.h"

IMPLEMENT_SINGLETON(SystemSettingConfig)

SystemSettingConfig::SystemSettingConfig()
{
    Clear();
}

SystemSettingConfig::~SystemSettingConfig()
{

}

void SystemSettingConfig::Clear()
{
    m_volume        = 0;
    m_se_volume     = 0;
    m_show_player   = 0;
    m_show_pet      = 0;
    m_show_shadow   = 0;
    m_show_particle = 0;
    m_mipmap        = 0;
    m_fullscreen    = 0;
    m_ride_pet      = 0;
    m_light         = 0;
    m_beam          = 0;
    m_effect_level  = 0;
    m_fog           = 0;
}

bool SystemSettingConfig::LoadConfig(const char* path)
{
    TiXmlDocument xmlDoc;

    if (!xmlDoc.LoadFile(path))
    {
        CnError("Load item file: %s failed\n", path);
        return false;
    }

    TiXmlElement* xmlRoot = xmlDoc.RootElement();
    if(!xmlRoot) return false;

    //配置信息
    for(TiXmlElement* xmlEle = xmlRoot->FirstChildElement(); xmlEle; xmlEle = xmlEle->NextSiblingElement())
    {
        if(strcmp(xmlEle->Value(), "volume") == 0)
        {
            m_volume = Crown::SDAtou(xmlEle->Attribute("value"));
        }
        else if(strcmp(xmlEle->Value(), "se_volume") == 0)
        {
            m_se_volume = Crown::SDAtou(xmlEle->Attribute("value"));
        }
        else if(strcmp(xmlEle->Value(), "show_player") == 0)
        {
            m_show_player = Crown::SDAtou(xmlEle->Attribute("value"));
        }
        else if(strcmp(xmlEle->Value(), "show_pet") == 0)
        {
            m_show_pet = Crown::SDAtou(xmlEle->Attribute("value"));
        }
        else if(strcmp(xmlEle->Value(), "show_shadow") == 0)
        {
            m_show_shadow = Crown::SDAtou(xmlEle->Attribute("value"));
        }
        else if(strcmp(xmlEle->Value(), "show_particle") == 0)
        {
            m_show_particle = Crown::SDAtou(xmlEle->Attribute("value"));
        }
        else if(strcmp(xmlEle->Value(), "mipmap") == 0)
        {
            m_mipmap = Crown::SDAtou(xmlEle->Attribute("value"));
        }
        else if(strcmp(xmlEle->Value(), "fullscreen") == 0)
        {
            m_fullscreen = Crown::SDAtou(xmlEle->Attribute("value"));
        }
        else if(strcmp(xmlEle->Value(), "ride_pet") == 0)
        {
            m_ride_pet = Crown::SDAtou(xmlEle->Attribute("value"));
        }
        else if(strcmp(xmlEle->Value(), "beam") == 0)
        {
            m_beam = Crown::SDAtou(xmlEle->Attribute("value"));
        }
        else if(strcmp(xmlEle->Value(), "light") == 0)
        {
            m_light = Crown::SDAtou(xmlEle->Attribute("value"));
        }
        else if(strcmp(xmlEle->Value(), "effect_level") == 0)
        {
            m_effect_level = Crown::SDAtou(xmlEle->Attribute("value"));
        }
        else if (strcmp(xmlEle->Value(), "fog") == 0)
        {
            m_fog = Crown::SDAtou(xmlEle->Attribute("value"));
        }
        else if (strcmp(xmlEle->Value(), "frame") == 0)
        {
            m_frame = Crown::SDAtou(xmlEle->Attribute("value"));
        }
    }

    return true;
}
