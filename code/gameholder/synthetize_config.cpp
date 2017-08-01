#include "gameholder_pch.h"
#include "synthetize_config.h"
#include "player.h"

IMPLEMENT_SINGLETON(SynthetizeConfig)

SynthetizeConfig::SynthetizeConfig()
{
    cirt_time = 0;
}

SynthetizeConfig::~SynthetizeConfig()
{
    
}

bool SynthetizeConfig::LoadConfig(const char* path)
{
    TiXmlDocument xmlDoc;

    if (!xmlDoc.LoadFile(path))
    {
        CnError("Load equipment file: %s failed\n", path);
        return false;
    }

    TiXmlElement* xmlRoot = xmlDoc.RootElement();
    if(!xmlRoot) return false;

    for(TiXmlElement* childEle = xmlRoot->FirstChildElement(); childEle; childEle = childEle->NextSiblingElement())
    {
        if(Crown::SDStrcmp(childEle->Value(), "cirt") == 0)
        {
           cirt_time = SDAtoi(childEle->Attribute("time"));
        }
        if(Crown::SDStrcmp(childEle->Value(), "public") == 0)
        {
           m_public = childEle->Attribute("public");
        }
        if(Crown::SDStrcmp(childEle->Value(), "jewel_level_limit") == 0)
        {
            for(TiXmlElement* xmlEle = childEle->FirstChildElement(); xmlEle; xmlEle = xmlEle->NextSiblingElement())
            {
                int32 jewel_level = SDAtoi(xmlEle->Attribute("jewel_level"));
                int32 player_level = SDAtoi(xmlEle->Attribute("player_level"));

                if(m_use_jewel_level.find(jewel_level) == m_use_jewel_level.end())
                {
                    m_use_jewel_level[jewel_level] = player_level;
                }
                else
                {
                    CnWarn("Repeat jewel_level_limit: %d\n", jewel_level);
                }
            }
        }
        //else if(Crown::SDStrcmp(childEle->Value(), "jewel_synthetize_limit") == 0)
        //{
        //    for(TiXmlElement* xmlEle = childEle->FirstChildElement(); xmlEle; xmlEle = xmlEle->NextSiblingElement())
        //    {
        //        int32 jewel_level = SDAtoi(xmlEle->Attribute("jewel_level"));
        //        int32 player_level = SDAtoi(xmlEle->Attribute("player_level"));

        //        if(m_synthetize_jewel_level.find(jewel_level) == m_synthetize_jewel_level.end())
        //        {
        //            m_synthetize_jewel_level[jewel_level] = player_level;
        //        }
        //        else
        //        {
        //            CnWarn("Repeat jewel_synthetize_limit: %d\n", jewel_level);
        //        }
        //    }
        //}
        else if(strcmp(childEle->Value(), "synthetize_jewel") == 0)
        {
            for(TiXmlElement* xmlEle = childEle->FirstChildElement(); xmlEle; xmlEle = xmlEle->NextSiblingElement())
            {
                SynthetizeJewel synthetize;
                synthetize.level = SDAtoi(xmlEle->Attribute("level"));

                for(TiXmlElement* jewelEle = xmlEle->FirstChildElement(); jewelEle; jewelEle = jewelEle->NextSiblingElement())
                {
                    SynthetizeCost cost;
                    cost.game_point = SDAtoi(jewelEle->Attribute("game_point"));
                    cost.lower_item_num = SDAtoi(jewelEle->Attribute("num"));
                    cost.material_item_id = SDAtoi(jewelEle->Attribute("material_item_id"));
                    cost.material_item_num = SDAtoi(jewelEle->Attribute("material_item_num"));
                    cost.critical_ratio = SDAtoi(jewelEle->Attribute("ratio"));
                    synthetize.list.push_back(cost);
                }

                if(m_synthjewel_map.find(synthetize.level) == m_synthjewel_map.end())
                {
                    m_synthjewel_map[synthetize.level] = synthetize;
                }
                else
                {
                    CnDbg("SynthetizeJewel level=%d duplicated\n", synthetize.level);
                }
            }
        }
    }

    return true;
}

bool SynthetizeConfig::CanUseJewel(int32 jewel_level, int32 player_level)
{
    std::map<int32, int32>::iterator levelIt;

    levelIt = m_use_jewel_level.find(jewel_level);
    if(levelIt == m_use_jewel_level.end())
    {
        return false;
    }

    if(player_level < levelIt->second)
    {
        return false;
    }

    return true;
}

//bool SynthetizeConfig::CanSynthetizeJewel(int32 jewel_level, int32 player_level)
//{
//    std::map<int32, int32>::iterator levelIt;
//
//    levelIt = m_synthetize_jewel_level.find(jewel_level);
//    if(levelIt == m_synthetize_jewel_level.end())
//    {
//        return false;
//    }
//
//    if(player_level < levelIt->second)
//    {
//        return false;
//    }
//
//    return true;
//}

SynthetizeJewel* SynthetizeConfig::GetSynthJewel(int32 level)
{
    SynthJewelMap::iterator jewelIt;

    jewelIt = m_synthjewel_map.find(level);
    if(jewelIt != m_synthjewel_map.end())
    {
        return &(jewelIt->second);
    }

    return 0;
}
