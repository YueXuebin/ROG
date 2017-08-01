#include "gameholder_pch.h"
#include "fashion_config.h"
#include "item_config_manager.h"

IMPLEMENT_SINGLETON(FashionConfig)


FashionConfig::FashionConfig()
{
    m_maxQuality = 0;
}

FashionConfig::~FashionConfig()
{

}
// 分数比较函数
bool SortConfigRules(const FashionLevelUp& left, const FashionLevelUp& right) 
{
    return (left.level < right.level);
}

bool FashionConfig::LoadConfig( const char* path )
{
    char file_path[SD_MAX_PATH];

    SDSprintf(file_path, "%s%s", path, FISHIONS_CONFIG_FILE);
    if(!LoadFashionsConfig(file_path))
    {
        CnFatal("load %s failed\n", file_path);
        return false;
    }

    SDSprintf(file_path, "%s%s", path, FISHIONS_TREASURE_CONFIG_FILE);
    if(!LoadFashionTreasureConfig(file_path))
    {
        CnFatal("load %s failed\n", file_path);
        return false;
    }

    SDSprintf(file_path, "%s%s", path, FISHIONS_SALE_FILE);
    if(!LoadSaleConfig(file_path))
    {
        CnFatal("load %s failed\n", file_path);
        return false;
    }

    return true;
}

//=====================================================
//读取新的化身配置
//=====================================================
bool FashionConfig::LoadFashionsConfig(const char* path)
{
    TiXmlDocument xmlDoc;
    if (!xmlDoc.LoadFile(path))
    {
        CnError("Load npc file: %s failed\n", path);
        return false;
    }
    
    TiXmlElement* xmlRoot = xmlDoc.RootElement();
    if(!xmlRoot) 
        return false;

    std::string  strvalue = "";
    for(TiXmlElement* tableEle = xmlRoot->FirstChildElement(); tableEle; tableEle = tableEle->NextSiblingElement())
    {
        if(strcmp(tableEle->Value(), "fashions") == 0)
        {
            FashionItem item;
            for(TiXmlElement* fashionsEle = tableEle->FirstChildElement(); fashionsEle; fashionsEle = fashionsEle->NextSiblingElement())
            {
                item.Clear();

                item.id = SDAtoi(fashionsEle->Attribute("id"));
                item.quality = SDAtoi(fashionsEle->Attribute("quality"));

                if(item.quality > m_maxQuality)
                m_maxQuality =  item.quality;
                
                int32 activate = SDAtoi(fashionsEle->Attribute("activate"));
                if(activate ==0)
                    continue;

                strvalue = fashionsEle->Attribute("addpro");
                LoadProList(strvalue, &item.prolist);

                strvalue = fashionsEle->Attribute("open");
                LoadFashionItemList(&item.openList, strvalue);

                strvalue = fashionsEle->Attribute("incarnationCard");
                LoadFashionItemList(&item.incarnationCard, strvalue);

                for(TiXmlElement* fashionEle = fashionsEle->FirstChildElement(); fashionEle; fashionEle = fashionEle->NextSiblingElement())
                {
                    if(strcmp(fashionEle->Value(), "levelup") == 0)
                    {
                        FashionLevelUp levelUp;
                        for(TiXmlElement* levelUpEle = fashionEle->FirstChildElement(); levelUpEle; levelUpEle = levelUpEle->NextSiblingElement())
                        {
                            levelUp.Clear();

                            levelUp.level = SDAtoi(levelUpEle->Attribute("lv"));
                            //加载物品
                            strvalue = levelUpEle->Attribute("need");
                            LoadFashionItemList(&levelUp.itemList, strvalue);
                            strvalue = levelUpEle->Attribute("addpro");
                            LoadProList(strvalue, &levelUp.prolist);
                            item.levelUpList.push_back(levelUp);
                        }
                        std::sort(item.levelUpList.begin(), item.levelUpList.end(), SortConfigRules);
                    }
                }
                m_fashionList.push_back(item);
            }
        }
        if(strcmp(tableEle->Value(), "synergism") == 0)
        {
            SynergismItem syitem;
            for(TiXmlElement* synergismEle = tableEle->FirstChildElement(); synergismEle; synergismEle = synergismEle->NextSiblingElement())
            {
                syitem.Clear();

                syitem.id = SDAtoi(synergismEle->Attribute("id"));
                strvalue = synergismEle->Attribute("fashions");
                LoadProList(strvalue, &syitem.fashuins);

                strvalue = synergismEle->Attribute("addpro");
                LoadProList(strvalue, &syitem.prolist);
                m_synergismLIist.push_back(syitem);
            }
        }
    }

    return true;
}

