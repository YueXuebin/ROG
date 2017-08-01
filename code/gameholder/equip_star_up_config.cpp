//auto created by structual_xml.py
#include "gameholder_pch.h"
#include "equip_star_up_config.h"

void EQUIP_STAR_UP_ROOT_HEAD_STRUCT::parse(TiXmlElement* xmlRoot)
{
    if (!xmlRoot) return;
    if (xmlRoot->Attribute("star"))
    {
        star = (int32)atoi(xmlRoot->Attribute("star"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "star");
    }
    if (xmlRoot->Attribute("price"))
    {
        price = (int32)atoi(xmlRoot->Attribute("price"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "price");
    }
    if (xmlRoot->Attribute("percent"))
    {
        percent = (int32)atoi(xmlRoot->Attribute("percent"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "percent");
    }
    if (xmlRoot->Attribute("success_rate"))
    {
        success_rate = (int32)atoi(xmlRoot->Attribute("success_rate"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "success_rate");
    }
    if (xmlRoot->Attribute("num"))
    {
        num = (int32)atoi(xmlRoot->Attribute("num"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "num");
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

void EQUIP_STAR_UP_ROOT_STRUCT::parse(TiXmlElement* xmlRoot)
{
    if (!xmlRoot) return;
    for(TiXmlElement* childEle = xmlRoot->FirstChildElement(); childEle; childEle = childEle->NextSiblingElement())
    {
        if (Crown::SDStrcmp(childEle->Value(), "head") == 0)
        {
            EQUIP_STAR_UP_ROOT_HEAD_STRUCT StructTplt;
            StructTplt.clear();
            StructTplt.parse(childEle);
            head_list.push_back(StructTplt);
            continue;
        }
    }
}

IMPLEMENT_SINGLETON(EQUIP_STAR_UP_ENTRY)

bool EQUIP_STAR_UP_ENTRY::LoadConfig(const char* path)
{
    char file_path[SD_MAX_PATH];
    SDSprintf(file_path, "%s%s", path, "equip_star_up.xml");
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
