#include "centerserver_pch.h"
#include "multserver_config.h"

IMPLEMENT_SINGLETON(MultserverConfig)

MultserverConfig::MultserverConfig()
{
    m_rewardMap.clear();
}

MultserverConfig::~MultserverConfig()
{

}

bool MultserverConfig::LoadConfig(const char* fileName)
{
    //std::string fileName = RES_SDB_PATH""MULTISERVER_BATTLE_FILE;

    TiXmlDocument xmlDoc;

    if(!xmlDoc.LoadFile(fileName))
    {
        CnError("Load %s file failed\n", fileName);
        return false;
    }

    TiXmlElement* xmlRoot = xmlDoc.RootElement();
    if(!xmlRoot) 
    {
        CnError("Load %s file failed\n", fileName);
        return false;
    }

    int32 rank = 0;
    int32 season_point = 0;
    for(TiXmlElement* seasonEle = xmlRoot->FirstChildElement(); seasonEle; seasonEle = seasonEle->NextSiblingElement())
    {
        if(Crown::SDStrcmp(seasonEle->Value(), "season_reward") == 0)
        {
            for(TiXmlElement* rewardEle = seasonEle->FirstChildElement(); rewardEle; rewardEle = rewardEle->NextSiblingElement())
            {
                if(Crown::SDStrcmp(rewardEle->Value(), "reward") == 0)
                {
                    rewardEle->QueryIntAttribute("rank", &rank);
                    rewardEle->QueryIntAttribute("season_point", &season_point);
                    m_rewardMap[rank] = season_point;
                }
            }
        }
    }

    return true;
}

int32 MultserverConfig::GetRewardByRank(int32 rank)
{
    for(std::map<int32, int32>::reverse_iterator rankIt = m_rewardMap.rbegin(); rankIt != m_rewardMap.rend(); ++rankIt)
    {
        if(rank >= rankIt->first)
            return rankIt->second;
    }

    return 0;
}

