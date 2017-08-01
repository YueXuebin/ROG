#include "gameholder_pch.h"
#include "stone_turn_gold_config.h"
#include "player.h"

IMPLEMENT_SINGLETON(StoneTurnGoldConfig)

StoneTurnGoldConfig::StoneTurnGoldConfig()
{
    
}

StoneTurnGoldConfig::~StoneTurnGoldConfig()
{
    
}

bool StoneTurnGoldConfig::LoadConfig(const char* path)
{
    if(!LoadStoneTurnGoldConfig(path))
    {
        CnFatal("load %s failed", path);
        return false;
    }

    return true;
}

bool StoneTurnGoldConfig::LoadStoneTurnGoldConfig(const char* path)
{
    StoneTurnGoldCost stoneTurnGoldCost;

    TiXmlDocument xmlDoc;

    if (!xmlDoc.LoadFile(path))
    {
        CnError("Load stone_turn_gold file: %s failed\n", path);
        return false;
    }

    TiXmlElement* xmlRoot = xmlDoc.RootElement();
    if(!xmlRoot) return false;

    //服务器配置信息
    for(TiXmlElement* costEle = xmlRoot->FirstChildElement(); costEle; costEle = costEle->NextSiblingElement())
    {
        if(strcmp(costEle->Value(), "turngold") == 0)
        {
            stoneTurnGoldCost.times = SDAtoi(costEle->Attribute("times"));
            stoneTurnGoldCost.money_point = SDAtoi(costEle->Attribute("money_point"));
            m_stoneTurnGoldCost.push_back(stoneTurnGoldCost);
        }
    }

    return true;
}

//int32 StoneTurnGoldConfig::GetCostMoneyPoint(int32 times)
//{
//    uint32 len = (uint32)m_stoneTurnGoldCost.size();
//
//    if(times < 0) return 0;
//
//    // 返回
//    if(times < (int32)len)
//    {
//        return m_stoneTurnGoldCost[times].money_point;
//    }
//
//    // 保证数组中一定有至少一个元素
//    if(len > 0)
//    {
//        return m_stoneTurnGoldCost[len - 1].money_point;
//    }
//
//    return 0;
//}
int32 StoneTurnGoldConfig::GetCostMoneyPoint(int32 times)
{
    std::vector<StoneTurnGoldCost>::iterator veciter;
    for (veciter = m_stoneTurnGoldCost.begin(); veciter != m_stoneTurnGoldCost.end(); veciter++)
    {

        if(veciter->times == times)
        {
            return veciter->money_point;
        }
    }

    uint32 len = (uint32)m_stoneTurnGoldCost.size();
    if(len > 0)
    {
        return m_stoneTurnGoldCost[len - 1].money_point;
    }

    return 0;
}

int32 StoneTurnGoldConfig::GetMaxLen()
{
     uint32 len = (uint32)m_stoneTurnGoldCost.size();

    return len;
}



