//auto created by structual_xml.py
#ifndef game_param_config_h__
#define game_param_config_h__

struct GAME_PARAM_ROOT_SYSTEM_STRUCT
{
    std::string name;
    int32 value;

    void clear()
    {
        name = "";
        value = 0;
    }

    void parse(TiXmlElement* xmlRoot);
};

struct GAME_PARAM_ROOT_STRUCT
{
    std::vector<GAME_PARAM_ROOT_SYSTEM_STRUCT> system_list;

    void clear()
    {
        system_list.clear();
    }

    void parse(TiXmlElement* xmlRoot);
};

class GAME_PARAM_ENTRY
{
    DECLARE_SINGLETON(GAME_PARAM_ENTRY)
public:
    bool LoadConfig(const char* path);
    const GAME_PARAM_ROOT_STRUCT & GetStruct() const { return m_Struct;}
private:
    GAME_PARAM_ROOT_STRUCT m_Struct;
};
#endif //game_param_config_h__
