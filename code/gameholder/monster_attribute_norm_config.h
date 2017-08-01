//auto created by structual_xml.py
#ifndef monster_attribute_norm_config_h__
#define monster_attribute_norm_config_h__

struct MONSTER_ATTRIBUTE_NORM_ROOT_TEMPLATE_STRUCT
{
    int32 magic_block;
    int32 gold;
    int32 move_speed;
    int32 crit;
    int32 attack_speed;
    int32 lightning_resistance;
    int32 armor;
    int32 energy_shield;
    int32 attack;
    int32 attack_block;
    int32 hp_recovery;
    int32 frost_resistance;
    int32 crit_damage;
    int32 dodge;
    int32 hit;
    int32 mp_recovery;
    int32 hp;
    int32 fire_resistance;
    int32 cast_speed;
    int32 level;
    int32 mp;
    int32 exp;

    void clear()
    {
        magic_block = 0;
        gold = 0;
        move_speed = 0;
        crit = 0;
        attack_speed = 0;
        lightning_resistance = 0;
        armor = 0;
        energy_shield = 0;
        attack = 0;
        attack_block = 0;
        hp_recovery = 0;
        hp = 0;
        crit_damage = 0;
        dodge = 0;
        hit = 0;
        mp_recovery = 0;
        frost_resistance = 0;
        fire_resistance = 0;
        cast_speed = 0;
        level = 0;
        mp = 0;
        exp = 0;
    }

    void parse(TiXmlElement* xmlRoot);
};

struct MONSTER_ATTRIBUTE_NORM_ROOT_STRUCT
{
    std::vector<MONSTER_ATTRIBUTE_NORM_ROOT_TEMPLATE_STRUCT> template_list;

    void clear()
    {
        template_list.clear();
    }

    void parse(TiXmlElement* xmlRoot);
};

class MONSTER_ATTRIBUTE_NORM_ENTRY
{
    DECLARE_SINGLETON(MONSTER_ATTRIBUTE_NORM_ENTRY)
public:
    bool LoadConfig(const char* path);
    const MONSTER_ATTRIBUTE_NORM_ROOT_STRUCT & GetStruct() const { return m_Struct;}
private:
    MONSTER_ATTRIBUTE_NORM_ROOT_STRUCT m_Struct;
};
#endif //monster_attribute_norm_config_h__
