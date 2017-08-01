//auto created by structual_xml.py
#include "commonlib_pch.h"
#include "mall_config.h"

void MALL_ROOT_VERSION_STRUCT::parse(TiXmlElement* xmlRoot)
{
    if (!xmlRoot) return;
    if (xmlRoot->Attribute("number"))
    {
        number = (int32)atoi(xmlRoot->Attribute("number"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "number");
    }
}

void MALL_ROOT_LIST_PRODUCT_STRUCT::parse(TiXmlElement* xmlRoot)
{
    if (!xmlRoot) return;
    if (xmlRoot->Attribute("show_vip_price"))
    {
        show_vip_price = (int32)atoi(xmlRoot->Attribute("show_vip_price"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "show_vip_price");
    }
    if (xmlRoot->Attribute("new_flag"))
    {
        new_flag = (int32)atoi(xmlRoot->Attribute("new_flag"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "new_flag");
    }
    if (xmlRoot->Attribute("show_flag"))
    {
        show_flag = (int32)atoi(xmlRoot->Attribute("show_flag"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "show_flag");
    }
    if (xmlRoot->Attribute("item_id"))
    {
        item_id = (int32)atoi(xmlRoot->Attribute("item_id"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "item_id");
    }
    if (xmlRoot->Attribute("product_num"))
    {
        product_num = (int32)atoi(xmlRoot->Attribute("product_num"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "product_num");
    }
    if (xmlRoot->Attribute("home_page"))
    {
        home_page = (int32)atoi(xmlRoot->Attribute("home_page"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "home_page");
    }
    if (xmlRoot->Attribute("show_price"))
    {
        show_price = (int32)atoi(xmlRoot->Attribute("show_price"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "show_price");
    }
    if (xmlRoot->Attribute("vip_price"))
    {
        vip_price = (int32)atoi(xmlRoot->Attribute("vip_price"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "vip_price");
    }
    if (xmlRoot->Attribute("again_discount"))
    {
        again_discount = (int32)atoi(xmlRoot->Attribute("again_discount"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "again_discount");
    }
    if (xmlRoot->Attribute("again_price"))
    {
        again_price = (int32)atoi(xmlRoot->Attribute("again_price"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "again_price");
    }
    if (xmlRoot->Attribute("sell_type"))
    {
        sell_type = (int32)atoi(xmlRoot->Attribute("sell_type"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "sell_type");
    }
    if (xmlRoot->Attribute("start_time"))
    {
        start_time = (xmlRoot->Attribute("start_time"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "start_time");
    }
    if (xmlRoot->Attribute("price"))
    {
        price = (int32)atoi(xmlRoot->Attribute("price"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "price");
    }
    if (xmlRoot->Attribute("again_start_time"))
    {
        again_start_time = (int32)atoi(xmlRoot->Attribute("again_start_time"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "again_start_time");
    }
    if (xmlRoot->Attribute("again_end_time"))
    {
        again_end_time = (int32)atoi(xmlRoot->Attribute("again_end_time"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "again_end_time");
    }
    if (xmlRoot->Attribute("discount"))
    {
        discount = (int32)atoi(xmlRoot->Attribute("discount"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "discount");
    }
    if (xmlRoot->Attribute("global_limit_num"))
    {
        global_limit_num = (int32)atoi(xmlRoot->Attribute("global_limit_num"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "global_limit_num");
    }
    if (xmlRoot->Attribute("item_num"))
    {
        item_num = (int32)atoi(xmlRoot->Attribute("item_num"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "item_num");
    }
    if (xmlRoot->Attribute("money_type"))
    {
        money_type = (int32)atoi(xmlRoot->Attribute("money_type"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "money_type");
    }
    if (xmlRoot->Attribute("show_again_price"))
    {
        show_again_price = (int32)atoi(xmlRoot->Attribute("show_again_price"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "show_again_price");
    }
    if (xmlRoot->Attribute("product_id"))
    {
        product_id = (int32)atoi(xmlRoot->Attribute("product_id"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "product_id");
    }
    if (xmlRoot->Attribute("hot_flag"))
    {
        hot_flag = (int32)atoi(xmlRoot->Attribute("hot_flag"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "hot_flag");
    }
    if (xmlRoot->Attribute("end_time"))
    {
        end_time = (xmlRoot->Attribute("end_time"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "end_time");
    }
    if (xmlRoot->Attribute("personal_limit_num"))
    {
        personal_limit_num = (int32)atoi(xmlRoot->Attribute("personal_limit_num"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "personal_limit_num");
    }
    if (xmlRoot->Attribute("present_flag"))
    {
        present_flag = (int32)atoi(xmlRoot->Attribute("present_flag"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "present_flag");
    }
    if (xmlRoot->Attribute("tag_type"))
    {
        tag_type = (int32)atoi(xmlRoot->Attribute("tag_type"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "tag_type");
    }
}

void MALL_ROOT_LIST_STRUCT::parse(TiXmlElement* xmlRoot)
{
    if (!xmlRoot) return;
    for(TiXmlElement* childEle = xmlRoot->FirstChildElement(); childEle; childEle = childEle->NextSiblingElement())
    {
        if (Crown::SDStrcmp(childEle->Value(), "product") == 0)
        {
            MALL_ROOT_LIST_PRODUCT_STRUCT StructTplt;
            StructTplt.clear();
            StructTplt.parse(childEle);
            product_list.push_back(StructTplt);
            continue;
        }
    }
}

void MALL_ROOT_STRUCT::parse(TiXmlElement* xmlRoot)
{
    if (!xmlRoot) return;
    for(TiXmlElement* childEle = xmlRoot->FirstChildElement(); childEle; childEle = childEle->NextSiblingElement())
    {
        if (Crown::SDStrcmp(childEle->Value(), "version") == 0)
        {
            version.clear();
            version.parse(childEle);
            continue;
        }
        if (Crown::SDStrcmp(childEle->Value(), "list") == 0)
        {
            list.clear();
            list.parse(childEle);
            continue;
        }
    }
}

IMPLEMENT_SINGLETON(MALL_ENTRY)

bool MALL_ENTRY::LoadConfig(const char* path)
{
    char file_path[SD_MAX_PATH];
    SDSprintf(file_path, "%s%s", path, "mall.xml");
    TiXmlDocument xmlDoc;
    if (!xmlDoc.LoadFile(file_path))
    {
        CnError("Load file: %s failed\n", file_path);
        return false;
    }
    TiXmlElement* xmlRoot = xmlDoc.RootElement();
    if(!xmlRoot) return false;
    m_Struct.parse(xmlRoot);
    return true;
}
