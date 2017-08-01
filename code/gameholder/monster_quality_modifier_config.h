//auto created by structual_xml.py
#ifndef monster_quality_modifier_config_h__
#define monster_quality_modifier_config_h__

struct MONSTER_QUALITY_MODIFIER_ROOT_MODIFIER_STRUCT
{
    int32 prop1_id;
    std::string name;
    int32 prop1_value;
    int32 id;
    int32 prop2_value;
    int32 prop2_id;

    void clear()
    {
        prop1_id = 0;
        name = "";
        prop1_value = 0;
        prop2_id = 0;
        prop2_value = 0;
        id = 0;
    }

    void parse(TiXmlElement* xmlRoot);
};

struct MONSTER_QUALITY_MODIFIER_ROOT_STRUCT
{
    std::vector<MONSTER_QUALITY_MODIFIER_ROOT_MODIFIER_STRUCT> modifier_list;

    void clear()
    {
        modifier_list.clear();
    }

    void parse(TiXmlElement* xmlRoot);
};

class MONSTER_QUALITY_MODIFIER_ENTRY
{
    DECLARE_SINGLETON(MONSTER_QUALITY_MODIFIER_ENTRY)
public:
    bool LoadConfig(const char* path);
    const MONSTER_QUALITY_MODIFIER_ROOT_STRUCT & GetStruct() const { return m_Struct;}
private:
    MONSTER_QUALITY_MODIFIER_ROOT_STRUCT m_Struct;
};
#endif //monster_quality_modifier_config_h__
