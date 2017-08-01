//auto created by structual_xml.py
#ifndef prop_name_config_h__
#define prop_name_config_h__

struct PROP_NAME_ROOT_TEMPLATE_STRUCT
{
    std::string name;
    int32 value_type;
    int32 id;
    std::string value_unit;
    std::string desc;

    void clear()
    {
        desc = "";
        value_type = 0;
        name = "";
        value_unit = "";
        id = 0;
    }

    void parse(TiXmlElement* xmlRoot);
};

struct PROP_NAME_ROOT_STRUCT
{
    std::vector<PROP_NAME_ROOT_TEMPLATE_STRUCT> template_list;

    void clear()
    {
        template_list.clear();
    }

    void parse(TiXmlElement* xmlRoot);
};

class PROP_NAME_ENTRY
{
    DECLARE_SINGLETON(PROP_NAME_ENTRY)
public:
    bool LoadConfig(const char* path);
    const PROP_NAME_ROOT_STRUCT & GetStruct() const { return m_Struct;}
private:
    PROP_NAME_ROOT_STRUCT m_Struct;
};
#endif //prop_name_config_h__
