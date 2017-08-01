//auto created by structual_xml.py
#ifndef rune_config_h__
#define rune_config_h__

struct RUNE_ROOT_RUNE_STRUCT
{
    int32 prop1_id;
    std::string name;
    int32 prop1_base;
    int32 feature;
    int32 prop2_base;
    int32 prop2_id;
    int32 mana;
    int32 trigger;
    std::string letter;
    int32 prop1_grow;
    int32 id;
    int32 prop2_grow;

    void clear()
    {
        prop1_id = 0;
        name = "";
        prop1_base = 0;
        feature = 0;
        prop2_base = 0;
        mana = 0;
        trigger = 0;
        letter = "";
        id = 0;
        prop1_grow = 0;
        prop2_id = 0;
        prop2_grow = 0;
    }

    void parse(TiXmlElement* xmlRoot);
};

struct RUNE_ROOT_STRUCT
{
    std::vector<RUNE_ROOT_RUNE_STRUCT> rune_list;

    void clear()
    {
        rune_list.clear();
    }

    void parse(TiXmlElement* xmlRoot);
};

class RUNE_ENTRY
{
    DECLARE_SINGLETON(RUNE_ENTRY)
public:
    bool LoadConfig(const char* path);
    const RUNE_ROOT_STRUCT & GetStruct() const { return m_Struct;}
private:
    RUNE_ROOT_STRUCT m_Struct;
};
#endif //rune_config_h__
