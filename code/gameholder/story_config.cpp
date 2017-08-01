//auto created by structual_xml.py
#include "gameholder_pch.h"
#include "story_config.h"

void STORY_ROOT_STORY_DIALOG_STRUCT::parse(TiXmlElement* xmlRoot)
{
    if (!xmlRoot) return;
    if (xmlRoot->Attribute("left_character"))
    {
        left_character = (xmlRoot->Attribute("left_character"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "left_character");
    }
    if (xmlRoot->Attribute("name"))
    {
        name = (xmlRoot->Attribute("name"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "name");
    }
    if (xmlRoot->Attribute("tell_wait"))
    {
        tell_wait = (int32)atoi(xmlRoot->Attribute("tell_wait"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "tell_wait");
    }
    if (xmlRoot->Attribute("left_eqp"))
    {
        left_eqp = (xmlRoot->Attribute("left_eqp"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "left_eqp");
    }
    if (xmlRoot->Attribute("item_icon"))
    {
        item_icon = (int32)atoi(xmlRoot->Attribute("item_icon"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "item_icon");
    }
    if (xmlRoot->Attribute("right_character"))
    {
        right_character = (xmlRoot->Attribute("right_character"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "right_character");
    }
    if (xmlRoot->Attribute("right_eqp"))
    {
        right_eqp = (xmlRoot->Attribute("right_eqp"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "right_eqp");
    }
    if (xmlRoot->Attribute("tell"))
    {
        tell = (xmlRoot->Attribute("tell"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "tell");
    }
}

void STORY_ROOT_STORY_STRUCT::parse(TiXmlElement* xmlRoot)
{
    if (!xmlRoot) return;
    if (xmlRoot->Attribute("trigger_type"))
    {
        trigger_type = (int32)atoi(xmlRoot->Attribute("trigger_type"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "trigger_type");
    }
    if (xmlRoot->Attribute("trigger_delay"))
    {
        trigger_delay = (int32)atoi(xmlRoot->Attribute("trigger_delay"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "trigger_delay");
    }
    if (xmlRoot->Attribute("pos"))
    {
        pos = (int32)atoi(xmlRoot->Attribute("pos"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "pos");
    }
    if (xmlRoot->Attribute("trigger_arg4"))
    {
        trigger_arg4 = (int32)atoi(xmlRoot->Attribute("trigger_arg4"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "trigger_arg4");
    }
    if (xmlRoot->Attribute("trigger_arg3"))
    {
        trigger_arg3 = (int32)atoi(xmlRoot->Attribute("trigger_arg3"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "trigger_arg3");
    }
    if (xmlRoot->Attribute("trigger_arg2"))
    {
        trigger_arg2 = (int32)atoi(xmlRoot->Attribute("trigger_arg2"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "trigger_arg2");
    }
    if (xmlRoot->Attribute("trigger_arg1"))
    {
        trigger_arg1 = (int32)atoi(xmlRoot->Attribute("trigger_arg1"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "trigger_arg1");
    }
    for(TiXmlElement* childEle = xmlRoot->FirstChildElement(); childEle; childEle = childEle->NextSiblingElement())
    {
        if (Crown::SDStrcmp(childEle->Value(), "dialog") == 0)
        {
            STORY_ROOT_STORY_DIALOG_STRUCT StructTplt;
            StructTplt.clear();
            StructTplt.parse(childEle);
            dialog_list.push_back(StructTplt);
            continue;
        }
    }
}

void STORY_ROOT_STRUCT::parse(TiXmlElement* xmlRoot)
{
    if (!xmlRoot) return;
    for(TiXmlElement* childEle = xmlRoot->FirstChildElement(); childEle; childEle = childEle->NextSiblingElement())
    {
        if (Crown::SDStrcmp(childEle->Value(), "story") == 0)
        {
            STORY_ROOT_STORY_STRUCT StructTplt;
            StructTplt.clear();
            StructTplt.parse(childEle);
            story_list.push_back(StructTplt);
            continue;
        }
    }
}

IMPLEMENT_SINGLETON(STORY_ENTRY)

bool STORY_ENTRY::LoadConfig(const char* path)
{
    char file_path[SD_MAX_PATH];
    SDSprintf(file_path, "%s%s", path, "story.xml");
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
