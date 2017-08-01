//auto created by structual_xml.py
#include "gameholder_pch.h"
#include "trigger_config.h"

void TRIGGER_ROOT_TRIGGER_STRUCT::parse(TiXmlElement* xmlRoot)
{
    if (!xmlRoot) return;
    if (xmlRoot->Attribute("pick_pos"))
    {
        pick_pos = (xmlRoot->Attribute("pick_pos"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "pick_pos");
    }
    if (xmlRoot->Attribute("pick_size"))
    {
        pick_size = (xmlRoot->Attribute("pick_size"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "pick_size");
    }
    if (xmlRoot->Attribute("object_set"))
    {
        object_set = (xmlRoot->Attribute("object_set"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "object_set");
    }
    if (xmlRoot->Attribute("name"))
    {
        name = (xmlRoot->Attribute("name"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "name");
    }
    if (xmlRoot->Attribute("effect_pos"))
    {
        effect_pos = (xmlRoot->Attribute("effect_pos"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "effect_pos");
    }
    if (xmlRoot->Attribute("show_id"))
    {
        show_id = (int32)atoi(xmlRoot->Attribute("show_id"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "show_id");
    }
    if (xmlRoot->Attribute("effect"))
    {
        effect = (xmlRoot->Attribute("effect"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "effect");
    }
    if (xmlRoot->Attribute("type"))
    {
        type = (int32)atoi(xmlRoot->Attribute("type"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "type");
    }
    if (xmlRoot->Attribute("object_set_pos"))
    {
        object_set_pos = (xmlRoot->Attribute("object_set_pos"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "object_set_pos");
    }
}

void TRIGGER_ROOT_STRUCT::parse(TiXmlElement* xmlRoot)
{
    if (!xmlRoot) return;
    for(TiXmlElement* childEle = xmlRoot->FirstChildElement(); childEle; childEle = childEle->NextSiblingElement())
    {
        if (Crown::SDStrcmp(childEle->Value(), "trigger") == 0)
        {
            TRIGGER_ROOT_TRIGGER_STRUCT StructTplt;
            StructTplt.clear();
            StructTplt.parse(childEle);
            trigger_list.push_back(StructTplt);
            continue;
        }
    }
}

IMPLEMENT_SINGLETON(TRIGGER_ENTRY)

bool TRIGGER_ENTRY::LoadConfig(const char* path)
{
    char file_path[SD_MAX_PATH];
    SDSprintf(file_path, "%s%s", path, "trigger.xml");
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
