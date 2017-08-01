#include "gameholder_pch.h"
#include "goldtree_config.h"

IMPLEMENT_SINGLETON(GoldtreeConfig)

GoldtreeConfig::GoldtreeConfig()
{
    m_levelGamepoint.clear();
    m_cdTime.clear();
    m_moneyPoint = 0;
    m_enterCount = 0;
}

GoldtreeConfig::~GoldtreeConfig()
{

}

bool GoldtreeConfig::LoadGoldtreeConfig(const char* path)
{
    TiXmlDocument xmlDoc;

    if (!xmlDoc.LoadFile(path))
    {
        CnError("Load treasury file: %s failed\n", path);
        return false;
    }

    TiXmlElement* xmlRoot = xmlDoc.RootElement();
    if(!xmlRoot) return false;

    for(TiXmlElement* xmlEle = xmlRoot->FirstChildElement(); xmlEle; xmlEle = xmlEle->NextSiblingElement())
    {
        if(strcmp(xmlEle->Value(), "base") == 0)
        {
            xmlEle->QueryUnsignedAttribute("money", &m_moneyPoint);
            xmlEle->QueryIntAttribute("enter_count", &m_enterCount);
        }
        else if(strcmp(xmlEle->Value(), "level_reward") == 0)
        {
            LevelInfo leveInfo;
            for(TiXmlElement* rewardEle = xmlEle->FirstChildElement(); rewardEle; rewardEle = rewardEle->NextSiblingElement())
            {
                leveInfo.clear();
                rewardEle->QueryUnsignedAttribute("level", &leveInfo.level);
                rewardEle->QueryUnsignedAttribute("gamepoint", &leveInfo.gamepoint);
                rewardEle->QueryUnsignedAttribute("boss_hp", &leveInfo.boss_hp);

                if(m_levelGamepoint.find(leveInfo.level) != m_levelGamepoint.end())
                {
                    CnFatal("level: %d reward is exist!\n", leveInfo.level);
                    continue;
                }

                m_levelGamepoint[leveInfo.level] = leveInfo;
            }
        }
        else if(strcmp(xmlEle->Value(), "cds") == 0)
        {
            for(TiXmlElement* cdsEle = xmlEle->FirstChildElement(); cdsEle; cdsEle = cdsEle->NextSiblingElement())
            {
                uint32 time = 0;
                uint32 id = 0;
                cdsEle->QueryUnsignedAttribute("id", &id);
                cdsEle->QueryUnsignedAttribute("time", &time);

                if(m_cdTime.find(id) != m_cdTime.end())
                {
                    CnFatal("Fatal: cd: %d  is exist!\n", id);
                    continue;
                }

                m_cdTime[id] = time;
            }
        }
    }
    return true;
}

uint32 GoldtreeConfig::GetCDTime(uint32 times)
{
    std::map<uint32, uint32>::iterator cdIt = m_cdTime.find(times);

    if(cdIt == m_cdTime.end())
    {
        //CnFatal("No. %d cd time is no exist!\n", times);
        //return 0;
        std::map<uint32, uint32>::reverse_iterator iterres;
        iterres = m_cdTime.rbegin();
        return iterres->second;
    }
  
    return cdIt->second;
}

uint32 GoldtreeConfig::GetGamePointByLevel(uint32 level)
{
    std::map<uint32, LevelInfo>::iterator it = m_levelGamepoint.find(level);
    // 没找到这一等级的金币奖励
    if(it == m_levelGamepoint.end())
    {
        CnFatal("level: %d reward is no exist!\n", level);
        return 0;
    }

    return it->second.gamepoint;
}

uint32 GoldtreeConfig::GetBossHpByLevel(uint32 level)
{
    std::map<uint32, LevelInfo>::iterator it = m_levelGamepoint.find(level);
    // 没找到这一等级的金币奖励
    if(it == m_levelGamepoint.end())
    {
        CnFatal("level: %d hp is no exist!\n", level);
        return 0;
    }

    return it->second.boss_hp;
}

uint32 GoldtreeConfig::GetGoldtreeCount()
{
    return m_enterCount;
}

