//auto created by structual_xml.py
#ifndef buff_config_h__
#define buff_config_h__

struct BUFF_ROOT_BUFF_STRUCT
{
    int32 aura_radius;
    int32 overlap;
    std::string name;
    int32 func_id;
    int32 effect_type;
    int32 is_aure;
    int32 id;
    int32 overlap_num;

    void clear()
    {
        aura_radius = 0;
        overlap = 0;
        name = "";
        func_id = 0;
        is_aure = 0;
        overlap_num = 0;
        id = 0;
        effect_type = 0;
    }

    void parse(TiXmlElement* xmlRoot);
};

struct BUFF_ROOT_STRUCT
{
    std::vector<BUFF_ROOT_BUFF_STRUCT> buff_list;

    void clear()
    {
        buff_list.clear();
    }

    void parse(TiXmlElement* xmlRoot);
};

class BUFF_ENTRY
{
    DECLARE_SINGLETON(BUFF_ENTRY)
public:
    bool LoadConfig(const char* path);
    const BUFF_ROOT_STRUCT & GetStruct() const { return m_Struct;}
private:
    BUFF_ROOT_STRUCT m_Struct;
};
#endif //buff_config_h__
