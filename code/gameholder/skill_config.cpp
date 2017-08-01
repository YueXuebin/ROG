//auto created by structual_xml.py
#include "gameholder_pch.h"
#include "skill_config.h"

void SKILL_ROOT_SKILL_STRUCT::parse(TiXmlElement* xmlRoot)
{
    if (!xmlRoot) return;
    if (xmlRoot->Attribute("prop2_grow"))
    {
        prop2_grow = (int32)atoi(xmlRoot->Attribute("prop2_grow"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "prop2_grow");
    }
    if (xmlRoot->Attribute("feature"))
    {
        feature = (int32)atoi(xmlRoot->Attribute("feature"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "feature");
    }
    if (xmlRoot->Attribute("cooldown"))
    {
        cooldown = (int32)atoi(xmlRoot->Attribute("cooldown"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "cooldown");
    }
    if (xmlRoot->Attribute("bullet_chain_id"))
    {
        bullet_chain_id = (int32)atoi(xmlRoot->Attribute("bullet_chain_id"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "bullet_chain_id");
    }
    if (xmlRoot->Attribute("bullet_repeat"))
    {
        bullet_repeat = (int32)atoi(xmlRoot->Attribute("bullet_repeat"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "bullet_repeat");
    }
    if (xmlRoot->Attribute("prop2_id"))
    {
        prop2_id = (int32)atoi(xmlRoot->Attribute("prop2_id"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "prop2_id");
    }
    if (xmlRoot->Attribute("attack_type"))
    {
        attack_type = (int32)atoi(xmlRoot->Attribute("attack_type"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "attack_type");
    }
    if (xmlRoot->Attribute("prop1_base"))
    {
        prop1_base = (int32)atoi(xmlRoot->Attribute("prop1_base"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "prop1_base");
    }
    if (xmlRoot->Attribute("cast_multi"))
    {
        cast_multi = (int32)atoi(xmlRoot->Attribute("cast_multi"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "cast_multi");
    }
    if (xmlRoot->Attribute("id"))
    {
        id = (int32)atoi(xmlRoot->Attribute("id"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "id");
    }
    if (xmlRoot->Attribute("bullet_multi_angle"))
    {
        bullet_multi_angle = (int32)atoi(xmlRoot->Attribute("bullet_multi_angle"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "bullet_multi_angle");
    }
    if (xmlRoot->Attribute("bullet_multi"))
    {
        bullet_multi = (int32)atoi(xmlRoot->Attribute("bullet_multi"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "bullet_multi");
    }
    if (xmlRoot->Attribute("to_friend"))
    {
        to_friend = (int32)atoi(xmlRoot->Attribute("to_friend"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "to_friend");
    }
    if (xmlRoot->Attribute("prop1_grow"))
    {
        prop1_grow = (int32)atoi(xmlRoot->Attribute("prop1_grow"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "prop1_grow");
    }
    if (xmlRoot->Attribute("buff"))
    {
        buff = (int32)atoi(xmlRoot->Attribute("buff"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "buff");
    }
    if (xmlRoot->Attribute("attack_speed"))
    {
        attack_speed = (int32)atoi(xmlRoot->Attribute("attack_speed"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "attack_speed");
    }
    if (xmlRoot->Attribute("bullet_chain"))
    {
        bullet_chain = (int32)atoi(xmlRoot->Attribute("bullet_chain"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "bullet_chain");
    }
    if (xmlRoot->Attribute("prop2_base"))
    {
        prop2_base = (int32)atoi(xmlRoot->Attribute("prop2_base"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "prop2_base");
    }
    if (xmlRoot->Attribute("buff_time"))
    {
        buff_time = (int32)atoi(xmlRoot->Attribute("buff_time"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "buff_time");
    }
    if (xmlRoot->Attribute("to_enemy"))
    {
        to_enemy = (int32)atoi(xmlRoot->Attribute("to_enemy"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "to_enemy");
    }
    if (xmlRoot->Attribute("buff_val1_base"))
    {
        buff_val1_base = (int32)atoi(xmlRoot->Attribute("buff_val1_base"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "buff_val1_base");
    }
    if (xmlRoot->Attribute("to_me"))
    {
        to_me = (int32)atoi(xmlRoot->Attribute("to_me"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "to_me");
    }
    if (xmlRoot->Attribute("desc"))
    {
        desc = (xmlRoot->Attribute("desc"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "desc");
    }
    if (xmlRoot->Attribute("prop1_id"))
    {
        prop1_id = (int32)atoi(xmlRoot->Attribute("prop1_id"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "prop1_id");
    }
    if (xmlRoot->Attribute("bullet_repeat_interval"))
    {
        bullet_repeat_interval = (int32)atoi(xmlRoot->Attribute("bullet_repeat_interval"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "bullet_repeat_interval");
    }
    if (xmlRoot->Attribute("name"))
    {
        name = (xmlRoot->Attribute("name"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "name");
    }
    if (xmlRoot->Attribute("buff_val1_grow"))
    {
        buff_val1_grow = (int32)atoi(xmlRoot->Attribute("buff_val1_grow"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "buff_val1_grow");
    }
    if (xmlRoot->Attribute("buff_val2_base"))
    {
        buff_val2_base = (int32)atoi(xmlRoot->Attribute("buff_val2_base"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "buff_val2_base");
    }
    if (xmlRoot->Attribute("mp"))
    {
        mp = (int32)atoi(xmlRoot->Attribute("mp"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "mp");
    }
    if (xmlRoot->Attribute("buff_val2_grow"))
    {
        buff_val2_grow = (int32)atoi(xmlRoot->Attribute("buff_val2_grow"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "buff_val2_grow");
    }
}

void SKILL_ROOT_STRUCT::parse(TiXmlElement* xmlRoot)
{
    if (!xmlRoot) return;
    for(TiXmlElement* childEle = xmlRoot->FirstChildElement(); childEle; childEle = childEle->NextSiblingElement())
    {
        if (Crown::SDStrcmp(childEle->Value(), "skill") == 0)
        {
            SKILL_ROOT_SKILL_STRUCT StructTplt;
            StructTplt.clear();
            StructTplt.parse(childEle);
            skill_list.push_back(StructTplt);
            continue;
        }
    }
}

IMPLEMENT_SINGLETON(SKILL_ENTRY)

bool SKILL_ENTRY::LoadConfig(const char* path)
{
    char file_path[SD_MAX_PATH];
    SDSprintf(file_path, "%s%s", path, "skill.xml");
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
