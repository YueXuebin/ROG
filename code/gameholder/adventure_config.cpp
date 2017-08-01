#include "gameholder_pch.h"
#include "adventure_config.h"
#include "game_util.h"

IMPLEMENT_SINGLETON(AdventureConfig)

AdventureConfig::AdventureConfig()
{
    m_refresh_time = 0;
    m_max_adventure_times = 0;
    m_max_rob_times = 0;
    m_adventures.clear();
}

AdventureConfig::~AdventureConfig()
{

}

bool AdventureConfig::LoadConfig( const char* path )
{
    AdventureTemplate adventureTmpl;
    REWARD_ITEM rewardItem;
    TiXmlDocument xmlDoc;

    if (!xmlDoc.LoadFile(path))
    {
        CnError("Load monster file: %s failed\n", path);
        return false;
    }

    TiXmlElement* xmlRoot = xmlDoc.RootElement();
    if(!xmlRoot) return false;

    for(TiXmlElement* xmlEle = xmlRoot->FirstChildElement(); xmlEle; xmlEle = xmlEle->NextSiblingElement())
    {
        if(strcmp(xmlEle->Value(), "refresh_time") == 0)
        {
            m_refresh_time = Crown::SDAtou(xmlEle->Attribute("value"));
        }
        else if(strcmp(xmlEle->Value(), "max_adventure_times") == 0)
        {
            m_max_adventure_times = SDAtoi(xmlEle->Attribute("value"));
        }
        else if(strcmp(xmlEle->Value(), "max_rob_times") == 0)
        {
            m_max_rob_times = SDAtoi(xmlEle->Attribute("value"));
        }
        else if(strcmp(xmlEle->Value(), "adventures") == 0)
        {
            for(TiXmlElement* advEle = xmlEle->FirstChildElement(); advEle; advEle = advEle->NextSiblingElement())
            {
                adventureTmpl.Clear();
                adventureTmpl.id = Crown::SDAtou(advEle->Attribute("map_id"));
                adventureTmpl.name = advEle->Attribute("name");
                adventureTmpl.take_time = Crown::SDAtou(advEle->Attribute("take_time"));

                for(TiXmlElement* itEle = advEle->FirstChildElement(); itEle; itEle = itEle->NextSiblingElement())
                {
                    if(strcmp(itEle->Value(), "rewards") != 0) continue;

                    rewardItem.Clear();

                    for(TiXmlElement* rewardEle = itEle->FirstChildElement(); rewardEle; rewardEle = rewardEle->NextSiblingElement())
                    {
                        // 加载奖励项目
                        if(!LoadRewardItem(rewardEle, rewardItem))
                        {
                            //CnWarn("Error adventure:%u, reward type:%d\n", adventureTmpl.id, rewardItem.type);
                        }
                        adventureTmpl.reward_list.push_back(rewardItem);
                    }
                }

                if(m_adventures.find(adventureTmpl.id) == m_adventures.end())
                {
                    m_adventures[adventureTmpl.id] = adventureTmpl;
                }
                else
                {
                    CnWarn("Duplicate adventure id: %u\n", adventureTmpl.id);
                }
            }
        }
    }

    return true;
}

const AdventureTemplate*  AdventureConfig::GetAdventureTemplate(uint32 mapID)
{
    if( m_adventures.find(mapID) == m_adventures.end() )
        return NULL;        // 该地图模板Id不存在

    return &m_adventures[mapID];
}