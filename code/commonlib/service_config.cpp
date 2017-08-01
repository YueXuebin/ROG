//auto created by structual_xml.py
#include "commonlib_pch.h"
#include "service_config.h"

void SERVICE_ROOT_SERVICE_STRUCT::parse(TiXmlElement* xmlRoot)
{
    if (!xmlRoot) return;
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
}

void SERVICE_ROOT_STRUCT::parse(TiXmlElement* xmlRoot)
{
    if (!xmlRoot) return;
    for(TiXmlElement* childEle = xmlRoot->FirstChildElement(); childEle; childEle = childEle->NextSiblingElement())
    {
        if (Crown::SDStrcmp(childEle->Value(), "service") == 0)
        {
            SERVICE_ROOT_SERVICE_STRUCT StructTplt;
            StructTplt.clear();
            StructTplt.parse(childEle);
            service_list.push_back(StructTplt);
            continue;
        }
    }
}

IMPLEMENT_SINGLETON(SERVICE_ENTRY)

bool SERVICE_ENTRY::LoadConfig(const char* path)
{
    char file_path[SD_MAX_PATH];
    SDSprintf(file_path, "%s%s", path, "service.xml");
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
