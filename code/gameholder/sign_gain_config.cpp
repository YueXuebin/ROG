//auto created by structual_xml.py
#include "gameholder_pch.h"
#include "sign_gain_config.h"

void SIGN_GAIN_ROOT_MONTH_DAY_STRUCT::parse(TiXmlElement* xmlRoot)
{
    if (!xmlRoot) return;
    if (xmlRoot->Attribute("itemId"))
    {
        itemId = (int32)atoi(xmlRoot->Attribute("itemId"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "itemId");
    }
    if (xmlRoot->Attribute("itemNum"))
    {
        itemNum = (int32)atoi(xmlRoot->Attribute("itemNum"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "itemNum");
    }
    if (xmlRoot->Attribute("vip"))
    {
        vip = (int32)atoi(xmlRoot->Attribute("vip"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "vip");
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
        name = (int32)atoi(xmlRoot->Attribute("name"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "name");
    }
}

void SIGN_GAIN_ROOT_MONTH_STRUCT::parse(TiXmlElement* xmlRoot)
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
    if (xmlRoot->Attribute("id"))
    {
        id = (int32)atoi(xmlRoot->Attribute("id"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "id");
    }
    for(TiXmlElement* childEle = xmlRoot->FirstChildElement(); childEle; childEle = childEle->NextSiblingElement())
    {
        if (Crown::SDStrcmp(childEle->Value(), "day") == 0)
        {
            SIGN_GAIN_ROOT_MONTH_DAY_STRUCT StructTplt;
            StructTplt.clear();
            StructTplt.parse(childEle);
            day_list.push_back(StructTplt);
            continue;
        }
    }
}

void SIGN_GAIN_ROOT_STRUCT::parse(TiXmlElement* xmlRoot)
{
    if (!xmlRoot) return;
    for(TiXmlElement* childEle = xmlRoot->FirstChildElement(); childEle; childEle = childEle->NextSiblingElement())
    {
        if (Crown::SDStrcmp(childEle->Value(), "month") == 0)
        {
            SIGN_GAIN_ROOT_MONTH_STRUCT StructTplt;
            StructTplt.clear();
            StructTplt.parse(childEle);
            month_list.push_back(StructTplt);
            continue;
        }
    }
}

IMPLEMENT_SINGLETON(SIGN_GAIN_ENTRY)

bool SIGN_GAIN_ENTRY::LoadConfig(const char* path)
{
    char file_path[SD_MAX_PATH];
    SDSprintf(file_path, "%s%s", path, "sign_gain.xml");
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
