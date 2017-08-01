//auto created by structual_xml.py
#include "gameholder_pch.h"
#include "alchemy_config.h"

void ALCHEMY_ROOT_ALCHEMY_STRUCT::parse(TiXmlElement* xmlRoot)
{
    if (!xmlRoot) return;
    if (xmlRoot->Attribute("dst_quality"))
    {
        dst_quality = (int32)atoi(xmlRoot->Attribute("dst_quality"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "dst_quality");
    }
    if (xmlRoot->Attribute("dst_item_id"))
    {
        dst_item_id = (int32)atoi(xmlRoot->Attribute("dst_item_id"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "dst_item_id");
    }
    if (xmlRoot->Attribute("item1_id"))
    {
        item1_id = (int32)atoi(xmlRoot->Attribute("item1_id"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "item1_id");
    }
    if (xmlRoot->Attribute("item2_id"))
    {
        item2_id = (int32)atoi(xmlRoot->Attribute("item2_id"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "item2_id");
    }
    if (xmlRoot->Attribute("item2_num"))
    {
        item2_num = (int32)atoi(xmlRoot->Attribute("item2_num"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "item2_num");
    }
    if (xmlRoot->Attribute("item4_id"))
    {
        item4_id = (int32)atoi(xmlRoot->Attribute("item4_id"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "item4_id");
    }
    if (xmlRoot->Attribute("item1_num"))
    {
        item1_num = (int32)atoi(xmlRoot->Attribute("item1_num"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "item1_num");
    }
    if (xmlRoot->Attribute("item3_id"))
    {
        item3_id = (int32)atoi(xmlRoot->Attribute("item3_id"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "item3_id");
    }
    if (xmlRoot->Attribute("item3_num"))
    {
        item3_num = (int32)atoi(xmlRoot->Attribute("item3_num"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "item3_num");
    }
    if (xmlRoot->Attribute("id"))
    {
        id = (int32)atoi(xmlRoot->Attribute("id"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "id");
    }
    if (xmlRoot->Attribute("item4_num"))
    {
        item4_num = (int32)atoi(xmlRoot->Attribute("item4_num"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "item4_num");
    }
}

void ALCHEMY_ROOT_STRUCT::parse(TiXmlElement* xmlRoot)
{
    if (!xmlRoot) return;
    for(TiXmlElement* childEle = xmlRoot->FirstChildElement(); childEle; childEle = childEle->NextSiblingElement())
    {
        if (Crown::SDStrcmp(childEle->Value(), "alchemy") == 0)
        {
            ALCHEMY_ROOT_ALCHEMY_STRUCT StructTplt;
            StructTplt.clear();
            StructTplt.parse(childEle);
            alchemy_list.push_back(StructTplt);
            continue;
        }
    }
}

IMPLEMENT_SINGLETON(ALCHEMY_ENTRY)

bool ALCHEMY_ENTRY::LoadConfig(const char* path)
{
    char file_path[SD_MAX_PATH];
    SDSprintf(file_path, "%s%s", path, "alchemy.xml");
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
