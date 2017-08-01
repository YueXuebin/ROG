//auto created by structual_xml.py
#ifndef passive_skill_config_h__
#define passive_skill_config_h__

struct PASSIVE_SKILL_ROOT_PASSIVE_SKILL_ITEM_STRUCT
{
    int32 prop1_id;
    int32 prop2_value;
    int32 prop1_value;
    int32 prop2_id;
    int32 level;

    void clear()
    {
        prop1_id = 0;
        prop2_value = 0;
        level = 0;
        prop1_value = 0;
        prop2_id = 0;
    }

    void parse(TiXmlElement* xmlRoot);
};

struct PASSIVE_SKILL_ROOT_PASSIVE_SKILL_STRUCT
{
    std::vector<PASSIVE_SKILL_ROOT_PASSIVE_SKILL_ITEM_STRUCT> item_list;
    std::string name;
    int32 prop2_id;
    int32 prop2_value;
    int32 unlock_points;
    int32 type;
    int32 id;
    std::string desc;

    void clear()
    {
        item_list.clear();
        name = "";
        id = 0;
        prop2_value = 0;
        unlock_points = 0;
        type = 0;
        prop2_id = 0;
        desc = "";
    }

    void parse(TiXmlElement* xmlRoot);
};

struct PASSIVE_SKILL_ROOT_STRUCT
{
    std::vector<PASSIVE_SKILL_ROOT_PASSIVE_SKILL_STRUCT> passive_skill_list;

    void clear()
    {
        passive_skill_list.clear();
    }

    void parse(TiXmlElement* xmlRoot);
};

class PASSIVE_SKILL_ENTRY
{
    DECLARE_SINGLETON(PASSIVE_SKILL_ENTRY)
public:
    bool LoadConfig(const char* path);
    const PASSIVE_SKILL_ROOT_STRUCT & GetStruct() const { return m_Struct;}
private:
    PASSIVE_SKILL_ROOT_STRUCT m_Struct;
};
#endif //passive_skill_config_h__
