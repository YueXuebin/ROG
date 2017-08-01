//auto created by structual_xml.py
#include "gameholder_pch.h"
#include "gem_config.h"

void GEM_ROOT_GEM_ITEM_STRUCT::parse(TiXmlElement* xmlRoot)
{
    if (!xmlRoot) return;
    if (xmlRoot->Attribute("remove_price"))
    {
        remove_price = (int32)atoi(xmlRoot->Attribute("remove_price"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "remove_price");
    }
    if (xmlRoot->Attribute("fusion_price"))
    {
        fusion_price = (int32)atoi(xmlRoot->Attribute("fusion_price"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "fusion_price");
    }
    if (xmlRoot->Attribute("level"))
    {
        level = (int32)atoi(xmlRoot->Attribute("level"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "level");
    }
    if (xmlRoot->Attribute("socketing_price"))
    {
        socketing_price = (int32)atoi(xmlRoot->Attribute("socketing_price"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "socketing_price");
    }
    if (xmlRoot->Attribute("fusion_exp"))
    {
        fusion_exp = (int32)atoi(xmlRoot->Attribute("fusion_exp"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "fusion_exp");
    }
    if (xmlRoot->Attribute("exp"))
    {
        exp = (int32)atoi(xmlRoot->Attribute("exp"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "exp");
    }
    if (xmlRoot->Attribute("prop_id"))
    {
        prop_id = (int32)atoi(xmlRoot->Attribute("prop_id"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "prop_id");
    }
    if (xmlRoot->Attribute("prop_value"))
    {
        prop_value = (int32)atoi(xmlRoot->Attribute("prop_value"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "prop_value");
    }
}

void GEM_ROOT_GEM_STRUCT::parse(TiXmlElement* xmlRoot)
{
    if (!xmlRoot) return;
    if (xmlRoot->Attribute("hole_type"))
    {
        hole_type = (int32)atoi(xmlRoot->Attribute("hole_type"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "hole_type");
    }
    if (xmlRoot->Attribute("id"))
    {
        id = (int32)atoi(xmlRoot->Attribute("id"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "id");
    }
    for(TiXmlElement* childEle = xmlRoot->FirstChildElement(); childEle; childEle = childEle->NextSiblingElement())
    {
        if (Crown::SDStrcmp(childEle->Value(), "item") == 0)
        {
            GEM_ROOT_GEM_ITEM_STRUCT StructTplt;
            StructTplt.clear();
            StructTplt.parse(childEle);
            item_list.push_back(StructTplt);
            continue;
        }
    }
}

void GEM_ROOT_STRUCT::parse(TiXmlElement* xmlRoot)
{
    if (!xmlRoot) return;
    for(TiXmlElement* childEle = xmlRoot->FirstChildElement(); childEle; childEle = childEle->NextSiblingElement())
    {
        if (Crown::SDStrcmp(childEle->Value(), "gem") == 0)
        {
            GEM_ROOT_GEM_STRUCT StructTplt;
            StructTplt.clear();
            StructTplt.parse(childEle);
            gem_list.push_back(StructTplt);
            continue;
        }
    }
}

IMPLEMENT_SINGLETON(GEM_ENTRY)

bool GEM_ENTRY::LoadConfig(const char* path)
{
    char file_path[SD_MAX_PATH];
    SDSprintf(file_path, "%s%s", path, "gem.xml");
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
