//auto created by structual_xml.py
#ifndef story_config_h__
#define story_config_h__

struct STORY_ROOT_STORY_DIALOG_STRUCT
{
    std::string left_character;
    std::string name;
    int32 tell_wait;
    std::string left_eqp;
    int32 item_icon;
    std::string right_character;
    std::string right_eqp;
    std::string tell;

    void clear()
    {
        left_character = "";
        name = "";
        tell_wait = 0;
        left_eqp = "";
        item_icon = 0;
        right_character = "";
        right_eqp = "";
        tell = "";
    }

    void parse(TiXmlElement* xmlRoot);
};

struct STORY_ROOT_STORY_STRUCT
{
    std::vector<STORY_ROOT_STORY_DIALOG_STRUCT> dialog_list;
    int32 trigger_type;
    int32 trigger_delay;
    int32 pos;
    int32 trigger_arg4;
    int32 trigger_arg3;
    int32 trigger_arg2;
    int32 trigger_arg1;

    void clear()
    {
        trigger_type = 0;
        trigger_delay = 0;
        pos = 0;
        trigger_arg4 = 0;
        trigger_arg3 = 0;
        trigger_arg2 = 0;
        trigger_arg1 = 0;
        dialog_list.clear();
    }

    void parse(TiXmlElement* xmlRoot);
};

struct STORY_ROOT_STRUCT
{
    std::vector<STORY_ROOT_STORY_STRUCT> story_list;

    void clear()
    {
        story_list.clear();
    }

    void parse(TiXmlElement* xmlRoot);
};

class STORY_ENTRY
{
    DECLARE_SINGLETON(STORY_ENTRY)
public:
    bool LoadConfig(const char* path);
    const STORY_ROOT_STRUCT & GetStruct() const { return m_Struct;}
private:
    STORY_ROOT_STRUCT m_Struct;
};
#endif //story_config_h__
