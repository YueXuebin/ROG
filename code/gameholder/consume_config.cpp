//auto created by structual_xml.py
#include "gameholder_pch.h"
#include "consume_config.h"

void CONSUME_ROOT_TEMPLATE_STRUCT::parse(TiXmlElement* xmlRoot)
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
    if (xmlRoot->Attribute("cd_type"))
    {
        cd_type = (int32)atoi(xmlRoot->Attribute("cd_type"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "cd_type");
    }
    if (xmlRoot->Attribute("cd_time"))
    {
        cd_time = (int32)atoi(xmlRoot->Attribute("cd_time"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "cd_time");
    }
    if (xmlRoot->Attribute("effect"))
    {
        effect = (xmlRoot->Attribute("effect"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "effect");
    }
    if (xmlRoot->Attribute("max_stack"))
    {
        max_stack = (int32)atoi(xmlRoot->Attribute("max_stack"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "max_stack");
    }
    if (xmlRoot->Attribute("id"))
    {
        id = (int32)atoi(xmlRoot->Attribute("id"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "id");
    }
    if (xmlRoot->Attribute("lv"))
    {
        lv = (int32)atoi(xmlRoot->Attribute("lv"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "lv");
    }
    if (xmlRoot->Attribute("sub_type"))
    {
        sub_type = (int32)atoi(xmlRoot->Attribute("sub_type"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "sub_type");
    }
    if (xmlRoot->Attribute("continued_time"))
    {
        continued_time = (int32)atoi(xmlRoot->Attribute("continued_time"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "continued_time");
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

void CONSUME_ROOT_STRUCT::parse(TiXmlElement* xmlRoot)
{
    if (!xmlRoot) return;
    for(TiXmlElement* childEle = xmlRoot->FirstChildElement(); childEle; childEle = childEle->NextSiblingElement())
    {
        if (Crown::SDStrcmp(childEle->Value(), "template") == 0)
        {
            CONSUME_ROOT_TEMPLATE_STRUCT StructTplt;
            StructTplt.clear();
            StructTplt.parse(childEle);
            template_list.push_back(StructTplt);
            continue;
        }
    }
}

IMPLEMENT_SINGLETON(CONSUME_ENTRY)

bool CONSUME_ENTRY::LoadConfig(const char* path)
{
    char file_path[SD_MAX_PATH];
    SDSprintf(file_path, "%s%s", path, "consume.xml");
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
