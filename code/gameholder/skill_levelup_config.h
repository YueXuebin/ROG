//auto created by structual_xml.py
#ifndef skill_levelup_config_h__
#define skill_levelup_config_h__

struct SKILL_LEVELUP_ROOT_SKILL_VALUE_STRUCT
{
    int32 blue;
    int32 gold;
    int32 level;
    int32 green_exp;
    int32 white_exp;
    int32 gold_exp;
    int32 purple;
    int32 green;
    int32 purple_exp;
    int32 white;
    int32 blue_exp;

    void clear()
    {
        blue = 0;
        gold = 0;
        level = 0;
        green_exp = 0;
        white_exp = 0;
        gold_exp = 0;
        purple = 0;
        green = 0;
        purple_exp = 0;
        white = 0;
        blue_exp = 0;
    }

    void parse(TiXmlElement* xmlRoot);
};

struct SKILL_LEVELUP_ROOT_STRUCT
{
    std::vector<SKILL_LEVELUP_ROOT_SKILL_VALUE_STRUCT> skill_value_list;

    void clear()
    {
        skill_value_list.clear();
    }

    void parse(TiXmlElement* xmlRoot);
};

class SKILL_LEVELUP_ENTRY
{
    DECLARE_SINGLETON(SKILL_LEVELUP_ENTRY)
public:
    bool LoadConfig(const char* path);
    const SKILL_LEVELUP_ROOT_STRUCT & GetStruct() const { return m_Struct;}
private:
    SKILL_LEVELUP_ROOT_STRUCT m_Struct;
};
#endif //skill_levelup_config_h__
