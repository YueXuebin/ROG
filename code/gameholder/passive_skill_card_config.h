//auto created by structual_xml.py
#ifndef passive_skill_card_config_h__
#define passive_skill_card_config_h__

struct PASSIVE_SKILL_CARD_ROOT_TPLT_STRUCT
{
    int32 prop1_id;
    std::string name;
    int32 prop1_value;
    int32 id;
    int32 prop2_value;
    int32 prop2_id;
    std::string desc;

    void clear()
    {
        prop1_id = 0;
        name = "";
        prop1_value = 0;
        prop2_id = 0;
        prop2_value = 0;
        id = 0;
        desc = "";
    }

    void parse(TiXmlElement* xmlRoot);
};

struct PASSIVE_SKILL_CARD_ROOT_STRUCT
{
    std::vector<PASSIVE_SKILL_CARD_ROOT_TPLT_STRUCT> tplt_list;

    void clear()
    {
        tplt_list.clear();
    }

    void parse(TiXmlElement* xmlRoot);
};

class PASSIVE_SKILL_CARD_ENTRY
{
    DECLARE_SINGLETON(PASSIVE_SKILL_CARD_ENTRY)
public:
    bool LoadConfig(const char* path);
    const PASSIVE_SKILL_CARD_ROOT_STRUCT & GetStruct() const { return m_Struct;}
private:
    PASSIVE_SKILL_CARD_ROOT_STRUCT m_Struct;
};
#endif //passive_skill_card_config_h__
