//auto created by structual_xml.py
#ifndef skill_config_h__
#define skill_config_h__

struct SKILL_ROOT_SKILL_STRUCT
{
    int32 prop2_grow;
    int32 cast_multi;
    int32 cooldown;
    int32 bullet_chain_id;
    int32 bullet_repeat;
    int32 id;
    int32 attack_type;
    int32 prop1_base;
    int32 feature;
    int32 prop2_id;
    int32 bullet_multi_angle;
    int32 bullet_multi;
    int32 to_friend;
    int32 prop1_grow;
    int32 buff;
    int32 attack_speed;
    int32 bullet_chain;
    int32 prop2_base;
    int32 buff_time;
    int32 to_enemy;
    int32 buff_val1_base;
    int32 to_me;
    std::string desc;
    int32 prop1_id;
    int32 bullet_repeat_interval;
    std::string name;
    int32 buff_val1_grow;
    int32 buff_val2_base;
    int32 mp;
    int32 buff_val2_grow;

    void clear()
    {
        prop2_grow = 0;
        feature = 0;
        cooldown = 0;
        bullet_chain_id = 0;
        bullet_repeat = 0;
        prop2_id = 0;
        attack_type = 0;
        prop1_base = 0;
        cast_multi = 0;
        id = 0;
        bullet_multi_angle = 0;
        bullet_multi = 0;
        to_friend = 0;
        prop1_grow = 0;
        buff = 0;
        attack_speed = 0;
        bullet_chain = 0;
        prop2_base = 0;
        buff_time = 0;
        to_enemy = 0;
        buff_val1_base = 0;
        to_me = 0;
        desc = "";
        prop1_id = 0;
        bullet_repeat_interval = 0;
        name = "";
        buff_val1_grow = 0;
        buff_val2_base = 0;
        mp = 0;
        buff_val2_grow = 0;
    }

    void parse(TiXmlElement* xmlRoot);
};

struct SKILL_ROOT_STRUCT
{
    std::vector<SKILL_ROOT_SKILL_STRUCT> skill_list;

    void clear()
    {
        skill_list.clear();
    }

    void parse(TiXmlElement* xmlRoot);
};

class SKILL_ENTRY
{
    DECLARE_SINGLETON(SKILL_ENTRY)
public:
    bool LoadConfig(const char* path);
    const SKILL_ROOT_STRUCT & GetStruct() const { return m_Struct;}
private:
    SKILL_ROOT_STRUCT m_Struct;
};
#endif //skill_config_h__
