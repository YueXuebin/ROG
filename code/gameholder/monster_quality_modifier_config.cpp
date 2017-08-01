//auto created by structual_xml.py
#include "gameholder_pch.h"
#include "monster_quality_modifier_config.h"

void MONSTER_QUALITY_MODIFIER_ROOT_MODIFIER_STRUCT::parse(TiXmlElement* xmlRoot)
{
    if (!xmlRoot) return;
    if (xmlRoot->Attribute("prop1_id"))
    {
        prop1_id = (int32)atoi(xmlRoot->Attribute("prop1_id"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "prop1_id");
    }
    if (xmlRoot->Attribute("name"))
    {
        name = (xmlRoot->Attribute("name"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "name");
    }
    if (xmlRoot->Attribute("prop1_value"))
    {
        prop1_value = (int32)atoi(xmlRoot->Attribute("prop1_value"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "prop1_value");
    }
    if (xmlRoot->Attribute("prop2_id"))
    {
        prop2_id = (int32)atoi(xmlRoot->Attribute("prop2_id"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "prop2_id");
    }
    if (xmlRoot->Attribute("prop2_value"))
    {
        prop2_value = (int32)atoi(xmlRoot->Attribute("prop2_value"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "prop2_value");
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

void MONSTER_QUALITY_MODIFIER_ROOT_STRUCT::parse(TiXmlElement* xmlRoot)
{
    if (!xmlRoot) return;
    for(TiXmlElement* childEle = xmlRoot->FirstChildElement(); childEle; childEle = childEle->NextSiblingElement())
    {
        if (Crown::SDStrcmp(childEle->Value(), "modifier") == 0)
        {
            MONSTER_QUALITY_MODIFIER_ROOT_MODIFIER_STRUCT StructTplt;
            StructTplt.clear();
            StructTplt.parse(childEle);
            modifier_list.push_back(StructTplt);
            continue;
        }
    }
}

IMPLEMENT_SINGLETON(MONSTER_QUALITY_MODIFIER_ENTRY)

bool MONSTER_QUALITY_MODIFIER_ENTRY::LoadConfig(const char* path)
{
    char file_path[SD_MAX_PATH];
    SDSprintf(file_path, "%s%s", path, "monster_quality_modifier.xml");
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
