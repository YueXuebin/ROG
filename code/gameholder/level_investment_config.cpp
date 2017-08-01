#include "gameholder_pch.h"
#include "level_investment_config.h"


IMPLEMENT_SINGLETON(LevelInvestmentConfig);

LevelInvestmentConfig::LevelInvestmentConfig()
{
    m_level_investment_map.clear();
    m_level_investment_range_list.clear();
}

LevelInvestmentConfig::~LevelInvestmentConfig()
{

}

bool LevelInvestmentConfig::LoadLevelInvestmentConfig( const char* path )
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

    std::string money_range = xmlRoot->Attribute("money_range");
    std::vector<std::string> ranges = Crown::SDSplitStrings(money_range, ',');
    std::vector<std::string>::iterator iter;
    for (iter = ranges.begin(); iter != ranges.end(); iter++)
    {
        m_level_investment_range_list.push_back(Crown::SDAtou(iter->c_str()));
    }

    bool ret = true;
    for(TiXmlElement* element = xmlRoot->FirstChildElement(); element; element = element->NextSiblingElement())
    {
        ret = ret && LoadNode(element);
    }

    return ret;
}

bool LevelInvestmentConfig::LoadNode(TiXmlElement* element)
{
    if(Crown::SDStrcmp(element->Value(), "investment") == 0)
    {
        uint32 level = 0;
        uint32 reward_percent = 0;
        element->QueryUnsignedAttribute("level", &level);
        element->QueryUnsignedAttribute("reward_percent", &reward_percent);

        m_level_investment_map[level] = reward_percent;
        return true;
    }

    return false;
}

uint32 LevelInvestmentConfig::GetLevelInvestmentRewardPercent(uint32 level)
{
    LevelInvestmentInfoMap::iterator iter = m_level_investment_map.find(level);
    if (iter != m_level_investment_map.end())
    {
        return iter->second;
    }

    return 0;
}

const LevelInvestmentInfoMap* LevelInvestmentConfig::GetLevelInvestmentMap()
{
    return &m_level_investment_map;
}

bool LevelInvestmentConfig::IsMoneyInInvestmentRange( uint32 money )
{
    LevelInvestmentRangeList::iterator iter;
    for (iter = m_level_investment_range_list.begin(); iter != m_level_investment_range_list.end(); iter++)
    {
        if (*iter == money)
            return true;
    }

    return false;
}



