//auto created by structual_xml.py
#include "gameholder_pch.h"
#include "passive_skill_card_config.h"

void PASSIVE_SKILL_CARD_ROOT_TPLT_STRUCT::parse(TiXmlElement* xmlRoot)
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
    if (xmlRoot->Attribute("desc"))
    {
        desc = (xmlRoot->Attribute("desc"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "desc");
    }
}

void PASSIVE_SKILL_CARD_ROOT_STRUCT::parse(TiXmlElement* xmlRoot)
{
    if (!xmlRoot) return;
    for(TiXmlElement* childEle = xmlRoot->FirstChildElement(); childEle; childEle = childEle->NextSiblingElement())
    {
        if (Crown::SDStrcmp(childEle->Value(), "tplt") == 0)
        {
            PASSIVE_SKILL_CARD_ROOT_TPLT_STRUCT StructTplt;
            StructTplt.clear();
            StructTplt.parse(childEle);
            tplt_list.push_back(StructTplt);
            continue;
        }
    }
}

IMPLEMENT_SINGLETON(PASSIVE_SKILL_CARD_ENTRY)

bool PASSIVE_SKILL_CARD_ENTRY::LoadConfig(const char* path)
{
    char file_path[SD_MAX_PATH];
    SDSprintf(file_path, "%s%s", path, "passive_skill_card.xml");
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
