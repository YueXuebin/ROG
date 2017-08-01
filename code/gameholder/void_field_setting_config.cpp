#include "gameholder_pch.h"
#include "void_field_setting_config.h"

IMPLEMENT_SINGLETON(VoidFieldSettingConfig)

VoidFieldSettingConfig::VoidFieldSettingConfig()
{
    Clear();
}

VoidFieldSettingConfig::~VoidFieldSettingConfig()
{

}

void VoidFieldSettingConfig::Clear()
{
    m_current_hook            = 0;
    m_auto_use_revival      = 0;
    m_auto_buy_revival            = 0;
    m_current_revival      = 0;
    m_reconnect_on_loss        = 0;
}

bool VoidFieldSettingConfig::LoadConfig(const char* path)
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
        if(strcmp(xmlEle->Value(), "current_hook") == 0)
        {
            m_current_hook = Crown::SDAtou(xmlEle->Attribute("value"));
        }
        else if(strcmp(xmlEle->Value(), "auto_use_revival") == 0)
        {
            m_auto_use_revival = Crown::SDAtou(xmlEle->Attribute("value"));
        }
        else if(strcmp(xmlEle->Value(), "auto_buy_revival") == 0)
        {
            m_auto_buy_revival = Crown::SDAtou(xmlEle->Attribute("value"));
        }
        else if(strcmp(xmlEle->Value(), "current_revival") == 0)
        {
            m_current_revival = Crown::SDAtou(xmlEle->Attribute("value"));
        }
        else if(strcmp(xmlEle->Value(), "reconnect_on_loss") == 0)
        {
            m_reconnect_on_loss = Crown::SDAtou(xmlEle->Attribute("value"));
        }
    }

    return true;
}