bool FashionConfig::LoadFashionTreasureConfig( const char* path )
{
    TiXmlDocument xmlDoc;
    if (!xmlDoc.LoadFile(path))
    {
        CnError("Load npc file: %s failed\n", path);
        return false;
    }

    TiXmlElement* xmlRoot = xmlDoc.RootElement();
    if(!xmlRoot) 
        return false;

    std::string  strvalue = "";
    std::vector<std::string> key_value;
    for(TiXmlElement* tableEle = xmlRoot->FirstChildElement(); tableEle; tableEle = tableEle->NextSiblingElement())
    {
        if(strcmp(tableEle->Value(), "treasure") == 0)
        {
            FashionTreasureItem item;
            for (TiXmlElement* treaEle = tableEle->FirstChildElement(); treaEle; treaEle = treaEle->NextSiblingElement())
            {
                item.Clear();
                item.type = SDAtoi(treaEle->Attribute("type"));
                item.package_id = SDAtoi(treaEle->Attribute("package_id"));

                strvalue = treaEle->Attribute("need");
                LoadFashionItemList(&item.itemList, strvalue);
                
                m_treasureList.push_back(item);
            }
        }

        if(strcmp(tableEle->Value(), "incarnations") == 0)
        {
            IncarnationItem incarItem;
            FashionItemList itemlist;
            for (TiXmlElement* treaEle = tableEle->FirstChildElement(); treaEle; treaEle = treaEle->NextSiblingElement())
            {
                incarItem.Clear();
                incarItem.incarnation_id = SDAtoi(treaEle->Attribute("incarnation_id"));

                strvalue = treaEle->Attribute("item");
                LoadFashionItemList(&itemlist, strvalue);
                for(std::vector<REWARD_ITEM>::iterator ri = itemlist.begin(); ri != itemlist.end(); ++ ri)
                {
                    incarItem.item.id = ri->id;
                    incarItem.item.num = ri->num;
                }
                m_incarnationItemList.push_back(incarItem);
            }
        }
        
        if(strcmp(tableEle->Value(), "bronz_reward") == 0)
        {
            strvalue = tableEle->Attribute("value");
            LoadFashionTreasureList(&m_bronzList, strvalue);
        }
        if(strcmp(tableEle->Value(), "bronz_good_reward") == 0)
        {
            strvalue = tableEle->Attribute("value");
            LoadFashionTreasureList(&m_bronzGoodList, strvalue);
        }
        if(strcmp(tableEle->Value(), "gold_reward") == 0)
        {
            strvalue = tableEle->Attribute("value");
            LoadFashionTreasureList(&m_goldzList, strvalue);
        }
        if(strcmp(tableEle->Value(), "gold_good_reward") == 0)
        {
            strvalue = tableEle->Attribute("value");
            LoadFashionTreasureList(&m_goldzGoodList, strvalue);
        }
        if(strcmp(tableEle->Value(), "free_time") == 0)
        {
            strvalue = tableEle->Attribute("value");
            m_free_time = SDAtoi(strvalue.c_str());
        }
        if(strcmp(tableEle->Value(), "bronz_first") == 0)
        {
            strvalue = tableEle->Attribute("value");
            m_bronzFirstID = SDAtoi(strvalue.c_str());
        }
        if(strcmp(tableEle->Value(), "gold_first") == 0)
        {
            strvalue = tableEle->Attribute("value");
            m_goldFirstID = SDAtoi(strvalue.c_str());
        }
    }
    return true;
}

bool FashionConfig::LoadSaleConfig(const char* path)
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
        //saleID = Crown::SDAtou(saleEle->Attribute("id"));
        //saleList.clear();

        //for(TiXmlElement* itemEle = saleEle->FirstChildElement(); itemEle; itemEle = itemEle->NextSiblingElement())
        //{
        //    itemID = Crown::SDAtou(itemEle->Attribute("id"));
        //    itemTmpl = ItemConfig::Instance()->GetItemTmpl(itemID);
        //    if(!itemTmpl)
        //    {
        //        CnWarn("SaleID:%u can't find itemID:%u\n", saleID, itemID);
        //        continue;
        //    }

        //    saleList.push_back(itemTmpl);
        //}

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

