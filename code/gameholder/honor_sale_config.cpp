#include "gameholder_pch.h"
#include "honor_sale_config.h"
#include "item_config_manager.h"

IMPLEMENT_SINGLETON(HonorSaleConfig)

HonorSaleConfig::HonorSaleConfig()
{
    //m_sale_map.clear();
}

HonorSaleConfig::~HonorSaleConfig()
{

}

bool HonorSaleConfig::LoadConfig(const char* path)
{
    TiXmlDocument xmlDoc;
    //const ItemTemplate* itemTmpl;
    uint32 itemID = 0;

    if (!xmlDoc.LoadFile(path))
    {
        CnError("Load item file: %s failed\n", path);
        return false;
    }

    TiXmlElement* xmlRoot = xmlDoc.RootElement();
    if(!xmlRoot) return false;

    for(TiXmlElement* saleEle = xmlRoot->FirstChildElement(); saleEle; saleEle = saleEle->NextSiblingElement())
    {
        itemID = 0;
        saleEle->QueryUnsignedAttribute("item_id", &itemID);
        //itemTmpl = ItemConfig::Instance()->GetItemTmpl(itemID);
        //if(!itemTmpl)
        //{
        //    CnWarn("Honor buy itemID:%u can't find \n", itemID);
        //    continue;
        //}

        //if(m_sale_map.find(itemID) != m_sale_map.end())
        //{
        //    CnWarn("Duplicate itemID: %u\n", itemID);
        //    continue;
        //}
        //m_sale_map[itemID] = itemTmpl;
    }

    return true;
}

//const struct ItemTemplate* HonorSaleConfig::GetSaleItem(uint32 itemID)
//{
//    SaleMap::iterator saleIt = m_sale_map.find(itemID);
//    if(saleIt != m_sale_map.end())
//    {
//        return saleIt->second;
//    }
//    return NULL;
//}

