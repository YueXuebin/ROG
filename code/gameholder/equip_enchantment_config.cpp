//auto created by structual_xml.py
#include "gameholder_pch.h"
#include "equip_enchantment_config.h"

void EQUIP_ENCHANTMENT_ROOT_FORMULA_STRUCT::parse(TiXmlElement* xmlRoot)
{
    if (!xmlRoot) return;
    if (xmlRoot->Attribute("item3_id"))
    {
        item3_id = (int32)atoi(xmlRoot->Attribute("item3_id"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "item3_id");
    }
    if (xmlRoot->Attribute("item1_id"))
    {
        item1_id = (int32)atoi(xmlRoot->Attribute("item1_id"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "item1_id");
    }
    if (xmlRoot->Attribute("item2_id"))
    {
        item2_id = (int32)atoi(xmlRoot->Attribute("item2_id"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "item2_id");
    }
    if (xmlRoot->Attribute("item2_num"))
    {
        item2_num = (int32)atoi(xmlRoot->Attribute("item2_num"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "item2_num");
    }
    if (xmlRoot->Attribute("attribute_id"))
    {
        attribute_id = (int32)atoi(xmlRoot->Attribute("attribute_id"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "attribute_id");
    }
    if (xmlRoot->Attribute("item4_id"))
    {
        item4_id = (int32)atoi(xmlRoot->Attribute("item4_id"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "item4_id");
    }
    if (xmlRoot->Attribute("item1_num"))
    {
        item1_num = (int32)atoi(xmlRoot->Attribute("item1_num"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "item1_num");
    }
    if (xmlRoot->Attribute("item3_num"))
    {
        item3_num = (int32)atoi(xmlRoot->Attribute("item3_num"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "item3_num");
    }
    if (xmlRoot->Attribute("item4_num"))
    {
        item4_num = (int32)atoi(xmlRoot->Attribute("item4_num"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "item4_num");
    }
}

void EQUIP_ENCHANTMENT_ROOT_STRUCT::parse(TiXmlElement* xmlRoot)
{
    if (!xmlRoot) return;
    for(TiXmlElement* childEle = xmlRoot->FirstChildElement(); childEle; childEle = childEle->NextSiblingElement())
    {
        if (Crown::SDStrcmp(childEle->Value(), "formula") == 0)
        {
            EQUIP_ENCHANTMENT_ROOT_FORMULA_STRUCT StructTplt;
            StructTplt.clear();
            StructTplt.parse(childEle);
            formula_list.push_back(StructTplt);
            continue;
        }
    }
}

IMPLEMENT_SINGLETON(EQUIP_ENCHANTMENT_ENTRY)

bool EQUIP_ENCHANTMENT_ENTRY::LoadConfig(const char* path)
{
    char file_path[SD_MAX_PATH];
    SDSprintf(file_path, "%s%s", path, "equip_enchantment.xml");
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