//加载合成
void FashionConfig::LoadComposeItem(const std::string& base_value, FashionTemplate* fishion)
{
    if(!fishion)
        return;
    std::vector<std::string> values = Crown::SDSplitStrings(base_value, ' ');
    for(uint32 i=0; i<values.size(); ++i)
    {
        std::vector<std::string> key_value = Crown::SDSplitStrings(values[i], '-');
        REWARD_ITEM curitem;
        curitem.Clear();
        if(key_value.size() == 3)
        {
            //curitem.type = SDAtoi(key_value[0].c_str());
            //curitem.id = SDAtoi(key_value[1].c_str());
            //curitem.num = SDAtoi(key_value[2].c_str());
        }
        fishion->vec_compose.push_back(curitem);
    }
}

PropertyList FashionConfig::LoadFashionProList(const std::string& base_value)
{
    PropertyList propertyList;
    std::vector<std::string> values = Crown::SDSplitStrings(base_value, ' ');
    for(uint32 i=0; i<values.size(); ++i)
    {
        std::vector<std::string> key_value = Crown::SDSplitStrings(values[i], '-');

        Property curproperty;
        curproperty.Clear();
        if(key_value.size() == 2)
        {
            curproperty.type = SDAtoi(key_value[0].c_str());
            curproperty.value= SDAtoi(key_value[1].c_str());
        }
        propertyList.push_back(curproperty);
    }

    return propertyList;
}

// 获得时装模板
//const FashionTemplate* FashionConfig::GetFashion(uint32 id)
//{
//    for(MapFashionAtlas::iterator mapfashionAtlas = m_fashionatlas.begin(); mapfashionAtlas!= m_fashionatlas.end(); mapfashionAtlas++)
//    {
//        VecFashionList::iterator iterfashion;
//        for (iterfashion = mapfashionAtlas->second.vecfashoion.begin(); iterfashion != mapfashionAtlas->second.vecfashoion.end(); iterfashion++)
//        {
//            if (id == iterfashion->id)
//            {
//                return  &(*iterfashion);
//            }
//        }  
//    }
//    return 0;
//}


//========================================================================
//解析升级物品
//========================================================================
void FashionConfig::LoadFashionItemList(FashionItemList* list, const std::string& base_value)
{
    std::vector<std::string> values = Crown::SDSplitStrings(base_value, ' ');
    for(uint32 i=0; i<values.size(); ++i)
    {
        std::vector<std::string> key_value = Crown::SDSplitStrings(values[i], '-');
        REWARD_ITEM curitem;
        curitem.Clear();
        if(key_value.size() == 3)
        {
            //curitem.type = SDAtoi(key_value[0].c_str());
            curitem.id = SDAtoi(key_value[1].c_str());
            curitem.num = SDAtoi(key_value[2].c_str());
        }
        else if(key_value.size() == 2)
        {
            //curitem.type = SDAtoi(key_value[0].c_str());
            curitem.id = SDAtoi(key_value[0].c_str());
            curitem.num = SDAtoi(key_value[1].c_str());
        }
        list->push_back(curitem);
    }
}

//========================================================================
//解析属性列表
//========================================================================
void FashionConfig::LoadProList(const std::string& base_value, PropertyList* list)
{
    std::vector<std::string> values = Crown::SDSplitStrings(base_value, ' ');
    for(uint32 i=0; i<values.size(); ++i)
    {
        std::vector<std::string> key_value = Crown::SDSplitStrings(values[i], '-');

        Property curproperty;
        curproperty.Clear();
        if(key_value.size() == 2)
        {
            curproperty.type = SDAtoi(key_value[0].c_str());
            curproperty.value= SDAtoi(key_value[1].c_str());
        }
        list->push_back(curproperty);
    }
}
//========================================================
//获取时装信息
//========================================================
const FashionItem* FashionConfig::GetNewFashion( int32 id )
{
    for (FashionConfList::iterator item = m_fashionList.begin(); item != m_fashionList.end(); item++)
    {
        if(item->id == id)
            return &(*item);
    }
    return NULL;
}

