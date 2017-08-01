//auto created by structual_xml.py
#ifndef levelup_config_h__
#define levelup_config_h__

struct LEVELUP_ROOT_LEVEL_STRUCT
{
    int32 dodge;
    int32 attack_speed;
    int32 hit;
    int32 energy_shield;
    int32 lightning_resistance;
    int32 level;
    int32 mp_recovery;
    int32 move_speed;
    int32 frost_resistance;
    int32 fire_resistance;
    int32 physics_damage;
    int32 armor;
    int32 hp_recovery;
    int32 cast_speed;
    int32 mp;
    int32 attack_block;
    int32 max_exp;
    int32 crit;
    int32 magic_block;
    int32 hp;
    int32 crit_damage;

    void clear()
    {
        dodge = 0;
        attack_speed = 0;
        armor = 0;
        hit = 0;
        lightning_resistance = 0;
        level = 0;
        mp_recovery = 0;
        move_speed = 0;
        frost_resistance = 0;
        fire_resistance = 0;
        cast_speed = 0;
        energy_shield = 0;
        magic_block = 0;
        physics_damage = 0;
        mp = 0;
        attack_block = 0;
        max_exp = 0;
        crit = 0;
        hp_recovery = 0;
        hp = 0;
        crit_damage = 0;
    }

    void parse(TiXmlElement* xmlRoot);
};

struct LEVELUP_ROOT_STRUCT
{
    std::vector<LEVELUP_ROOT_LEVEL_STRUCT> level_list;

    void clear()
    {
        level_list.clear();
    }

    void parse(TiXmlElement* xmlRoot);
};

class LEVELUP_ENTRY
{
    DECLARE_SINGLETON(LEVELUP_ENTRY)
public:
    bool LoadConfig(const char* path);
    const LEVELUP_ROOT_STRUCT & GetStruct() const { return m_Struct;}
private:
    LEVELUP_ROOT_STRUCT m_Struct;
};
#endif //levelup_config_h__
