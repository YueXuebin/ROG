//背包格开启花费
#include "gameholder_pch.h"
#include "bag_grid_open_cost_config.h"
#include "config_reader.h"

IMPLEMENT_SINGLETON(BagGridOpenCostConfig)

BagGridOpenCostConfig::BagGridOpenCostConfig()
{
}

BagGridOpenCostConfig::~BagGridOpenCostConfig()
{
    Clear();
}

bool BagGridOpenCostConfig::isValidGridIdx(uint32 idx) const
{
    return idx < mGridCostDataList.size();
}

uint32 BagGridOpenCostConfig::GetGridMaxIdx() const
{
    return mGridCostDataList.size() - 1;
}

uint32 BagGridOpenCostConfig::GetGridOpenCost(uint32 beginIdx, uint32 endIdx) const
{
    if (beginIdx > endIdx)
    {
        std::swap(beginIdx, endIdx);
    }

    uint32 cost = 0;

    uint32 length = Crown::SDMin(endIdx, GetGridMaxIdx());
    for (uint32 i = beginIdx; i <= length; ++i)
    {
        cost += mGridCostDataList[i].moneyPointCost;
    }

    return cost;
}

bool BagGridOpenCostConfig::LoadConfig(const char* path)
{
    TiXmlDocument xmlDoc;

    if (!xmlDoc.LoadFile(path))
    {
        CnError("Load bag grid open cost file: %s failed\n", path);
        return false;
    }

    TiXmlElement *xmlRoot = xmlDoc.RootElement();

    if (!xmlRoot)
    {
        return false;
    }

    TiXmlElement *xmlElement = xmlRoot->FirstChildElement();
    uint32 idx = 0;
    while (xmlElement)
    {
        GridCostData data;
        data.idx = idx++;
        ConfigReader::Read(xmlElement, "money_point_cost", data.moneyPointCost);
        mGridCostDataList.push_back(data);
        xmlElement = xmlElement->NextSiblingElement();
    }

    return true;
}

void BagGridOpenCostConfig::Clear()
{
    mGridCostDataList.clear();
}
