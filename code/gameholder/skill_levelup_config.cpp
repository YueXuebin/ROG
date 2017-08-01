//auto created by structual_xml.py
#include "gameholder_pch.h"
#include "skill_levelup_config.h"

void SKILL_LEVELUP_ROOT_SKILL_VALUE_STRUCT::parse(TiXmlElement* xmlRoot)
{
    if (!xmlRoot) return;
    if (xmlRoot->Attribute("blue"))
    {
        blue = (int32)atoi(xmlRoot->Attribute("blue"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "blue");
    }
    if (xmlRoot->Attribute("gold"))
    {
        gold = (int32)atoi(xmlRoot->Attribute("gold"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "gold");
    }
    if (xmlRoot->Attribute("level"))
    {
        level = (int32)atoi(xmlRoot->Attribute("level"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "level");
    }
    if (xmlRoot->Attribute("green_exp"))
    {
        green_exp = (int32)atoi(xmlRoot->Attribute("green_exp"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "green_exp");
    }
    if (xmlRoot->Attribute("white_exp"))
    {
        white_exp = (int32)atoi(xmlRoot->Attribute("white_exp"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "white_exp");
    }
    if (xmlRoot->Attribute("gold_exp"))
    {
        gold_exp = (int32)atoi(xmlRoot->Attribute("gold_exp"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "gold_exp");
    }
    if (xmlRoot->Attribute("purple"))
    {
        purple = (int32)atoi(xmlRoot->Attribute("purple"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "purple");
    }
    if (xmlRoot->Attribute("green"))
    {
        green = (int32)atoi(xmlRoot->Attribute("green"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "green");
    }
    if (xmlRoot->Attribute("purple_exp"))
    {
        purple_exp = (int32)atoi(xmlRoot->Attribute("purple_exp"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "purple_exp");
    }
    if (xmlRoot->Attribute("white"))
    {
        white = (int32)atoi(xmlRoot->Attribute("white"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "white");
    }
    if (xmlRoot->Attribute("blue_exp"))
    {
        blue_exp = (int32)atoi(xmlRoot->Attribute("blue_exp"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "blue_exp");
    }
}

void SKILL_LEVELUP_ROOT_STRUCT::parse(TiXmlElement* xmlRoot)
{
    if (!xmlRoot) return;
    for(TiXmlElement* childEle = xmlRoot->FirstChildElement(); childEle; childEle = childEle->NextSiblingElement())
    {
        if (Crown::SDStrcmp(childEle->Value(), "skill_value") == 0)
        {
            SKILL_LEVELUP_ROOT_SKILL_VALUE_STRUCT StructTplt;
            StructTplt.clear();
            StructTplt.parse(childEle);
            skill_value_list.push_back(StructTplt);
            continue;
        }
    }
}

IMPLEMENT_SINGLETON(SKILL_LEVELUP_ENTRY)

bool SKILL_LEVELUP_ENTRY::LoadConfig(const char* path)
{
    char file_path[SD_MAX_PATH];
    SDSprintf(file_path, "%s%s", path, "skill_levelup.xml");
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
