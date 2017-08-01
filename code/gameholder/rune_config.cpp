//auto created by structual_xml.py
#include "gameholder_pch.h"
#include "rune_config.h"

void RUNE_ROOT_RUNE_STRUCT::parse(TiXmlElement* xmlRoot)
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
    if (xmlRoot->Attribute("prop1_base"))
    {
        prop1_base = (int32)atoi(xmlRoot->Attribute("prop1_base"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "prop1_base");
    }
    if (xmlRoot->Attribute("feature"))
    {
        feature = (int32)atoi(xmlRoot->Attribute("feature"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "feature");
    }
    if (xmlRoot->Attribute("prop2_base"))
    {
        prop2_base = (int32)atoi(xmlRoot->Attribute("prop2_base"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "prop2_base");
    }
    if (xmlRoot->Attribute("mana"))
    {
        mana = (int32)atoi(xmlRoot->Attribute("mana"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "mana");
    }
    if (xmlRoot->Attribute("trigger"))
    {
        trigger = (int32)atoi(xmlRoot->Attribute("trigger"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "trigger");
    }
    if (xmlRoot->Attribute("letter"))
    {
        letter = (xmlRoot->Attribute("letter"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "letter");
    }
    if (xmlRoot->Attribute("id"))
    {
        id = (int32)atoi(xmlRoot->Attribute("id"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "id");
    }
    if (xmlRoot->Attribute("prop1_grow"))
    {
        prop1_grow = (int32)atoi(xmlRoot->Attribute("prop1_grow"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "prop1_grow");
    }
    if (xmlRoot->Attribute("prop2_id"))
    {
        prop2_id = (int32)atoi(xmlRoot->Attribute("prop2_id"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "prop2_id");
    }
    if (xmlRoot->Attribute("prop2_grow"))
    {
        prop2_grow = (int32)atoi(xmlRoot->Attribute("prop2_grow"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "prop2_grow");
    }
}

void RUNE_ROOT_STRUCT::parse(TiXmlElement* xmlRoot)
{
    if (!xmlRoot) return;
    for(TiXmlElement* childEle = xmlRoot->FirstChildElement(); childEle; childEle = childEle->NextSiblingElement())
    {
        if (Crown::SDStrcmp(childEle->Value(), "rune") == 0)
        {
            RUNE_ROOT_RUNE_STRUCT StructTplt;
            StructTplt.clear();
            StructTplt.parse(childEle);
            rune_list.push_back(StructTplt);
            continue;
        }
    }
}

IMPLEMENT_SINGLETON(RUNE_ENTRY)

bool RUNE_ENTRY::LoadConfig(const char* path)
{
    char file_path[SD_MAX_PATH];
    SDSprintf(file_path, "%s%s", path, "rune.xml");
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
