//auto created by structual_xml.py
#ifndef stone_config_h__
#define stone_config_h__

struct STONE_ROOT_TEMPLATE_STRUCT
{
    int32 id;
    int32 sub_type;
    int32 func_id;
    std::string name;
    std::string desc;

    void clear()
    {
        func_id = 0;
        desc = "";
        id = 0;
        name = "";
        sub_type = 0;
    }

    void parse(TiXmlElement* xmlRoot);
};

struct STONE_ROOT_STRUCT
{
    std::vector<STONE_ROOT_TEMPLATE_STRUCT> template_list;

    void clear()
    {
        template_list.clear();
    }

    void parse(TiXmlElement* xmlRoot);
};

class STONE_ENTRY
{
    DECLARE_SINGLETON(STONE_ENTRY)
public:
    bool LoadConfig(const char* path);
    const STONE_ROOT_STRUCT & GetStruct() const { return m_Struct;}
private:
    STONE_ROOT_STRUCT m_Struct;
};
#endif //stone_config_h__
