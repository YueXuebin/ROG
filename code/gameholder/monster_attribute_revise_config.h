//auto created by structual_xml.py
#ifndef monster_attribute_revise_config_h__
#define monster_attribute_revise_config_h__

struct MONSTER_ATTRIBUTE_REVISE_ROOT_MONSTER_STRUCT
{
    int32 mp_recovery_rate;
    int32 magic_block;
    int32 gold;
    std::string ai;
    std::string character;
    int32 movie_time;
    int32 radius;
    int32 cast_speed_rate;
    int32 crit;
    std::string movie_name;
    int32 id;
    int32 move_speed_rate;
    int32 attack_speed;
    int32 skill_1;
    int32 attack_block_rate;
    int32 mp_rate;
    int32 armor_rate;
    int32 frost_resistance_rate;
    int32 lightning_resistance;
    int32 armor;
    std::string eqp;
    int32 cast_speed;
    int32 dodge_rate;
    int32 fire_resistance_rate;
    int32 energy_shield;
    int32 attack;
    int32 attack_block;
    int32 move_speed;
    int32 hp_recovery;
    int32 package_id;
    int32 type;
    int32 hp;
    int32 crit_damage;
    int32 dodge;
    int32 hit;
    int32 mp_recovery;
    int32 frost_resistance;
    int32 fire_resistance;
    int32 magic_block_rate;
    int32 gold_rate;
    int32 energy_shield_rate;
    int32 crit_rate;
    int32 hp_recovery_rate;
    int32 exp_rate;
    int32 attack_rate;
    std::string name;
    int32 lightning_resistance_rate;
    int32 attack_speed_rate;
    int32 skill_2;
    int32 mp;
    int32 exp;
    int32 crit_damage_rate;
    int32 hp_rate;
    int32 hit_rate;

    void clear()
    {
        dodge = 0;
        magic_block = 0;
        frost_resistance_rate = 0;
        ai = "";
        dodge_rate = 0;
        movie_time = 0;
        radius = 0;
        cast_speed_rate = 0;
        crit = 0;
        movie_name = "";
        id = 0;
        move_speed_rate = 0;
        attack_speed = 0;
        attack_speed_rate = 0;
        attack_block_rate = 0;
        mp_rate = 0;
        armor_rate = 0;
        gold = 0;
        lightning_resistance = 0;
        armor = 0;
        eqp = "";
        character = "";
        fire_resistance_rate = 0;
        energy_shield = 0;
        attack = 0;
        attack_block = 0;
        move_speed = 0;
        hp_recovery = 0;
        type = 0;
        frost_resistance = 0;
        crit_damage = 0;
        mp_recovery_rate = 0;
        hit = 0;
        energy_shield_rate = 0;
        mp_recovery = 0;
        hp = 0;
        fire_resistance = 0;
        magic_block_rate = 0;
        gold_rate = 0;
        cast_speed = 0;
        crit_rate = 0;
        lightning_resistance_rate = 0;
        hp_recovery_rate = 0;
        exp_rate = 0;
        attack_rate = 0;
        name = "";
        package_id = 0;
        skill_1 = 0;
        skill_2 = 0;
        mp = 0;
        exp = 0;
        crit_damage_rate = 0;
        hp_rate = 0;
        hit_rate = 0;
    }

    void parse(TiXmlElement* xmlRoot);
};

struct MONSTER_ATTRIBUTE_REVISE_ROOT_STRUCT
{
    std::vector<MONSTER_ATTRIBUTE_REVISE_ROOT_MONSTER_STRUCT> monster_list;

    void clear()
    {
        monster_list.clear();
    }

    void parse(TiXmlElement* xmlRoot);
};

class MONSTER_ATTRIBUTE_REVISE_ENTRY
{
    DECLARE_SINGLETON(MONSTER_ATTRIBUTE_REVISE_ENTRY)
public:
    bool LoadConfig(const char* path);
    const MONSTER_ATTRIBUTE_REVISE_ROOT_STRUCT & GetStruct() const { return m_Struct;}
private:
    MONSTER_ATTRIBUTE_REVISE_ROOT_STRUCT m_Struct;
};
#endif //monster_attribute_revise_config_h__
