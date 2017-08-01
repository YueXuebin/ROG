//auto created by structual_xml.py
#ifndef dungeon_clearance_bonus_config_h__
#define dungeon_clearance_bonus_config_h__

struct DUNGEON_CLEARANCE_BONUS_ROOT_DUNGEON_STRUCT
{
    std::string two_star_bonus;
    int32 clearance_exp;
    int32 dungeon_id;
    std::string clearance_bonus;
    std::string three_star_bonus;

    void clear()
    {
        three_star_bonus = "";
        clearance_exp = 0;
        dungeon_id = 0;
        clearance_bonus = "";
        two_star_bonus = "";
    }

    void parse(TiXmlElement* xmlRoot);
};

struct DUNGEON_CLEARANCE_BONUS_ROOT_STRUCT
{
    std::vector<DUNGEON_CLEARANCE_BONUS_ROOT_DUNGEON_STRUCT> dungeon_list;

    void clear()
    {
        dungeon_list.clear();
    }

    void parse(TiXmlElement* xmlRoot);
};

class DUNGEON_CLEARANCE_BONUS_ENTRY
{
    DECLARE_SINGLETON(DUNGEON_CLEARANCE_BONUS_ENTRY)
public:
    bool LoadConfig(const char* path);
    const DUNGEON_CLEARANCE_BONUS_ROOT_STRUCT & GetStruct() const { return m_Struct;}
private:
    DUNGEON_CLEARANCE_BONUS_ROOT_STRUCT m_Struct;
};
#endif //dungeon_clearance_bonus_config_h__
