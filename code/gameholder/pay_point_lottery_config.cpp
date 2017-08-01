//充值积分抽奖配置文件
#include "gameholder_pch.h"
#include "pay_point_lottery_config.h"
#include "config_reader.h"

IMPLEMENT_SINGLETON(PayPointLotteryConfig)

PayPointLotteryConfig::PayPointLotteryConfig()
{
}

PayPointLotteryConfig::~PayPointLotteryConfig()
{
    Clear();
}

uint32 PayPointLotteryConfig::GetLotteryPackageTID(int32 vipLevel) const
{
    LOTTERY_PACKAGE_LIST::const_iterator i, iEnd = mLotteryPackageList.end();
    for (i = mLotteryPackageList.begin(); i != iEnd; ++i)
    {
        if (vipLevel >= i->vip_min_level && vipLevel <= i->vip_max_level)
        {
            return i->tid;
        }
    }
    return 0;
}

int32 PayPointLotteryConfig::GetLotteryItemIdxByTID(uint32 tid) const
{
    for (uint32 i = 0; i < mLotteryItemList.size(); ++i)
    {
        if (mLotteryItemList[i].tid == tid)
        {
            return i;
        }
    }
    return -1;
}

const PAY_POINT_LOTTERY_ITEM* PayPointLotteryConfig::GetLotteryItemByIdx(int32 idx) const
{
    if (idx < 0 || idx >= int(mLotteryItemList.size()))
    {
        return NULL;
    }
    return &mLotteryItemList[idx];
}

const PAY_POINT_LOTTERY_ITEM* PayPointLotteryConfig::GetLotteryItemByTID(uint32 tid) const
{
    LOTTERY_ITEM_LIST::const_iterator i, iEnd = mLotteryItemList.end();
    for (i = mLotteryItemList.begin(); i != iEnd; ++i)
    {
        if (i->tid == tid)
        {
            return &(*i);
        }
    }
    return NULL;
}

int32 PayPointLotteryConfig::GetExchangeItemIdxByTID(uint32 tid) const
{
    for (uint32 i = 0; i < mExchangeItemList.size(); ++i)
    {
        if (mExchangeItemList[i].tid == tid)
        {
            return i;
        }
    }
    return -1;
}

const PAY_POINT_LOTTERY_ITEM* PayPointLotteryConfig::GetExchangeItemByIdx(int32 idx) const
{
    if (idx < 0 || idx >= int(mExchangeItemList.size()))
    {
        return NULL;
    }
    return &mExchangeItemList[idx];
}

const PAY_POINT_LOTTERY_ITEM* PayPointLotteryConfig::GetExchangeItemByTID(uint32 tid) const
{
    LOTTERY_ITEM_LIST::const_iterator i, iEnd = mExchangeItemList.end();
    for (i = mExchangeItemList.begin(); i != iEnd; ++i)
    {
        if (i->tid == tid)
        {
            return &(*i);
        }
    }
    return NULL;
}

bool PayPointLotteryConfig::isWorldRadioItem(uint32 tid) const
{
    std::vector<unsigned int>::const_iterator i, iEnd = mLotteryWorldRadioItemTIDFilterList.end();
    for (i = mLotteryWorldRadioItemTIDFilterList.begin(); i != iEnd; ++i)
    {
        if ((*i) == tid)
        {
            return true;
        }
    }
    return false;
}

void PayPointLotteryConfig::Clear()
{
    mStartTimeStamp = 0;
    mEndTimeStamp = 0;
    mMaxGetCount = 0;
    mLotteryCostItemTid = 0;
    mLotteryItemList.clear();
    mExchangeItemList.clear();
    mLotteryPackageList.clear();
    mLotteryWorldRadioItemTIDFilterList.clear();
}

bool PayPointLotteryConfig::LoadConfig(const char* path)
{
    TiXmlDocument xmlDoc;

    if (!xmlDoc.LoadFile(path))
    {
        CnError("Load pay point lottery file: %s failed\n", path);
        return false;
    }

    TiXmlElement *xmlRoot = xmlDoc.RootElement();
    if (!xmlRoot)
    {
        return false;
    }

    TiXmlElement *xmlElement = xmlRoot->FirstChildElement("time");
    std::string timeStr;
    ConfigReader::Read(xmlElement, "start", timeStr);
    mStartTimeStamp = Crown::TimeStamp(timeStr);
    ConfigReader::Read(xmlElement, "end", timeStr);
    mEndTimeStamp = Crown::TimeStamp(timeStr);

    xmlElement = xmlRoot->FirstChildElement("exchange_rate");
    ConfigReader::Read(xmlElement, "number", mExchangeRate);

    xmlElement = xmlRoot->FirstChildElement("max_get_count");
    ConfigReader::Read(xmlElement, "number", mMaxGetCount);

    xmlElement = xmlRoot->FirstChildElement("lottery_cost_item");
    ConfigReader::Read(xmlElement, "tid", mLotteryCostItemTid);

    xmlElement = xmlRoot->FirstChildElement("lottery_item");
    for (TiXmlElement *elem = xmlElement->FirstChildElement(); elem; elem = elem->NextSiblingElement())
    {
        if (strcmp(elem->Value(), "item") == 0)
        {
            PAY_POINT_LOTTERY_ITEM item;
            ConfigReader::Read(elem, "tid", item.tid);
            ConfigReader::Read(elem, "color", item.color);
            ConfigReader::Read(elem, "number", item.number);
            ConfigReader::Read(elem, "cost", item.cost);
            mLotteryItemList.push_back(item);
        }
    }

    xmlElement = xmlRoot->FirstChildElement("exchange_item");
    for (TiXmlElement *elem = xmlElement->FirstChildElement(); elem; elem = elem->NextSiblingElement())
    {
        if (strcmp(elem->Value(), "item") == 0)
        {
            PAY_POINT_LOTTERY_ITEM item;
            ConfigReader::Read(elem, "tid", item.tid);
            ConfigReader::Read(elem, "color", item.color);
            ConfigReader::Read(elem, "number", item.number);
            ConfigReader::Read(elem, "cost", item.cost);
            mExchangeItemList.push_back(item);
        }
    }

    xmlElement = xmlRoot->FirstChildElement("lottery_package");
    for (TiXmlElement *elem = xmlElement->FirstChildElement(); elem; elem = elem->NextSiblingElement())
    {
        if (strcmp(elem->Value(), "package") == 0)
        {
            PAY_POINT_LOTTERY_PACKAGE package;
            ConfigReader::Read(elem, "tid", package.tid);
            ConfigReader::Read(elem, "vip_min_level", package.vip_min_level);
            ConfigReader::Read(elem, "vip_max_level", package.vip_max_level);
            mLotteryPackageList.push_back(package);
        }
    }

    xmlElement = xmlRoot->FirstChildElement("lottery_world_radio_filter");
    std::string worldRadioFilter;
    ConfigReader::Read(xmlElement, "list", worldRadioFilter);
    std::vector<std::string> valueList = Crown::SDSplitStrings(worldRadioFilter, '|');
    for (uint32 i = 0; i < valueList.size(); ++i)
    {
        mLotteryWorldRadioItemTIDFilterList.push_back(Crown::SDAtou(valueList[i].c_str()));
    }

    return true;
}