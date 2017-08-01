//auto created by structual_xml.py
#include "gameholder_pch.h"
#include "equip_attribute_config.h"

void EQUIP_ATTRIBUTE_ROOT_MODIFIER_STRUCT::parse(TiXmlElement* xmlRoot)
{
    if (!xmlRoot) return;
    if (xmlRoot->Attribute("grade"))
    {
        grade = (int32)atoi(xmlRoot->Attribute("grade"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "grade");
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
    if (xmlRoot->Attribute("type"))
    {
        type = (int32)atoi(xmlRoot->Attribute("type"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "type");
    }
    if (xmlRoot->Attribute("id"))
    {
        id = (int32)atoi(xmlRoot->Attribute("id"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "id");
    }
}

void EQUIP_ATTRIBUTE_ROOT_STRUCT::parse(TiXmlElement* xmlRoot)
{
    if (!xmlRoot) return;
    for(TiXmlElement* childEle = xmlRoot->FirstChildElement(); childEle; childEle = childEle->NextSiblingElement())
    {
        if (Crown::SDStrcmp(childEle->Value(), "modifier") == 0)
        {
            EQUIP_ATTRIBUTE_ROOT_MODIFIER_STRUCT StructTplt;
            StructTplt.clear();
            StructTplt.parse(childEle);
            modifier_list.push_back(StructTplt);
            continue;
        }
    }
}

IMPLEMENT_SINGLETON(EQUIP_ATTRIBUTE_ENTRY)

bool EQUIP_ATTRIBUTE_ENTRY::LoadConfig(const char* path)
{
    char file_path[SD_MAX_PATH];
    SDSprintf(file_path, "%s%s", path, "equip_attribute.xml");
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
