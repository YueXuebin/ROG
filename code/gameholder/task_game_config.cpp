//auto created by structual_xml.py
#include "gameholder_pch.h"
#include "task_game_config.h"

void TASK_GAME_ROOT_TASK_STRUCT::parse(TiXmlElement* xmlRoot)
{
    if (!xmlRoot) return;
    if (xmlRoot->Attribute("task_reward"))
    {
        task_reward = (xmlRoot->Attribute("task_reward"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "task_reward");
    }
    if (xmlRoot->Attribute("archer_reward"))
    {
        archer_reward = (xmlRoot->Attribute("archer_reward"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "archer_reward");
    }
    if (xmlRoot->Attribute("task_type"))
    {
        task_type = (int32)atoi(xmlRoot->Attribute("task_type"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "task_type");
    }
    if (xmlRoot->Attribute("task_id"))
    {
        task_id = (int32)atoi(xmlRoot->Attribute("task_id"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "task_id");
    }
    if (xmlRoot->Attribute("mage_reward"))
    {
        mage_reward = (xmlRoot->Attribute("mage_reward"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "mage_reward");
    }
    if (xmlRoot->Attribute("task_target"))
    {
        task_target = (xmlRoot->Attribute("task_target"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "task_target");
    }
    if (xmlRoot->Attribute("pre_condition"))
    {
        pre_condition = (xmlRoot->Attribute("pre_condition"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "pre_condition");
    }
    if (xmlRoot->Attribute("warrior_reward"))
    {
        warrior_reward = (xmlRoot->Attribute("warrior_reward"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "warrior_reward");
    }
    if (xmlRoot->Attribute("task_name"))
    {
        task_name = (xmlRoot->Attribute("task_name"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "task_name");
    }
    if (xmlRoot->Attribute("task_desc"))
    {
        task_desc = (xmlRoot->Attribute("task_desc"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "task_desc");
    }
    if (xmlRoot->Attribute("sub_type"))
    {
        sub_type = (int32)atoi(xmlRoot->Attribute("sub_type"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "sub_type");
    }
}

void TASK_GAME_ROOT_STRUCT::parse(TiXmlElement* xmlRoot)
{
    if (!xmlRoot) return;
    for(TiXmlElement* childEle = xmlRoot->FirstChildElement(); childEle; childEle = childEle->NextSiblingElement())
    {
        if (Crown::SDStrcmp(childEle->Value(), "task") == 0)
        {
            TASK_GAME_ROOT_TASK_STRUCT StructTplt;
            StructTplt.clear();
            StructTplt.parse(childEle);
            task_list.push_back(StructTplt);
            continue;
        }
    }
}

IMPLEMENT_SINGLETON(TASK_GAME_ENTRY)

bool TASK_GAME_ENTRY::LoadConfig(const char* path)
{
    char file_path[SD_MAX_PATH];
    SDSprintf(file_path, "%s%s", path, "task_game.xml");
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
