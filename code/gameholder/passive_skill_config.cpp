//auto created by structual_xml.py
#include "gameholder_pch.h"
#include "passive_skill_config.h"

void PASSIVE_SKILL_ROOT_PASSIVE_SKILL_ITEM_STRUCT::parse(TiXmlElement* xmlRoot)
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
    if (xmlRoot->Attribute("prop2_value"))
    {
        prop2_value = (int32)atoi(xmlRoot->Attribute("prop2_value"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "prop2_value");
    }
    if (xmlRoot->Attribute("level"))
    {
        level = (int32)atoi(xmlRoot->Attribute("level"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "level");
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
}

void PASSIVE_SKILL_ROOT_PASSIVE_SKILL_STRUCT::parse(TiXmlElement* xmlRoot)
{
    if (!xmlRoot) return;
    if (xmlRoot->Attribute("name"))
    {
        name = (xmlRoot->Attribute("name"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "name");
    }
    if (xmlRoot->Attribute("id"))
    {
        id = (int32)atoi(xmlRoot->Attribute("id"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "id");
    }
    if (xmlRoot->Attribute("prop2_value"))
    {
        prop2_value = (int32)atoi(xmlRoot->Attribute("prop2_value"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "prop2_value");
    }
    if (xmlRoot->Attribute("unlock_points"))
    {
        unlock_points = (int32)atoi(xmlRoot->Attribute("unlock_points"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "unlock_points");
    }
    if (xmlRoot->Attribute("type"))
    {
        type = (int32)atoi(xmlRoot->Attribute("type"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "type");
    }
    if (xmlRoot->Attribute("prop2_id"))
    {
        prop2_id = (int32)atoi(xmlRoot->Attribute("prop2_id"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "prop2_id");
    }
    if (xmlRoot->Attribute("desc"))
    {
        desc = (xmlRoot->Attribute("desc"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "desc");
    }
    for(TiXmlElement* childEle = xmlRoot->FirstChildElement(); childEle; childEle = childEle->NextSiblingElement())
    {
        if (Crown::SDStrcmp(childEle->Value(), "item") == 0)
        {
            PASSIVE_SKILL_ROOT_PASSIVE_SKILL_ITEM_STRUCT StructTplt;
            StructTplt.clear();
            StructTplt.parse(childEle);
            item_list.push_back(StructTplt);
            continue;
        }
    }
}

void PASSIVE_SKILL_ROOT_STRUCT::parse(TiXmlElement* xmlRoot)
{
    if (!xmlRoot) return;
    for(TiXmlElement* childEle = xmlRoot->FirstChildElement(); childEle; childEle = childEle->NextSiblingElement())
    {
        if (Crown::SDStrcmp(childEle->Value(), "passive_skill") == 0)
        {
            PASSIVE_SKILL_ROOT_PASSIVE_SKILL_STRUCT StructTplt;
            StructTplt.clear();
            StructTplt.parse(childEle);
            passive_skill_list.push_back(StructTplt);
            continue;
        }
    }
}

IMPLEMENT_SINGLETON(PASSIVE_SKILL_ENTRY)

bool PASSIVE_SKILL_ENTRY::LoadConfig(const char* path)
{
    char file_path[SD_MAX_PATH];
    SDSprintf(file_path, "%s%s", path, "passive_skill.xml");
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
