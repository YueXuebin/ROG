//auto created by structual_xml.py
#include "gameholder_pch.h"
#include "equip_quality_attribute_config.h"

void EQUIP_QUALITY_ATTRIBUTE_ROOT_PROP_STRUCT::parse(TiXmlElement* xmlRoot)
{
    if (!xmlRoot) return;
    if (xmlRoot->Attribute("class_14"))
    {
        class_14 = (int32)atoi(xmlRoot->Attribute("class_14"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "class_14");
    }
    if (xmlRoot->Attribute("class_10"))
    {
        class_10 = (int32)atoi(xmlRoot->Attribute("class_10"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "class_10");
    }
    if (xmlRoot->Attribute("class_11"))
    {
        class_11 = (int32)atoi(xmlRoot->Attribute("class_11"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "class_11");
    }
    if (xmlRoot->Attribute("class_12"))
    {
        class_12 = (int32)atoi(xmlRoot->Attribute("class_12"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "class_12");
    }
    if (xmlRoot->Attribute("class_13"))
    {
        class_13 = (int32)atoi(xmlRoot->Attribute("class_13"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "class_13");
    }
    if (xmlRoot->Attribute("class_2"))
    {
        class_2 = (int32)atoi(xmlRoot->Attribute("class_2"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "class_2");
    }
    if (xmlRoot->Attribute("class_3"))
    {
        class_3 = (int32)atoi(xmlRoot->Attribute("class_3"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "class_3");
    }
    if (xmlRoot->Attribute("class_1"))
    {
        class_1 = (int32)atoi(xmlRoot->Attribute("class_1"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "class_1");
    }
    if (xmlRoot->Attribute("class_6"))
    {
        class_6 = (int32)atoi(xmlRoot->Attribute("class_6"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "class_6");
    }
    if (xmlRoot->Attribute("class_7"))
    {
        class_7 = (int32)atoi(xmlRoot->Attribute("class_7"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "class_7");
    }
    if (xmlRoot->Attribute("class_4"))
    {
        class_4 = (int32)atoi(xmlRoot->Attribute("class_4"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "class_4");
    }
    if (xmlRoot->Attribute("class_5"))
    {
        class_5 = (int32)atoi(xmlRoot->Attribute("class_5"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "class_5");
    }
    if (xmlRoot->Attribute("class_8"))
    {
        class_8 = (int32)atoi(xmlRoot->Attribute("class_8"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "class_8");
    }
    if (xmlRoot->Attribute("class_9"))
    {
        class_9 = (int32)atoi(xmlRoot->Attribute("class_9"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "class_9");
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

void EQUIP_QUALITY_ATTRIBUTE_ROOT_STRUCT::parse(TiXmlElement* xmlRoot)
{
    if (!xmlRoot) return;
    for(TiXmlElement* childEle = xmlRoot->FirstChildElement(); childEle; childEle = childEle->NextSiblingElement())
    {
        if (Crown::SDStrcmp(childEle->Value(), "prop") == 0)
        {
            EQUIP_QUALITY_ATTRIBUTE_ROOT_PROP_STRUCT StructTplt;
            StructTplt.clear();
            StructTplt.parse(childEle);
            prop_list.push_back(StructTplt);
            continue;
        }
    }
}

IMPLEMENT_SINGLETON(EQUIP_QUALITY_ATTRIBUTE_ENTRY)

bool EQUIP_QUALITY_ATTRIBUTE_ENTRY::LoadConfig(const char* path)
{
    char file_path[SD_MAX_PATH];
    SDSprintf(file_path, "%s%s", path, "equip_quality_attribute.xml");
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