//=================================================================
//获取时装下级升级条件
//=================================================================
const FashionLevelUp* FashionConfig::GetFashionNextLevel( int32 id, int32 level)
{
    for (FashionConfList::iterator item = m_fashionList.begin(); item != m_fashionList.end(); item++)
    {
        if(item->id == id)
        {
            for (FashionLevelList::iterator lvup = item->levelUpList.begin(); lvup != item->levelUpList.end(); lvup++)
            {
                if(lvup->level == level)
                {
                    return &(*lvup);
                }
            }
        }
    }
    return NULL;
}

//===========================================================================
//获取宝藏抽取配置
//===========================================================================
const FashionTreasureItem* FashionConfig::GetFashionTreasureNeed( int32 type )
{
    for (TreasureList::iterator it = m_treasureList.begin(); it != m_treasureList.end(); it++)
    {
        if(it->type == type)
            return &(*it);
    }
    return NULL;
}
const IncarnationItem* FashionConfig::GetIncarnationItemFragment( int32 id )
{
    for (std::vector<IncarnationItem>::iterator it = m_incarnationItemList.begin(); it != m_incarnationItemList.end(); it++)
    {
        if(it->incarnation_id == id)
            return &(*it);
    }
    return NULL;
}

//const ItemTemplate* FashionConfig::GetSaleItem(uint32 saleID, uint32 itemID)
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

//================================================
// 获取幻化卡对应的时装信息
//================================================
const FashionItem* FashionConfig::GetIncarnationCardFashionItem( int32 id )
{
    for (FashionConfList::iterator item = m_fashionList.begin(); item != m_fashionList.end(); item++)
    {
        if(item->incarnationCard.size() >= 1)
        {
            if(id == item->incarnationCard[0].id)
            {
               return &(*item);
            }
        }
    }
    return NULL;
}

//=========================================
// 解析化身宝藏物品
//=========================================
void FashionConfig::LoadFashionTreasureList( FashionTreasureConfigItemList* rewardList, const std::string& base_value )
{
    std::vector<std::string> values = Crown::SDSplitStrings(base_value, ' ');
    for(uint32 i=0; i<values.size(); ++i)
    {
        FASHION_TREASURE_CONFIG curitem;
        curitem.Clear();

        std::vector<std::string> reward_value = Crown::SDSplitStrings(values[i], '|');
        if(reward_value.size() >= 1)
        {
            std::vector<std::string> item_value = Crown::SDSplitStrings(reward_value[0], '-');
            if(item_value.size() == 3)
            {
                //curitem.item.type = SDAtoi(item_value[0].c_str());
                curitem.item.id = SDAtoi(item_value[1].c_str());
                curitem.item.num = SDAtoi(item_value[2].c_str());
            }
            else if(item_value.size() == 2)
            {
                //curitem.item.type = SDAtoi(item_value[0].c_str());
                curitem.item.id = SDAtoi(item_value[0].c_str());
                curitem.item.num = SDAtoi(item_value[1].c_str());
            }
        }

        if(reward_value.size() >= 2)
        {
            std::vector<std::string> probability_value = Crown::SDSplitStrings(reward_value[1], '-');
            if(probability_value.size() == 2)
            {
                curitem.probability_s = SDAtoi(probability_value[0].c_str());
                curitem.probability_e = SDAtoi(probability_value[1].c_str());
            }
        }
        rewardList->push_back(curitem);
    }
}

uint32 FashionConfig::GetTimeUint(const std::string& base_value )
{
    std::vector<std::string> values = Crown::SDSplitStrings(base_value, ':');
    if(values.size() == 4)
    {
        return SDAtoi(values[0].c_str())*24*3600 + SDAtoi(values[1].c_str())*3600 + SDAtoi(values[2].c_str())*60 + SDAtoi(values[3].c_str());
    }
    else if(values.size() == 3)
    {
        return SDAtoi(values[0].c_str())*3600 + SDAtoi(values[1].c_str())*60 + SDAtoi(values[2].c_str());
    }
    else if(values.size() == 2)
    {
        return SDAtoi(values[0].c_str())*60 + SDAtoi(values[1].c_str());
    }
    else if(values.size() == 1)
    {
        return SDAtoi(values[0].c_str());
    }
    else
    {
        return 0;
    }
}
