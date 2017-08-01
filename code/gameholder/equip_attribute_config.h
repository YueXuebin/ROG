//auto created by structual_xml.py
#ifndef equip_attribute_config_h__
#define equip_attribute_config_h__

struct EQUIP_ATTRIBUTE_ROOT_MODIFIER_STRUCT
{
    int32 grade;
    int32 prop_id;
    int32 prop_value;
    int32 type;
    int32 id;

    void clear()
    {
        grade = 0;
        prop_id = 0;
        prop_value = 0;
        type = 0;
        id = 0;
    }

    void parse(TiXmlElement* xmlRoot);
};

struct EQUIP_ATTRIBUTE_ROOT_STRUCT
{
    std::vector<EQUIP_ATTRIBUTE_ROOT_MODIFIER_STRUCT> modifier_list;

    void clear()
    {
        modifier_list.clear();
    }

    void parse(TiXmlElement* xmlRoot);
};

class EQUIP_ATTRIBUTE_ENTRY
{
    DECLARE_SINGLETON(EQUIP_ATTRIBUTE_ENTRY)
public:
    bool LoadConfig(const char* path);
    const EQUIP_ATTRIBUTE_ROOT_STRUCT & GetStruct() const { return m_Struct;}
private:
    EQUIP_ATTRIBUTE_ROOT_STRUCT m_Struct;
};
#endif //equip_attribute_config_h__
