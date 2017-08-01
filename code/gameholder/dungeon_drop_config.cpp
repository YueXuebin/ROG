//auto created by structual_xml.py
#include "gameholder_pch.h"
#include "dungeon_drop_config.h"

void DUNGEON_DROP_ROOT_DROP_STRUCT::parse(TiXmlElement* xmlRoot)
{
    if (!xmlRoot) return;
    if (xmlRoot->Attribute("package_id"))
    {
        package_id = (int32)atoi(xmlRoot->Attribute("package_id"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "package_id");
    }
    if (xmlRoot->Attribute("drop_probability"))
    {
        drop_probability = (int32)atoi(xmlRoot->Attribute("drop_probability"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "drop_probability");
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

void DUNGEON_DROP_ROOT_STRUCT::parse(TiXmlElement* xmlRoot)
{
    if (!xmlRoot) return;
    for(TiXmlElement* childEle = xmlRoot->FirstChildElement(); childEle; childEle = childEle->NextSiblingElement())
    {
        if (Crown::SDStrcmp(childEle->Value(), "drop") == 0)
        {
            DUNGEON_DROP_ROOT_DROP_STRUCT StructTplt;
            StructTplt.clear();
            StructTplt.parse(childEle);
            drop_list.push_back(StructTplt);
            continue;
        }
    }
}

IMPLEMENT_SINGLETON(DUNGEON_DROP_ENTRY)

bool DUNGEON_DROP_ENTRY::LoadConfig(const char* path)
{
    char file_path[SD_MAX_PATH];
    SDSprintf(file_path, "%s%s", path, "dungeon_drop.xml");
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
