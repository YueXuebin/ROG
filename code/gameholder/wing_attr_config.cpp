//auto created by structual_xml.py
#include "gameholder_pch.h"
#include "wing_attr_config.h"

void WING_ATTR_ROOT_WING_ITEM_STRUCT::parse(TiXmlElement* xmlRoot)
{
    if (!xmlRoot) return;
    if (xmlRoot->Attribute("attr_2"))
    {
        attr_2 = (int32)atoi(xmlRoot->Attribute("attr_2"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "attr_2");
    }
    if (xmlRoot->Attribute("attr_3"))
    {
        attr_3 = (int32)atoi(xmlRoot->Attribute("attr_3"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "attr_3");
    }
    if (xmlRoot->Attribute("attr_1"))
    {
        attr_1 = (int32)atoi(xmlRoot->Attribute("attr_1"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "attr_1");
    }
    if (xmlRoot->Attribute("star"))
    {
        star = (int32)atoi(xmlRoot->Attribute("star"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "star");
    }
    if (xmlRoot->Attribute("attr_4"))
    {
        attr_4 = (int32)atoi(xmlRoot->Attribute("attr_4"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "attr_4");
    }
    if (xmlRoot->Attribute("level"))
    {
        level = (int32)atoi(xmlRoot->Attribute("level"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "level");
    }
    if (xmlRoot->Attribute("attr_type_2"))
    {
        attr_type_2 = (int32)atoi(xmlRoot->Attribute("attr_type_2"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "attr_type_2");
    }
    if (xmlRoot->Attribute("attr_type_3"))
    {
        attr_type_3 = (int32)atoi(xmlRoot->Attribute("attr_type_3"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "attr_type_3");
    }
    if (xmlRoot->Attribute("step"))
    {
        step = (int32)atoi(xmlRoot->Attribute("step"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "step");
    }
    if (xmlRoot->Attribute("attr_type_1"))
    {
        attr_type_1 = (int32)atoi(xmlRoot->Attribute("attr_type_1"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "attr_type_1");
    }
    if (xmlRoot->Attribute("attr_type_4"))
    {
        attr_type_4 = (int32)atoi(xmlRoot->Attribute("attr_type_4"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "attr_type_4");
    }
    if (xmlRoot->Attribute("num_1"))
    {
        num_1 = (int32)atoi(xmlRoot->Attribute("num_1"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "num_1");
    }
    if (xmlRoot->Attribute("item_id_1"))
    {
        item_id_1 = (int32)atoi(xmlRoot->Attribute("item_id_1"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "item_id_1");
    }
}

void WING_ATTR_ROOT_WING_STRUCT::parse(TiXmlElement* xmlRoot)
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
    if (xmlRoot->Attribute("eqp"))
    {
        eqp = (xmlRoot->Attribute("eqp"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "eqp");
    }
    if (xmlRoot->Attribute("item_id"))
    {
        item_id = (int32)atoi(xmlRoot->Attribute("item_id"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "item_id");
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
    if (xmlRoot->Attribute("icon"))
    {
        icon = (int32)atoi(xmlRoot->Attribute("icon"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "icon");
    }
    for(TiXmlElement* childEle = xmlRoot->FirstChildElement(); childEle; childEle = childEle->NextSiblingElement())
    {
        if (Crown::SDStrcmp(childEle->Value(), "item") == 0)
        {
            WING_ATTR_ROOT_WING_ITEM_STRUCT StructTplt;
            StructTplt.clear();
            StructTplt.parse(childEle);
            item_list.push_back(StructTplt);
            continue;
        }
    }
}

void WING_ATTR_ROOT_STRUCT::parse(TiXmlElement* xmlRoot)
{
    if (!xmlRoot) return;
    for(TiXmlElement* childEle = xmlRoot->FirstChildElement(); childEle; childEle = childEle->NextSiblingElement())
    {
        if (Crown::SDStrcmp(childEle->Value(), "wing") == 0)
        {
            WING_ATTR_ROOT_WING_STRUCT StructTplt;
            StructTplt.clear();
            StructTplt.parse(childEle);
            wing_list.push_back(StructTplt);
            continue;
        }
    }
}

IMPLEMENT_SINGLETON(WING_ATTR_ENTRY)

bool WING_ATTR_ENTRY::LoadConfig(const char* path)
{
    char file_path[SD_MAX_PATH];
    SDSprintf(file_path, "%s%s", path, "wing_attr.xml");
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
