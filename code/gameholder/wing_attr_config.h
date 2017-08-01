//auto created by structual_xml.py
#ifndef wing_attr_config_h__
#define wing_attr_config_h__

struct WING_ATTR_ROOT_WING_ITEM_STRUCT
{
    int32 attr_2;
    int32 attr_3;
    int32 attr_1;
    int32 star;
    int32 attr_4;
    int32 level;
    int32 attr_type_2;
    int32 attr_type_3;
    int32 step;
    int32 attr_type_1;
    int32 attr_type_4;
    int32 num_1;
    int32 item_id_1;

    void clear()
    {
        attr_2 = 0;
        attr_3 = 0;
        attr_1 = 0;
        star = 0;
        attr_4 = 0;
        level = 0;
        attr_type_2 = 0;
        attr_type_3 = 0;
        step = 0;
        attr_type_1 = 0;
        attr_type_4 = 0;
        num_1 = 0;
        item_id_1 = 0;
    }

    void parse(TiXmlElement* xmlRoot);
};

struct WING_ATTR_ROOT_WING_STRUCT
{
    std::vector<WING_ATTR_ROOT_WING_ITEM_STRUCT> item_list;
    std::string name;
    std::string eqp;
    int32 item_id;
    int32 icon;
    int32 id;
    std::string desc;

    void clear()
    {
        item_list.clear();
        name = "";
        eqp = "";
        item_id = 0;
        desc = "";
        id = 0;
        icon = 0;
    }

    void parse(TiXmlElement* xmlRoot);
};

struct WING_ATTR_ROOT_STRUCT
{
    std::vector<WING_ATTR_ROOT_WING_STRUCT> wing_list;

    void clear()
    {
        wing_list.clear();
    }

    void parse(TiXmlElement* xmlRoot);
};

class WING_ATTR_ENTRY
{
    DECLARE_SINGLETON(WING_ATTR_ENTRY)
public:
    bool LoadConfig(const char* path);
    const WING_ATTR_ROOT_STRUCT & GetStruct() const { return m_Struct;}
private:
    WING_ATTR_ROOT_STRUCT m_Struct;
};
#endif //wing_attr_config_h__
