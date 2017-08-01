#include "gameholder_pch.h"
#include "vip_only_dungeon_config.h"
#include "config_reader.h"

IMPLEMENT_SINGLETON(VIPOnlyDungeonConfig)

VIPOnlyDungeonConfig::VIPOnlyDungeonConfig()
{
}

VIPOnlyDungeonConfig::~VIPOnlyDungeonConfig()
{
    Clear();
}

int32 VIPOnlyDungeonConfig::GetSoulValue(bool isWin)
{
    return isWin ? m_winSoulValue : m_loseSoulValue;
}

bool VIPOnlyDungeonConfig::HasVIPConfig(int level) const
{
    VIPOnlyDungeonCfgMap::const_iterator i = m_vipOnlyDungeonCfgMap.find(level);
    return i != m_vipOnlyDungeonCfgMap.end();
}

const VIP_ONLY_DUNGEON_CFG* VIPOnlyDungeonConfig::GetVIPConfig(int level) const
{
    VIPOnlyDungeonCfgMap::const_iterator i = m_vipOnlyDungeonCfgMap.find(level);
    if (i != m_vipOnlyDungeonCfgMap.end())
    {
        return i->second;
    }
    return NULL;
}

bool VIPOnlyDungeonConfig::LoadConfig(const char* path)
{
    TiXmlDocument xmlDoc;

    if (!xmlDoc.LoadFile(path))
    {
        CnError("Load vip only dungeon file: %s failed\n", path);
        return false;
    }

    TiXmlElement *xmlRoot = xmlDoc.RootElement();
    if (!xmlRoot)
    {
        return false;
    }

    TiXmlElement *xmlElement = xmlRoot->FirstChildElement("dungeon");
    ConfigReader::Read(xmlElement, "id", m_dungeonID);

    xmlElement = xmlRoot->FirstChildElement("soul");
    ConfigReader::Read(xmlElement, "win", m_winSoulValue);
    ConfigReader::Read(xmlElement, "lose", m_loseSoulValue);

    int level;

    xmlElement = xmlRoot->FirstChildElement("vip");
    while (xmlElement)
    {
        ConfigReader::Read(xmlElement, "level", level);

        VIP_ONLY_DUNGEON_CFG *cfg;
        VIPOnlyDungeonCfgMap::iterator i = m_vipOnlyDungeonCfgMap.find(level);
        if (i != m_vipOnlyDungeonCfgMap.end())
        {
            cfg = i->second;
        }
        else
        {
            cfg = CnNew VIP_ONLY_DUNGEON_CFG();
            cfg->level = level;
            m_vipOnlyDungeonCfgMap[level] = cfg;
        }

        ConfigReader::Read(xmlElement, "max_pay_count", cfg->max_pay_count);
        ConfigReader::Read(xmlElement, "max_free_count", cfg->max_free_count);
        ConfigReader::Read(xmlElement, "money_point_cost", cfg->money_point_cost);
        ConfigReader::Read(xmlElement, "discount", cfg->discount);

        xmlElement = xmlElement->NextSiblingElement("vip");
    }

    return true;
}

void VIPOnlyDungeonConfig::Clear()
{
    VIPOnlyDungeonCfgMap::iterator i, iEnd = m_vipOnlyDungeonCfgMap.end();
    for (i = m_vipOnlyDungeonCfgMap.begin(); i != iEnd; ++i)
    {
        CnDelete i->second;
    }
    m_vipOnlyDungeonCfgMap.clear();
}