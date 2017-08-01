//auto created by structual_xml.py
#include "gameholder_pch.h"
#include "levelup_config.h"

void LEVELUP_ROOT_LEVEL_STRUCT::parse(TiXmlElement* xmlRoot)
{
    if (!xmlRoot) return;
    if (xmlRoot->Attribute("dodge"))
    {
        dodge = (int32)atoi(xmlRoot->Attribute("dodge"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "dodge");
    }
    if (xmlRoot->Attribute("attack_speed"))
    {
        attack_speed = (int32)atoi(xmlRoot->Attribute("attack_speed"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "attack_speed");
    }
    if (xmlRoot->Attribute("armor"))
    {
        armor = (int32)atoi(xmlRoot->Attribute("armor"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "armor");
    }
    if (xmlRoot->Attribute("hit"))
    {
        hit = (int32)atoi(xmlRoot->Attribute("hit"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "hit");
    }
    if (xmlRoot->Attribute("lightning_resistance"))
    {
        lightning_resistance = (int32)atoi(xmlRoot->Attribute("lightning_resistance"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "lightning_resistance");
    }
    if (xmlRoot->Attribute("level"))
    {
        level = (int32)atoi(xmlRoot->Attribute("level"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "level");
    }
    if (xmlRoot->Attribute("mp_recovery"))
    {
        mp_recovery = (int32)atoi(xmlRoot->Attribute("mp_recovery"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "mp_recovery");
    }
    if (xmlRoot->Attribute("move_speed"))
    {
        move_speed = (int32)atoi(xmlRoot->Attribute("move_speed"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "move_speed");
    }
    if (xmlRoot->Attribute("frost_resistance"))
    {
        frost_resistance = (int32)atoi(xmlRoot->Attribute("frost_resistance"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "frost_resistance");
    }
    if (xmlRoot->Attribute("fire_resistance"))
    {
        fire_resistance = (int32)atoi(xmlRoot->Attribute("fire_resistance"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "fire_resistance");
    }
    if (xmlRoot->Attribute("cast_speed"))
    {
        cast_speed = (int32)atoi(xmlRoot->Attribute("cast_speed"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "cast_speed");
    }
    if (xmlRoot->Attribute("energy_shield"))
    {
        energy_shield = (int32)atoi(xmlRoot->Attribute("energy_shield"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "energy_shield");
    }
    if (xmlRoot->Attribute("magic_block"))
    {
        magic_block = (int32)atoi(xmlRoot->Attribute("magic_block"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "magic_block");
    }
    if (xmlRoot->Attribute("physics_damage"))
    {
        physics_damage = (int32)atoi(xmlRoot->Attribute("physics_damage"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "physics_damage");
    }
    if (xmlRoot->Attribute("mp"))
    {
        mp = (int32)atoi(xmlRoot->Attribute("mp"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "mp");
    }
    if (xmlRoot->Attribute("attack_block"))
    {
        attack_block = (int32)atoi(xmlRoot->Attribute("attack_block"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "attack_block");
    }
    if (xmlRoot->Attribute("max_exp"))
    {
        max_exp = (int32)atoi(xmlRoot->Attribute("max_exp"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "max_exp");
    }
    if (xmlRoot->Attribute("crit"))
    {
        crit = (int32)atoi(xmlRoot->Attribute("crit"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "crit");
    }
    if (xmlRoot->Attribute("hp_recovery"))
    {
        hp_recovery = (int32)atoi(xmlRoot->Attribute("hp_recovery"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "hp_recovery");
    }
    if (xmlRoot->Attribute("hp"))
    {
        hp = (int32)atoi(xmlRoot->Attribute("hp"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "hp");
    }
    if (xmlRoot->Attribute("crit_damage"))
    {
        crit_damage = (int32)atoi(xmlRoot->Attribute("crit_damage"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "crit_damage");
    }
}

void LEVELUP_ROOT_STRUCT::parse(TiXmlElement* xmlRoot)
{
    if (!xmlRoot) return;
    for(TiXmlElement* childEle = xmlRoot->FirstChildElement(); childEle; childEle = childEle->NextSiblingElement())
    {
        if (Crown::SDStrcmp(childEle->Value(), "level") == 0)
        {
            LEVELUP_ROOT_LEVEL_STRUCT StructTplt;
            StructTplt.clear();
            StructTplt.parse(childEle);
            level_list.push_back(StructTplt);
            continue;
        }
    }
}

IMPLEMENT_SINGLETON(LEVELUP_ENTRY)

bool LEVELUP_ENTRY::LoadConfig(const char* path)
{
    char file_path[SD_MAX_PATH];
    SDSprintf(file_path, "%s%s", path, "levelup.xml");
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
