#include "gameholder_pch.h"
#include "limitbuy_config.h"
IMPLEMENT_SINGLETON(LimitBuyConfig)
LimitBuyConfig::LimitBuyConfig(void)
{
}

LimitBuyConfig::~LimitBuyConfig(void)
{
}
bool LimitBuyConfig::LoadConfig(const char* path)
{
    TiXmlDocument xmlDoc;

    if (!xmlDoc.LoadFile(path))
    {
        CnError("Load item file: %s failed\n", path);
        return false;
    }

    TiXmlElement* xmlRoot = xmlDoc.RootElement();
    if(!xmlRoot) return false;

    for(TiXmlElement* productEle = xmlRoot->FirstChildElement(); productEle; productEle = productEle->NextSiblingElement())
    {
       uint32 id = 0;
       productEle->QueryUnsignedAttribute("product_id", &id);

       LimitBuyTemp limitbuytemp;
       limitbuytemp.Clear();
       productEle->QueryIntAttribute("product_num", &limitbuytemp.num);
       productEle->QueryIntAttribute("type", &limitbuytemp.type);

       if(id == 0) continue;
       if(m_buyLimitList.find(id)  == m_buyLimitList.end())
       {
           m_buyLimitList[id] = limitbuytemp;
       }
       else
       {
            CnWarn("Duplicate id: %u\n", id);
       }
    }
    return true;
}
LimitBuyTemp* LimitBuyConfig::FindLimitTemp(int32 productid)
{
    std::map<uint32,LimitBuyTemp>::iterator limittemp;
     limittemp = m_buyLimitList.find(productid);
     if(limittemp != m_buyLimitList.end())
         return &(limittemp->second);
     return 0;
}