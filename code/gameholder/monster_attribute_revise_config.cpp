//auto created by structual_xml.py
#include "gameholder_pch.h"
#include "monster_attribute_revise_config.h"

void MONSTER_ATTRIBUTE_REVISE_ROOT_MONSTER_STRUCT::parse(TiXmlElement* xmlRoot)
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
    if (xmlRoot->Attribute("magic_block"))
    {
        magic_block = (int32)atoi(xmlRoot->Attribute("magic_block"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "magic_block");
    }
    if (xmlRoot->Attribute("frost_resistance_rate"))
    {
        frost_resistance_rate = (int32)atoi(xmlRoot->Attribute("frost_resistance_rate"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "frost_resistance_rate");
    }
    if (xmlRoot->Attribute("ai"))
    {
        ai = (xmlRoot->Attribute("ai"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "ai");
    }
    if (xmlRoot->Attribute("dodge_rate"))
    {
        dodge_rate = (int32)atoi(xmlRoot->Attribute("dodge_rate"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "dodge_rate");
    }
    if (xmlRoot->Attribute("movie_time"))
    {
        movie_time = (int32)atoi(xmlRoot->Attribute("movie_time"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "movie_time");
    }
    if (xmlRoot->Attribute("radius"))
    {
        radius = (int32)atoi(xmlRoot->Attribute("radius"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "radius");
    }
    if (xmlRoot->Attribute("cast_speed_rate"))
    {
        cast_speed_rate = (int32)atoi(xmlRoot->Attribute("cast_speed_rate"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "cast_speed_rate");
    }
    if (xmlRoot->Attribute("crit"))
    {
        crit = (int32)atoi(xmlRoot->Attribute("crit"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "crit");
    }
    if (xmlRoot->Attribute("movie_name"))
    {
        movie_name = (xmlRoot->Attribute("movie_name"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "movie_name");
    }
    if (xmlRoot->Attribute("id"))
    {
        id = (int32)atoi(xmlRoot->Attribute("id"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "id");
    }
    if (xmlRoot->Attribute("move_speed_rate"))
    {
        move_speed_rate = (int32)atoi(xmlRoot->Attribute("move_speed_rate"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "move_speed_rate");
    }
    if (xmlRoot->Attribute("attack_speed"))
    {
        attack_speed = (int32)atoi(xmlRoot->Attribute("attack_speed"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "attack_speed");
    }
    if (xmlRoot->Attribute("attack_speed_rate"))
    {
        attack_speed_rate = (int32)atoi(xmlRoot->Attribute("attack_speed_rate"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "attack_speed_rate");
    }
    if (xmlRoot->Attribute("attack_block_rate"))
    {
        attack_block_rate = (int32)atoi(xmlRoot->Attribute("attack_block_rate"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "attack_block_rate");
    }
    if (xmlRoot->Attribute("mp_rate"))
    {
        mp_rate = (int32)atoi(xmlRoot->Attribute("mp_rate"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "mp_rate");
    }
    if (xmlRoot->Attribute("armor_rate"))
    {
        armor_rate = (int32)atoi(xmlRoot->Attribute("armor_rate"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "armor_rate");
    }
    if (xmlRoot->Attribute("gold"))
    {
        gold = (int32)atoi(xmlRoot->Attribute("gold"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "gold");
    }
    if (xmlRoot->Attribute("lightning_resistance"))
    {
        lightning_resistance = (int32)atoi(xmlRoot->Attribute("lightning_resistance"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "lightning_resistance");
    }
    if (xmlRoot->Attribute("armor"))
    {
        armor = (int32)atoi(xmlRoot->Attribute("armor"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "armor");
    }
    if (xmlRoot->Attribute("eqp"))
    {
        eqp = (xmlRoot->Attribute("eqp"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "eqp");
    }
    if (xmlRoot->Attribute("character"))
    {
        character = (xmlRoot->Attribute("character"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "character");
    }
    if (xmlRoot->Attribute("fire_resistance_rate"))
    {
        fire_resistance_rate = (int32)atoi(xmlRoot->Attribute("fire_resistance_rate"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "fire_resistance_rate");
    }
    if (xmlRoot->Attribute("energy_shield"))
    {
        energy_shield = (int32)atoi(xmlRoot->Attribute("energy_shield"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "energy_shield");
    }
    if (xmlRoot->Attribute("attack"))
    {
        attack = (int32)atoi(xmlRoot->Attribute("attack"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "attack");
    }
    if (xmlRoot->Attribute("attack_block"))
    {
        attack_block = (int32)atoi(xmlRoot->Attribute("attack_block"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "attack_block");
    }
    if (xmlRoot->Attribute("move_speed"))
    {
        move_speed = (int32)atoi(xmlRoot->Attribute("move_speed"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "move_speed");
    }
    if (xmlRoot->Attribute("hp_recovery"))
    {
        hp_recovery = (int32)atoi(xmlRoot->Attribute("hp_recovery"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "hp_recovery");
    }
    if (xmlRoot->Attribute("type"))
    {
        type = (int32)atoi(xmlRoot->Attribute("type"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "type");
    }
    if (xmlRoot->Attribute("frost_resistance"))
    {
        frost_resistance = (int32)atoi(xmlRoot->Attribute("frost_resistance"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "frost_resistance");
    }
    if (xmlRoot->Attribute("crit_damage"))
    {
        crit_damage = (int32)atoi(xmlRoot->Attribute("crit_damage"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "crit_damage");
    }
    if (xmlRoot->Attribute("mp_recovery_rate"))
    {
        mp_recovery_rate = (int32)atoi(xmlRoot->Attribute("mp_recovery_rate"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "mp_recovery_rate");
    }
    if (xmlRoot->Attribute("hit"))
    {
        hit = (int32)atoi(xmlRoot->Attribute("hit"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "hit");
    }
    if (xmlRoot->Attribute("energy_shield_rate"))
    {
        energy_shield_rate = (int32)atoi(xmlRoot->Attribute("energy_shield_rate"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "energy_shield_rate");
    }
    if (xmlRoot->Attribute("mp_recovery"))
    {
        mp_recovery = (int32)atoi(xmlRoot->Attribute("mp_recovery"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "mp_recovery");
    }
    if (xmlRoot->Attribute("hp"))
    {
        hp = (int32)atoi(xmlRoot->Attribute("hp"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "hp");
    }
    if (xmlRoot->Attribute("fire_resistance"))
    {
        fire_resistance = (int32)atoi(xmlRoot->Attribute("fire_resistance"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "fire_resistance");
    }
    if (xmlRoot->Attribute("magic_block_rate"))
    {
        magic_block_rate = (int32)atoi(xmlRoot->Attribute("magic_block_rate"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "magic_block_rate");
    }
    if (xmlRoot->Attribute("gold_rate"))
    {
        gold_rate = (int32)atoi(xmlRoot->Attribute("gold_rate"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "gold_rate");
    }
    if (xmlRoot->Attribute("cast_speed"))
    {
        cast_speed = (int32)atoi(xmlRoot->Attribute("cast_speed"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "cast_speed");
    }
    if (xmlRoot->Attribute("crit_rate"))
    {
        crit_rate = (int32)atoi(xmlRoot->Attribute("crit_rate"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "crit_rate");
    }
    if (xmlRoot->Attribute("lightning_resistance_rate"))
    {
        lightning_resistance_rate = (int32)atoi(xmlRoot->Attribute("lightning_resistance_rate"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "lightning_resistance_rate");
    }
    if (xmlRoot->Attribute("hp_recovery_rate"))
    {
        hp_recovery_rate = (int32)atoi(xmlRoot->Attribute("hp_recovery_rate"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "hp_recovery_rate");
    }
    if (xmlRoot->Attribute("exp_rate"))
    {
        exp_rate = (int32)atoi(xmlRoot->Attribute("exp_rate"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "exp_rate");
    }
    if (xmlRoot->Attribute("attack_rate"))
    {
        attack_rate = (int32)atoi(xmlRoot->Attribute("attack_rate"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "attack_rate");
    }
    if (xmlRoot->Attribute("name"))
    {
        name = (xmlRoot->Attribute("name"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "name");
    }
    if (xmlRoot->Attribute("package_id"))
    {
        package_id = (int32)atoi(xmlRoot->Attribute("package_id"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "package_id");
    }
    if (xmlRoot->Attribute("skill_1"))
    {
        skill_1 = (int32)atoi(xmlRoot->Attribute("skill_1"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "skill_1");
    }
    if (xmlRoot->Attribute("skill_2"))
    {
        skill_2 = (int32)atoi(xmlRoot->Attribute("skill_2"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "skill_2");
    }
    if (xmlRoot->Attribute("mp"))
    {
        mp = (int32)atoi(xmlRoot->Attribute("mp"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "mp");
    }
    if (xmlRoot->Attribute("exp"))
    {
        exp = (int32)atoi(xmlRoot->Attribute("exp"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "exp");
    }
    if (xmlRoot->Attribute("crit_damage_rate"))
    {
        crit_damage_rate = (int32)atoi(xmlRoot->Attribute("crit_damage_rate"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "crit_damage_rate");
    }
    if (xmlRoot->Attribute("hp_rate"))
    {
        hp_rate = (int32)atoi(xmlRoot->Attribute("hp_rate"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "hp_rate");
    }
    if (xmlRoot->Attribute("hit_rate"))
    {
        hit_rate = (int32)atoi(xmlRoot->Attribute("hit_rate"));
    }
    else
    {
        CnWarn("Can't find '%s' attribute '%s' \n", xmlRoot->Value(), "hit_rate");
    }
}

void MONSTER_ATTRIBUTE_REVISE_ROOT_STRUCT::parse(TiXmlElement* xmlRoot)
{
    if (!xmlRoot) return;
    for(TiXmlElement* childEle = xmlRoot->FirstChildElement(); childEle; childEle = childEle->NextSiblingElement())
    {
        if (Crown::SDStrcmp(childEle->Value(), "monster") == 0)
        {
            MONSTER_ATTRIBUTE_REVISE_ROOT_MONSTER_STRUCT StructTplt;
            StructTplt.clear();
            StructTplt.parse(childEle);
            monster_list.push_back(StructTplt);
            continue;
        }
    }
}

IMPLEMENT_SINGLETON(MONSTER_ATTRIBUTE_REVISE_ENTRY)

bool MONSTER_ATTRIBUTE_REVISE_ENTRY::LoadConfig(const char* path)
{
    char file_path[SD_MAX_PATH];
    SDSprintf(file_path, "%s%s", path, "monster_attribute_revise.xml");
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
