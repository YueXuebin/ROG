#include "gameholder_pch.h"
#include "treasury_config.h"

IMPLEMENT_SINGLETON(TreasuryConfig)

TreasuryConfig::TreasuryConfig()
{
    m_levelGamepoint.clear();
    m_cdTime.clear();
    m_moneyPoint = 0;
}

TreasuryConfig::~TreasuryConfig()
{

}

bool TreasuryConfig::LoadTreasuryConfig(const char* path)
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
            for(TiXmlElement* rewardEle = xmlEle->FirstChildElement(); rewardEle; rewardEle = rewardEle->NextSiblingElement())
            {
                uint32 level = 0;
                uint32 gamepoint = 0;
                rewardEle->QueryUnsignedAttribute("level", &level);
                rewardEle->QueryUnsignedAttribute("gamepoint", &gamepoint);

                if(m_levelGamepoint.find(level) != m_levelGamepoint.end())
                {
                    CnFatal("level: %d reward is exist!\n", level);
                    continue;
                }

                m_levelGamepoint[level] = gamepoint;
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

uint32 TreasuryConfig::GetReward(uint32 level)
{
    std::map<uint32, uint32>::iterator gamepoint = m_levelGamepoint.find(level);

    // 没找到这一等级的金币奖励
    if(gamepoint == m_levelGamepoint.end())
    {
        CnFatal("level: %d reward is no exist!\n", level);
        return 0;
    }

    return gamepoint->second;
}

uint32 TreasuryConfig::GetCDTime(uint32 times)
{
    std::map<uint32, uint32>::iterator cdIt = m_cdTime.find(times);

    if(cdIt == m_cdTime.end())
    {
       // CnFatal("No. %d cd time is no exist!\n", times);
        std::map<uint32, uint32>::reverse_iterator iterres;
        iterres = m_cdTime.rbegin();
        return iterres->second;
        //return 0;
    }

    return cdIt->second;
}

uint32 TreasuryConfig::GetTreasuryCount()
{
    return m_enterCount;
}

