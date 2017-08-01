//auto created by structual_xml.py
#ifndef consume_config_h__
#define consume_config_h__

struct CONSUME_ROOT_TEMPLATE_STRUCT
{
    int32 sub_type;
    std::string name;
    int32 cd_type;
    int32 cd_time;
    std::string effect;
    int32 max_stack;
    int32 id;
    int32 continued_time;
    std::string desc;

    void clear()
    {
        name = "";
        cd_type = 0;
        cd_time = 0;
        effect = "";
        max_stack = 0;
        id = 0;
        sub_type = 0;
        continued_time = 0;
        desc = "";
    }

    void parse(TiXmlElement* xmlRoot);
};

struct CONSUME_ROOT_STRUCT
{
    std::vector<CONSUME_ROOT_TEMPLATE_STRUCT> template_list;

    void clear()
    {
        template_list.clear();
    }

    void parse(TiXmlElement* xmlRoot);
};

class CONSUME_ENTRY
{
    DECLARE_SINGLETON(CONSUME_ENTRY)
public:
    bool LoadConfig(const char* path);
    const CONSUME_ROOT_STRUCT & GetStruct() const { return m_Struct;}
private:
    CONSUME_ROOT_STRUCT m_Struct;
};
#endif //consume_config_h__
