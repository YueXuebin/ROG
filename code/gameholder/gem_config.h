//auto created by structual_xml.py
#ifndef gem_config_h__
#define gem_config_h__

struct GEM_ROOT_GEM_ITEM_STRUCT
{
    int32 remove_price;
    int32 fusion_price;
    int32 level;
    int32 socketing_price;
    int32 fusion_exp;
    int32 exp;
    int32 prop_id;
    int32 prop_value;

    void clear()
    {
        remove_price = 0;
        fusion_price = 0;
        level = 0;
        socketing_price = 0;
        fusion_exp = 0;
        exp = 0;
        prop_id = 0;
        prop_value = 0;
    }

    void parse(TiXmlElement* xmlRoot);
};

struct GEM_ROOT_GEM_STRUCT
{
    std::vector<GEM_ROOT_GEM_ITEM_STRUCT> item_list;
    int32 hole_type;
    int32 id;

    void clear()
    {
        item_list.clear();
        hole_type = 0;
        id = 0;
    }

    void parse(TiXmlElement* xmlRoot);
};

struct GEM_ROOT_STRUCT
{
    std::vector<GEM_ROOT_GEM_STRUCT> gem_list;

    void clear()
    {
        gem_list.clear();
    }

    void parse(TiXmlElement* xmlRoot);
};

class GEM_ENTRY
{
    DECLARE_SINGLETON(GEM_ENTRY)
public:
    bool LoadConfig(const char* path);
    const GEM_ROOT_STRUCT & GetStruct() const { return m_Struct;}
private:
    GEM_ROOT_STRUCT m_Struct;
};
#endif //gem_config_h__
