//auto created by structual_xml.py
#include "commonlib_pch.h"
#include "game_param_config.h"

void GAME_PARAM_ROOT_SYSTEM_STRUCT::parse(TiXmlElement* xmlRoot)
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
    if (xmlRoot->Attribute("value"))
    {
        value = (int32)atoi(xmlRoot->Attribute("value"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "value");
    }
}

void GAME_PARAM_ROOT_STRUCT::parse(TiXmlElement* xmlRoot)
{
    if (!xmlRoot) return;
    for(TiXmlElement* childEle = xmlRoot->FirstChildElement(); childEle; childEle = childEle->NextSiblingElement())
    {
        if (Crown::SDStrcmp(childEle->Value(), "system") == 0)
        {
            GAME_PARAM_ROOT_SYSTEM_STRUCT StructTplt;
            StructTplt.clear();
            StructTplt.parse(childEle);
            system_list.push_back(StructTplt);
            continue;
        }
    }
}

IMPLEMENT_SINGLETON(GAME_PARAM_ENTRY)

bool GAME_PARAM_ENTRY::LoadConfig(const char* path)
{
    char file_path[SD_MAX_PATH];
    SDSprintf(file_path, "%s%s", path, "game_param.xml");
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
