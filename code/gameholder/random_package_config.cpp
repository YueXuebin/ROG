//auto created by structual_xml.py
#include "gameholder_pch.h"
#include "random_package_config.h"

void RANDOM_PACKAGE_ROOT_PACKAGE_ITEM_STRUCT::parse(TiXmlElement* xmlRoot)
{
    if (!xmlRoot) return;
    if (xmlRoot->Attribute("server_cd"))
    {
        server_cd = (xmlRoot->Attribute("server_cd"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "server_cd");
    }
    if (xmlRoot->Attribute("reward_name"))
    {
        reward_name = (xmlRoot->Attribute("reward_name"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "reward_name");
    }
    if (xmlRoot->Attribute("get_times"))
    {
        get_times = (int32)atoi(xmlRoot->Attribute("get_times"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "get_times");
    }
    if (xmlRoot->Attribute("card_id"))
    {
        card_id = (int32)atoi(xmlRoot->Attribute("card_id"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "card_id");
    }
    if (xmlRoot->Attribute("min_val"))
    {
        min_val = (int32)atoi(xmlRoot->Attribute("min_val"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "min_val");
    }
    if (xmlRoot->Attribute("max_val"))
    {
        max_val = (int32)atoi(xmlRoot->Attribute("max_val"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "max_val");
    }
    if (xmlRoot->Attribute("rate"))
    {
        rate = (int32)atoi(xmlRoot->Attribute("rate"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "rate");
    }
    if (xmlRoot->Attribute("personal_cd"))
    {
        personal_cd = (xmlRoot->Attribute("personal_cd"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "personal_cd");
    }
    if (xmlRoot->Attribute("is_repeat"))
    {
        is_repeat = (int32)atoi(xmlRoot->Attribute("is_repeat"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "is_repeat");
    }
    if (xmlRoot->Attribute("is_send_notice"))
    {
        is_send_notice = (int32)atoi(xmlRoot->Attribute("is_send_notice"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "is_send_notice");
    }
    if (xmlRoot->Attribute("reward_id"))
    {
        reward_id = (int32)atoi(xmlRoot->Attribute("reward_id"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "reward_id");
    }
}

void RANDOM_PACKAGE_ROOT_PACKAGE_STRUCT::parse(TiXmlElement* xmlRoot)
{
    if (!xmlRoot) return;
    if (xmlRoot->Attribute("random_num"))
    {
        random_num = (int32)atoi(xmlRoot->Attribute("random_num"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "random_num");
    }
    if (xmlRoot->Attribute("id"))
    {
        id = (int32)atoi(xmlRoot->Attribute("id"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "id");
    }
    if (xmlRoot->Attribute("package_name"))
    {
        package_name = (xmlRoot->Attribute("package_name"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "package_name");
    }
    for(TiXmlElement* childEle = xmlRoot->FirstChildElement(); childEle; childEle = childEle->NextSiblingElement())
    {
        if (Crown::SDStrcmp(childEle->Value(), "item") == 0)
        {
            RANDOM_PACKAGE_ROOT_PACKAGE_ITEM_STRUCT StructTplt;
            StructTplt.clear();
            StructTplt.parse(childEle);
            item_list.push_back(StructTplt);
            continue;
        }
    }
}

void RANDOM_PACKAGE_ROOT_STRUCT::parse(TiXmlElement* xmlRoot)
{
    if (!xmlRoot) return;
    for(TiXmlElement* childEle = xmlRoot->FirstChildElement(); childEle; childEle = childEle->NextSiblingElement())
    {
        if (Crown::SDStrcmp(childEle->Value(), "package") == 0)
        {
            RANDOM_PACKAGE_ROOT_PACKAGE_STRUCT StructTplt;
            StructTplt.clear();
            StructTplt.parse(childEle);
            package_list.push_back(StructTplt);
            continue;
        }
    }
}

IMPLEMENT_SINGLETON(RANDOM_PACKAGE_ENTRY)

bool RANDOM_PACKAGE_ENTRY::LoadConfig(const char* path)
{
    char file_path[SD_MAX_PATH];
    SDSprintf(file_path, "%s%s", path, "random_package.xml");
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
