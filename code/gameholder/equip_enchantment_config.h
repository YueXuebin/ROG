//auto created by structual_xml.py
#ifndef equip_enchantment_config_h__
#define equip_enchantment_config_h__

struct EQUIP_ENCHANTMENT_ROOT_FORMULA_STRUCT
{
    int32 item3_id;
    int32 item1_id;
    int32 item2_id;
    int32 item2_num;
    int32 attribute_id;
    int32 item4_id;
    int32 item1_num;
    int32 item3_num;
    int32 item4_num;

    void clear()
    {
        item3_id = 0;
        item1_id = 0;
        item2_id = 0;
        item2_num = 0;
        attribute_id = 0;
        item4_id = 0;
        item1_num = 0;
        item3_num = 0;
        item4_num = 0;
    }

    void parse(TiXmlElement* xmlRoot);
};

struct EQUIP_ENCHANTMENT_ROOT_STRUCT
{
    std::vector<EQUIP_ENCHANTMENT_ROOT_FORMULA_STRUCT> formula_list;

    void clear()
    {
        formula_list.clear();
    }

    void parse(TiXmlElement* xmlRoot);
};

class EQUIP_ENCHANTMENT_ENTRY
{
    DECLARE_SINGLETON(EQUIP_ENCHANTMENT_ENTRY)
public:
    bool LoadConfig(const char* path);
    const EQUIP_ENCHANTMENT_ROOT_STRUCT & GetStruct() const { return m_Struct;}
private:
    EQUIP_ENCHANTMENT_ROOT_STRUCT m_Struct;
};
#endif //equip_enchantment_config_h__
