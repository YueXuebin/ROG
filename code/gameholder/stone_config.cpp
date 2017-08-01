//auto created by structual_xml.py
#include "gameholder_pch.h"
#include "stone_config.h"

void STONE_ROOT_TEMPLATE_STRUCT::parse(TiXmlElement* xmlRoot)
{
    if (!xmlRoot) return;
    if (xmlRoot->Attribute("func_id"))
    {
        func_id = (int32)atoi(xmlRoot->Attribute("func_id"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "func_id");
    }
    if (xmlRoot->Attribute("desc"))
    {
        desc = (xmlRoot->Attribute("desc"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "desc");
    }
    if (xmlRoot->Attribute("id"))
    {
        id = (int32)atoi(xmlRoot->Attribute("id"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "id");
    }
    if (xmlRoot->Attribute("name"))
    {
        name = (xmlRoot->Attribute("name"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "name");
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

void STONE_ROOT_STRUCT::parse(TiXmlElement* xmlRoot)
{
    if (!xmlRoot) return;
    for(TiXmlElement* childEle = xmlRoot->FirstChildElement(); childEle; childEle = childEle->NextSiblingElement())
    {
        if (Crown::SDStrcmp(childEle->Value(), "template") == 0)
        {
            STONE_ROOT_TEMPLATE_STRUCT StructTplt;
            StructTplt.clear();
            StructTplt.parse(childEle);
            template_list.push_back(StructTplt);
            continue;
        }
    }
}

IMPLEMENT_SINGLETON(STONE_ENTRY)

bool STONE_ENTRY::LoadConfig(const char* path)
{
    char file_path[SD_MAX_PATH];
    SDSprintf(file_path, "%s%s", path, "stone.xml");
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
