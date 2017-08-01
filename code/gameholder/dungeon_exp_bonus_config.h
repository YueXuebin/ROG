//auto created by structual_xml.py
#ifndef dungeon_exp_bonus_config_h__
#define dungeon_exp_bonus_config_h__

struct DUNGEON_EXP_BONUS_ROOT_DUNGEON_STRUCT
{
    std::string exp_bonus_3;
    std::string exp_bonus_5;
    std::string exp_bonus_2;
    std::string exp_bonus_4;
    int32 exp_5;
    int32 exp_4;
    std::string exp_bonus_1;
    int32 exp_1;
    int32 exp_3;
    int32 exp_2;
    int32 id;

    void clear()
    {
        exp_5 = 0;
        exp_3 = 0;
        exp_4 = 0;
        exp_2 = 0;
        exp_bonus_3 = "";
        exp_bonus_2 = "";
        exp_bonus_1 = "";
        exp_1 = 0;
        exp_bonus_5 = "";
        exp_bonus_4 = "";
        id = 0;
    }

    void parse(TiXmlElement* xmlRoot);
};

struct DUNGEON_EXP_BONUS_ROOT_STRUCT
{
    std::vector<DUNGEON_EXP_BONUS_ROOT_DUNGEON_STRUCT> dungeon_list;

    void clear()
    {
        dungeon_list.clear();
    }

    void parse(TiXmlElement* xmlRoot);
};

class DUNGEON_EXP_BONUS_ENTRY
{
    DECLARE_SINGLETON(DUNGEON_EXP_BONUS_ENTRY)
public:
    bool LoadConfig(const char* path);
    const DUNGEON_EXP_BONUS_ROOT_STRUCT & GetStruct() const { return m_Struct;}
private:
    DUNGEON_EXP_BONUS_ROOT_STRUCT m_Struct;
};
#endif //dungeon_exp_bonus_config_h__
