//auto created by structual_xml.py
#include "gameholder_pch.h"
#include "buff_config.h"

void BUFF_ROOT_BUFF_STRUCT::parse(TiXmlElement* xmlRoot)
{
    if (!xmlRoot) return;
    if (xmlRoot->Attribute("aura_radius"))
    {
        aura_radius = (int32)atoi(xmlRoot->Attribute("aura_radius"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "aura_radius");
    }
    if (xmlRoot->Attribute("overlap"))
    {
        overlap = (int32)atoi(xmlRoot->Attribute("overlap"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "overlap");
    }
    if (xmlRoot->Attribute("name"))
    {
        name = (xmlRoot->Attribute("name"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "name");
    }
    if (xmlRoot->Attribute("func_id"))
    {
        func_id = (int32)atoi(xmlRoot->Attribute("func_id"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "func_id");
    }
    if (xmlRoot->Attribute("is_aure"))
    {
        is_aure = (int32)atoi(xmlRoot->Attribute("is_aure"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "is_aure");
    }
    if (xmlRoot->Attribute("overlap_num"))
    {
        overlap_num = (int32)atoi(xmlRoot->Attribute("overlap_num"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "overlap_num");
    }
    if (xmlRoot->Attribute("id"))
    {
        id = (int32)atoi(xmlRoot->Attribute("id"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "id");
    }
    if (xmlRoot->Attribute("effect_type"))
    {
        effect_type = (int32)atoi(xmlRoot->Attribute("effect_type"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "effect_type");
    }
}

void BUFF_ROOT_STRUCT::parse(TiXmlElement* xmlRoot)
{
    if (!xmlRoot) return;
    for(TiXmlElement* childEle = xmlRoot->FirstChildElement(); childEle; childEle = childEle->NextSiblingElement())
    {
        if (Crown::SDStrcmp(childEle->Value(), "buff") == 0)
        {
            BUFF_ROOT_BUFF_STRUCT StructTplt;
            StructTplt.clear();
            StructTplt.parse(childEle);
            buff_list.push_back(StructTplt);
            continue;
        }
    }
}

IMPLEMENT_SINGLETON(BUFF_ENTRY)

bool BUFF_ENTRY::LoadConfig(const char* path)
{
    char file_path[SD_MAX_PATH];
    SDSprintf(file_path, "%s%s", path, "buff.xml");
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
