//auto created by structual_xml.py
#include "gameholder_pch.h"
#include "dungeon_clearance_bonus_config.h"

void DUNGEON_CLEARANCE_BONUS_ROOT_DUNGEON_STRUCT::parse(TiXmlElement* xmlRoot)
{
    if (!xmlRoot) return;
    if (xmlRoot->Attribute("three_star_bonus"))
    {
        three_star_bonus = (xmlRoot->Attribute("three_star_bonus"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "three_star_bonus");
    }
    if (xmlRoot->Attribute("clearance_exp"))
    {
        clearance_exp = (int32)atoi(xmlRoot->Attribute("clearance_exp"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "clearance_exp");
    }
    if (xmlRoot->Attribute("dungeon_id"))
    {
        dungeon_id = (int32)atoi(xmlRoot->Attribute("dungeon_id"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "dungeon_id");
    }
    if (xmlRoot->Attribute("clearance_bonus"))
    {
        clearance_bonus = (xmlRoot->Attribute("clearance_bonus"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "clearance_bonus");
    }
    if (xmlRoot->Attribute("two_star_bonus"))
    {
        two_star_bonus = (xmlRoot->Attribute("two_star_bonus"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "two_star_bonus");
    }
}

void DUNGEON_CLEARANCE_BONUS_ROOT_STRUCT::parse(TiXmlElement* xmlRoot)
{
    if (!xmlRoot) return;
    for(TiXmlElement* childEle = xmlRoot->FirstChildElement(); childEle; childEle = childEle->NextSiblingElement())
    {
        if (Crown::SDStrcmp(childEle->Value(), "dungeon") == 0)
        {
            DUNGEON_CLEARANCE_BONUS_ROOT_DUNGEON_STRUCT StructTplt;
            StructTplt.clear();
            StructTplt.parse(childEle);
            dungeon_list.push_back(StructTplt);
            continue;
        }
    }
}

IMPLEMENT_SINGLETON(DUNGEON_CLEARANCE_BONUS_ENTRY)

bool DUNGEON_CLEARANCE_BONUS_ENTRY::LoadConfig(const char* path)
{
    char file_path[SD_MAX_PATH];
    SDSprintf(file_path, "%s%s", path, "dungeon_clearance_bonus.xml");
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
