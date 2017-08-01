#include "gameholder_pch.h"
#include "horoscope_config.h"
#include "player.h"

IMPLEMENT_SINGLETON(HoroscopeConfig)

HoroscopeConfig::HoroscopeConfig()
{
    // 初始化
    m_max_horoscope_level= 0;
}

HoroscopeConfig::~HoroscopeConfig()
{
    
}

// ==================================================
// 读取星空图配置
// ==================================================
bool HoroscopeConfig::LoadConfig(const char* path)
{
    HoroscopeTemplate horoscopeInfo;
    ConstellationTemplate constellationTmpl;
    Property constellationProp;
    int32 horoscope_level = 0;

    TiXmlDocument xmlDoc;

    if (!xmlDoc.LoadFile(path))
    {
        CnError("Load equipment file: %s failed\n", path);
        return false;
    }

    TiXmlElement* xmlRoot = xmlDoc.RootElement();
    if(!xmlRoot) return false;

    for(TiXmlElement* horoscopeEle = xmlRoot->FirstChildElement(); horoscopeEle; horoscopeEle = horoscopeEle->NextSiblingElement())
    {
        horoscopeInfo.Clear();
        horoscopeInfo.id = Crown::SDAtou(horoscopeEle->Attribute("id"));
        horoscopeInfo.max_level = Crown::SDAtou(horoscopeEle->Attribute("max_level"));

        m_max_horoscope_level += horoscopeInfo.max_level;

        if(horoscopeInfo.id == 12)
        {
            m_max_bronze_level = m_max_horoscope_level;
        }
        else if(horoscopeInfo.id == 24)
        {
            m_max_silver_level = m_max_horoscope_level;
        }
        else if(horoscopeInfo.id == 36)
        {
            m_max_gold_level = m_max_horoscope_level;
        }

        for(TiXmlElement* constellationEle = horoscopeEle->FirstChildElement(); 
            constellationEle; constellationEle = constellationEle->NextSiblingElement())
        {
            constellationTmpl.Clear();
            constellationTmpl.level = Crown::SDAtou(constellationEle->Attribute("level"));
            constellationTmpl.game_point = SDAtoi(constellationEle->Attribute("game_point"));
            constellationTmpl.soul_point = SDAtoi(constellationEle->Attribute("soul_point"));
            constellationTmpl.time = Crown::SDAtou(constellationEle->Attribute("time"));
            constellationTmpl.player_level = Crown::SDAtou(constellationEle->Attribute("player_level"));
            constellationTmpl.start_point = Crown::SDAtou(constellationEle->Attribute("star_point"));

            std::string props = constellationEle->Attribute("prop");
            std::vector<std::string> values = Crown::SDSplitStrings(props, '|');
            for(uint32 i=0; i<values.size(); ++i)
            {
                std::vector<std::string> key_value = Crown::SDSplitStrings(values[i], '-');
                constellationProp.Clear();
                if(key_value.size() == 2)
                {
                    constellationProp.type = SDAtoi(key_value[0].c_str());
                    constellationProp.value = SDAtoi(key_value[1].c_str());
                    constellationTmpl.props.push_back(constellationProp);
                }
            }

            ++horoscope_level;

            if(m_constellation_map.find(horoscope_level) == m_constellation_map.end())
            {
                m_constellation_map[horoscope_level] = constellationTmpl;
            }
            else
            {
                CnWarn("Duplicate constellation level: %u\n", horoscope_level);
            }
        }
    }

    //LoadHoroscopeLimit();

    return true;
}

// ==================================================
// 加载星空图限制
// ==================================================
void HoroscopeConfig::LoadHoroscopeLimit()
{
    HoroscopeLimit horoscopeLimit;

    horoscopeLimit.horoLowLevel = 0;
    horoscopeLimit.horoHighLevel = 35;
    horoscopeLimit.playerLevel = 15;
    m_horoscopeLimit.push_back(horoscopeLimit);

    horoscopeLimit.horoLowLevel = 36;
    horoscopeLimit.horoHighLevel = 93;
    horoscopeLimit.playerLevel = 20;
    m_horoscopeLimit.push_back(horoscopeLimit);

    horoscopeLimit.horoLowLevel = 94;
    horoscopeLimit.horoHighLevel = 128;
    horoscopeLimit.playerLevel = 25;
    m_horoscopeLimit.push_back(horoscopeLimit);

    horoscopeLimit.horoLowLevel = 129;
    horoscopeLimit.horoHighLevel = 185;
    horoscopeLimit.playerLevel = 30;
    m_horoscopeLimit.push_back(horoscopeLimit);

    horoscopeLimit.horoLowLevel = 186;
    horoscopeLimit.horoHighLevel = 221;
    horoscopeLimit.playerLevel = 35;
    m_horoscopeLimit.push_back(horoscopeLimit);

    horoscopeLimit.horoLowLevel = 222;
    horoscopeLimit.horoHighLevel = 278;
    horoscopeLimit.playerLevel = 40;
    m_horoscopeLimit.push_back(horoscopeLimit);

    horoscopeLimit.horoLowLevel = 279;
    horoscopeLimit.horoHighLevel = 336;
    horoscopeLimit.playerLevel = 50;
    m_horoscopeLimit.push_back(horoscopeLimit);

    horoscopeLimit.horoLowLevel = 337;
    horoscopeLimit.horoHighLevel = 394;
    horoscopeLimit.playerLevel = 60;
    m_horoscopeLimit.push_back(horoscopeLimit);

    horoscopeLimit.horoLowLevel = 395;
    horoscopeLimit.horoHighLevel = 417;
    horoscopeLimit.playerLevel = 70;
    m_horoscopeLimit.push_back(horoscopeLimit);
}

// ==================================================
// 根据星空图等级，获得星座模板
// ==================================================
const ConstellationTemplate* HoroscopeConfig::GetConstellation(int32 level)
{
    std::map<int32, ConstellationTemplate>::iterator constellationIt;
    constellationIt = m_constellation_map.find(level);
    if(constellationIt != m_constellation_map.end())
    {
        return &(constellationIt->second);
    }
    return 0;
}

int32 HoroscopeConfig::GetMaxHoroscopeLevel()
{
    return m_max_horoscope_level;
}

///< 根据角色的星空图等级，返回模板
void HoroscopeConfig::GetConstellationProps(int32 level, std::vector<const ConstellationTemplate*>& list)
{
    const ConstellationTemplate* tmpl = 0;

    list.clear();

    for(int32 i=1; i<=level; ++i)
    {
        tmpl = GetConstellation(i);
        if(tmpl)
        {
            list.push_back(tmpl);
        }
    }
}

bool HoroscopeConfig::CheckHoroscopeLevel(int32 HoroscopeLevel, int32 playerLevel)
{
    std::vector<HoroscopeLimit>::iterator it;

    for(it=m_horoscopeLimit.begin(); it!=m_horoscopeLimit.end(); ++it)
    {
        if((it->horoLowLevel <= HoroscopeLevel) && (HoroscopeLevel <= it->horoHighLevel))
        {
            break;
        }
    }

    if(it == m_horoscopeLimit.end())
        return false;

    if(playerLevel < it->playerLevel)
        return false;

    return true;
}
