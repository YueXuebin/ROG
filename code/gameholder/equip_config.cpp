//auto created by structual_xml.py
#include "gameholder_pch.h"
#include "equip_config.h"

void EQUIP_ROOT_TEMPLATE_STRUCT::parse(TiXmlElement* xmlRoot)
{
    if (!xmlRoot) return;
    if (xmlRoot->Attribute("eqp_type"))
    {
        eqp_type = (int32)atoi(xmlRoot->Attribute("eqp_type"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "eqp_type");
    }
    if (xmlRoot->Attribute("name"))
    {
        name = (xmlRoot->Attribute("name"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "name");
    }
    if (xmlRoot->Attribute("female_eqp"))
    {
        female_eqp = (xmlRoot->Attribute("female_eqp"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "female_eqp");
    }
    if (xmlRoot->Attribute("eqp_level"))
    {
        eqp_level = (int32)atoi(xmlRoot->Attribute("eqp_level"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "eqp_level");
    }
    if (xmlRoot->Attribute("id"))
    {
        id = (int32)atoi(xmlRoot->Attribute("id"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "id");
    }
    if (xmlRoot->Attribute("male_eqp"))
    {
        male_eqp = (xmlRoot->Attribute("male_eqp"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "male_eqp");
    }
    if (xmlRoot->Attribute("grade"))
    {
        grade = (int32)atoi(xmlRoot->Attribute("grade"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "grade");
    }
    if (xmlRoot->Attribute("prop_key_2"))
    {
        prop_key_2 = (int32)atoi(xmlRoot->Attribute("prop_key_2"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "prop_key_2");
    }
    if (xmlRoot->Attribute("part_type"))
    {
        part_type = (int32)atoi(xmlRoot->Attribute("part_type"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "part_type");
    }
    if (xmlRoot->Attribute("prop_key_1"))
    {
        prop_key_1 = (int32)atoi(xmlRoot->Attribute("prop_key_1"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "prop_key_1");
    }
    if (xmlRoot->Attribute("prop_value_1"))
    {
        prop_value_1 = (int32)atoi(xmlRoot->Attribute("prop_value_1"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "prop_value_1");
    }
    if (xmlRoot->Attribute("prop_value_2"))
    {
        prop_value_2 = (int32)atoi(xmlRoot->Attribute("prop_value_2"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "prop_value_2");
    }
    if (xmlRoot->Attribute("sub_type"))
    {
        sub_type = (int32)atoi(xmlRoot->Attribute("sub_type"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "sub_type");
    }
    if (xmlRoot->Attribute("max_hole"))
    {
        max_hole = (int32)atoi(xmlRoot->Attribute("max_hole"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "max_hole");
    }
}

void EQUIP_ROOT_STRUCT::parse(TiXmlElement* xmlRoot)
{
    if (!xmlRoot) return;
    for(TiXmlElement* childEle = xmlRoot->FirstChildElement(); childEle; childEle = childEle->NextSiblingElement())
    {
        if (Crown::SDStrcmp(childEle->Value(), "template") == 0)
        {
            EQUIP_ROOT_TEMPLATE_STRUCT StructTplt;
            StructTplt.clear();
            StructTplt.parse(childEle);
            template_list.push_back(StructTplt);
            continue;
        }
    }
}

IMPLEMENT_SINGLETON(EQUIP_ENTRY)

bool EQUIP_ENTRY::LoadConfig(const char* path)
{
    char file_path[SD_MAX_PATH];
    SDSprintf(file_path, "%s%s", path, "equip.xml");
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
