//auto created by structual_xml.py
#ifndef equip_config_h__
#define equip_config_h__

struct EQUIP_ROOT_TEMPLATE_STRUCT
{
    int32 eqp_type;
    std::string name;
    std::string female_eqp;
    int32 eqp_level;
    int32 sub_type;
    std::string male_eqp;
    int32 grade;
    int32 prop_key_2;
    int32 part_type;
    int32 prop_key_1;
    int32 prop_value_1;
    int32 prop_value_2;
    int32 id;
    int32 max_hole;

    void clear()
    {
        eqp_type = 0;
        name = "";
        female_eqp = "";
        eqp_level = 0;
        id = 0;
        male_eqp = "";
        grade = 0;
        prop_key_2 = 0;
        part_type = 0;
        prop_key_1 = 0;
        prop_value_1 = 0;
        prop_value_2 = 0;
        sub_type = 0;
        max_hole = 0;
    }

    void parse(TiXmlElement* xmlRoot);
};

struct EQUIP_ROOT_STRUCT
{
    std::vector<EQUIP_ROOT_TEMPLATE_STRUCT> template_list;

    void clear()
    {
        template_list.clear();
    }

    void parse(TiXmlElement* xmlRoot);
};

class EQUIP_ENTRY
{
    DECLARE_SINGLETON(EQUIP_ENTRY)
public:
    bool LoadConfig(const char* path);
    const EQUIP_ROOT_STRUCT & GetStruct() const { return m_Struct;}
private:
    EQUIP_ROOT_STRUCT m_Struct;
};
#endif //equip_config_h__
