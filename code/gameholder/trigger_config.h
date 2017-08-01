//auto created by structual_xml.py
#ifndef trigger_config_h__
#define trigger_config_h__

struct TRIGGER_ROOT_TRIGGER_STRUCT
{
    std::string pick_pos;
    std::string pick_size;
    std::string object_set;
    std::string name;
    std::string effect_pos;
    int32 show_id;
    std::string effect;
    int32 type;
    std::string object_set_pos;

    void clear()
    {
        pick_pos = "";
        pick_size = "";
        object_set = "";
        name = "";
        effect_pos = "";
        show_id = 0;
        effect = "";
        type = 0;
        object_set_pos = "";
    }

    void parse(TiXmlElement* xmlRoot);
};

struct TRIGGER_ROOT_STRUCT
{
    std::vector<TRIGGER_ROOT_TRIGGER_STRUCT> trigger_list;

    void clear()
    {
        trigger_list.clear();
    }

    void parse(TiXmlElement* xmlRoot);
};

class TRIGGER_ENTRY
{
    DECLARE_SINGLETON(TRIGGER_ENTRY)
public:
    bool LoadConfig(const char* path);
    const TRIGGER_ROOT_STRUCT & GetStruct() const { return m_Struct;}
private:
    TRIGGER_ROOT_STRUCT m_Struct;
};
#endif //trigger_config_h__
