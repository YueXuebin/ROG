#include "gameholder_pch.h"
#include "guild_sale_config.h"
#include "item_config_manager.h"

IMPLEMENT_SINGLETON(GuildSaleConfig)

GuildSaleConfig::GuildSaleConfig()
{
    //m_sale_map.clear();
}

GuildSaleConfig::~GuildSaleConfig()
{

}

bool GuildSaleConfig::LoadConfig(const char* path)
{
    TiXmlDocument xmlDoc;
    //const ItemTemplate* itemTmpl;
    uint32 saleID = 0;
    uint32 itemID = 0;
    //SaleList saleList;

    if (!xmlDoc.LoadFile(path))
    {
        CnError("Load item file: %s failed\n", path);
        return false;
    }

    TiXmlElement* xmlRoot = xmlDoc.RootElement();
    if(!xmlRoot) return false;

    //服务器配置信息
    for(TiXmlElement* saleEle = xmlRoot->FirstChildElement(); saleEle; saleEle = saleEle->NextSiblingElement())
    {
        saleID = Crown::SDAtou(saleEle->Attribute("id"));
        //saleList.clear();

        for(TiXmlElement* itemEle = saleEle->FirstChildElement(); itemEle; itemEle = itemEle->NextSiblingElement())
        {
            itemID = Crown::SDAtou(itemEle->Attribute("id"));
            //itemTmpl = ItemConfig::Instance()->GetItemTmpl(itemID);
            //if(!itemTmpl)
            //{
            //    CnWarn("SaleID:%u can't find itemID:%u\n", saleID, itemID);
            //    continue;
            //}

            //saleList.push_back(itemTmpl);
        }

        //if(m_sale_map.find(saleID) == m_sale_map.end())
        //{
        //    m_sale_map[saleID] = saleList;
        //}
        //else
        //{
        //    CnWarn("Duplicate saleID: %u\n", saleID);
        //}
    }

    return true;
}

//const ItemTemplate* GuildSaleConfig::GetSaleItem(uint32 saleID, uint32 itemID)
//{
//    std::map<uint32, SaleList>::iterator saleIt;
//    saleIt = m_sale_map.find(saleID);
//
//    if(saleIt != m_sale_map.end())
//    {
//        for(SaleList::iterator itemIt = saleIt->second.begin(); 
//            itemIt != saleIt->second.end(); ++itemIt)
//        {
//            if((*itemIt)->id == itemID)
//            {
//                return *itemIt;
//            }
//        }
//    }
//
//    return nullptr;
//}
