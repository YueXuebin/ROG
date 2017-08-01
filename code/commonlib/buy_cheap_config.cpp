#include "commonlib_pch.h"
#include "buy_cheap_config.h"


IMPLEMENT_SINGLETON(BuyCheapConfig)

BuyCheapConfig::BuyCheapConfig()
{
    m_buyCheapsTmp.Clear();
}

BuyCheapConfig::~BuyCheapConfig()
{

}

bool BuyCheapConfig::LoadConfig(const char* path)
{
    TiXmlDocument xmlDoc;

    if (!xmlDoc.LoadFile(path))
    {
        CnError("Load buy cheap file: %s failed\n", path);
        return false;
    }

    TiXmlElement* xmlRoot = xmlDoc.RootElement();
    if(!xmlRoot) return false;

    for(TiXmlElement* xmlEle = xmlRoot->FirstChildElement(); xmlEle; xmlEle = xmlEle->NextSiblingElement())
    {
        if(strcmp(xmlEle->Value(), "buy_cheaps") == 0)
        {
            for (TiXmlElement* buyCheapEle = xmlEle->FirstChildElement(); buyCheapEle; buyCheapEle = buyCheapEle->NextSiblingElement())
            {
                BUY_CHEAPS_ITEM buyCheapsItem;
                buyCheapEle->QueryIntAttribute("day", &buyCheapsItem.day);
                buyCheapEle->QueryUnsignedAttribute("discount_item_id", &buyCheapsItem.discount_item_id);
                buyCheapEle->QueryIntAttribute("discount_item_num", &buyCheapsItem.discount_item_num);
                buyCheapEle->QueryUnsignedAttribute("ultimate_item_id", &buyCheapsItem.ultimate_item_id);
                buyCheapEle->QueryIntAttribute("ultimate_item_num", &buyCheapsItem.ultimate_item_num);
                buyCheapEle->QueryIntAttribute("original_cost", &buyCheapsItem.original_cost);
                buyCheapEle->QueryIntAttribute("discount_1", &buyCheapsItem.discount_1);
                buyCheapEle->QueryIntAttribute("discount_2", &buyCheapsItem.discount_2);
                buyCheapEle->QueryIntAttribute("discount_3", &buyCheapsItem.discount_3);
                buyCheapEle->QueryIntAttribute("discount_4", &buyCheapsItem.discount_4);
                m_buyCheapsTmp.buy_cheaps.push_back(buyCheapsItem);
            }
        }
    }

    return true;
}

bool BuyCheapConfig::DBSetConfig(std::string buyCheaps, Json::Value* jsonValue)
{
    m_buyCheapsTmp.Clear();
    if(!JsonToStruct(m_buyCheapsTmp, buyCheaps, jsonValue))
    {
        CnError("buy cheap db json to sturct error!");
        return false;
    }

    return true;
}

const BUY_CHEAPS_ITEM* BuyCheapConfig::GetBuyCheapByDay(int32 day)
{
    int32 index = day % m_buyCheapsTmp.buy_cheaps.size();

    for(std::vector<BUY_CHEAPS_ITEM>::iterator it = m_buyCheapsTmp.buy_cheaps.begin(); it != m_buyCheapsTmp.buy_cheaps.end(); ++it)
    {
        if(it->day == index)
            return &(*it);
    }

    return NULL;
}

