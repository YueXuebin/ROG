#include "gameholder_pch.h"
#include "dance_config.h"

IMPLEMENT_SINGLETON(DanceConfig)

DanceConfig::DanceConfig()
{
    m_dance.clear();
}

DanceConfig::~DanceConfig()
{

}

bool DanceConfig::LoadConfig(const char* path)
{
    DanceTemplate DanceTmpl;
    TiXmlDocument xmlDoc;

    if (!xmlDoc.LoadFile(path))
    {
        CnError("Load dance file: %s failed\n", path);
        return false;
    }

    TiXmlElement* xmlRoot = xmlDoc.RootElement();
    if(!xmlRoot)
        return false;


    for(TiXmlElement* danceEle = xmlRoot->FirstChildElement(); danceEle; danceEle = danceEle->NextSiblingElement())
    {
        DanceTmpl.Clear();

        if(strcmp(danceEle->Value(), "dance") == 0)
        {
            danceEle->QueryUnsignedAttribute("type", &DanceTmpl.type);
            danceEle->QueryUnsignedAttribute("exp", &DanceTmpl.exp);
            danceEle->QueryUnsignedAttribute("time", &DanceTmpl.time);
            DanceTmpl.time *= MINUTES_SECONDS;
            danceEle->QueryIntAttribute("every_second", &DanceTmpl.everySecond);
            danceEle->QueryIntAttribute("gain", &DanceTmpl.gain);
            danceEle->QueryIntAttribute("hp", &DanceTmpl.hp_rate);
            danceEle->QueryIntAttribute("mp", &DanceTmpl.mp_rate);

            if(m_dance.find(DanceTmpl.type) != m_dance.end())
            {
                CnError("dance type:%d is exist\n", DanceTmpl.type);
                break;
            }
            else
            {
                m_dance[DanceTmpl.type] = DanceTmpl;
            }
        }
    }
    return true;
}

const DanceTemplate* DanceConfig::GetDanceTemplate(uint32 danceType)
{
    std::map<uint32, DanceTemplate>::iterator danceIt = m_dance.find(danceType);
    if(danceIt == m_dance.end())
    {
        return NULL;
    }

    return &danceIt->second;
}

const std::map<uint32, DanceTemplate>* DanceConfig::GetDanceTemplate()
{
    return &m_dance;
}

