//auto created by structual_xml.py
#include "gameholder_pch.h"
#include "dungeon_exp_bonus_config.h"

void DUNGEON_EXP_BONUS_ROOT_DUNGEON_STRUCT::parse(TiXmlElement* xmlRoot)
{
    if (!xmlRoot) return;
    if (xmlRoot->Attribute("exp_5"))
    {
        exp_5 = (int32)atoi(xmlRoot->Attribute("exp_5"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "exp_5");
    }
    if (xmlRoot->Attribute("exp_3"))
    {
        exp_3 = (int32)atoi(xmlRoot->Attribute("exp_3"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "exp_3");
    }
    if (xmlRoot->Attribute("exp_4"))
    {
        exp_4 = (int32)atoi(xmlRoot->Attribute("exp_4"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "exp_4");
    }
    if (xmlRoot->Attribute("exp_2"))
    {
        exp_2 = (int32)atoi(xmlRoot->Attribute("exp_2"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "exp_2");
    }
    if (xmlRoot->Attribute("exp_bonus_3"))
    {
        exp_bonus_3 = (xmlRoot->Attribute("exp_bonus_3"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "exp_bonus_3");
    }
    if (xmlRoot->Attribute("exp_bonus_2"))
    {
        exp_bonus_2 = (xmlRoot->Attribute("exp_bonus_2"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "exp_bonus_2");
    }
    if (xmlRoot->Attribute("exp_bonus_1"))
    {
        exp_bonus_1 = (xmlRoot->Attribute("exp_bonus_1"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "exp_bonus_1");
    }
    if (xmlRoot->Attribute("exp_1"))
    {
        exp_1 = (int32)atoi(xmlRoot->Attribute("exp_1"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "exp_1");
    }
    if (xmlRoot->Attribute("exp_bonus_5"))
    {
        exp_bonus_5 = (xmlRoot->Attribute("exp_bonus_5"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "exp_bonus_5");
    }
    if (xmlRoot->Attribute("exp_bonus_4"))
    {
        exp_bonus_4 = (xmlRoot->Attribute("exp_bonus_4"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "exp_bonus_4");
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

void DUNGEON_EXP_BONUS_ROOT_STRUCT::parse(TiXmlElement* xmlRoot)
{
    if (!xmlRoot) return;
    for(TiXmlElement* childEle = xmlRoot->FirstChildElement(); childEle; childEle = childEle->NextSiblingElement())
    {
        if (Crown::SDStrcmp(childEle->Value(), "dungeon") == 0)
        {
            DUNGEON_EXP_BONUS_ROOT_DUNGEON_STRUCT StructTplt;
            StructTplt.clear();
            StructTplt.parse(childEle);
            dungeon_list.push_back(StructTplt);
            continue;
        }
    }
}

IMPLEMENT_SINGLETON(DUNGEON_EXP_BONUS_ENTRY)

bool DUNGEON_EXP_BONUS_ENTRY::LoadConfig(const char* path)
{
    char file_path[SD_MAX_PATH];
    SDSprintf(file_path, "%s%s", path, "dungeon_exp_bonus.xml");
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
