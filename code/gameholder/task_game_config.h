//auto created by structual_xml.py
#ifndef task_game_config_h__
#define task_game_config_h__

struct TASK_GAME_ROOT_TASK_STRUCT
{
    std::string task_reward;
    std::string pre_condition;
    int32 task_type;
    int32 task_id;
    std::string mage_reward;
    std::string task_target;
    std::string archer_reward;
    std::string warrior_reward;
    std::string task_name;
    std::string task_desc;
    int32 sub_type;

    void clear()
    {
        task_reward = "";
        archer_reward = "";
        task_type = 0;
        task_id = 0;
        mage_reward = "";
        task_target = "";
        pre_condition = "";
        warrior_reward = "";
        task_name = "";
        task_desc = "";
        sub_type = 0;
    }

    void parse(TiXmlElement* xmlRoot);
};

struct TASK_GAME_ROOT_STRUCT
{
    std::vector<TASK_GAME_ROOT_TASK_STRUCT> task_list;

    void clear()
    {
        task_list.clear();
    }

    void parse(TiXmlElement* xmlRoot);
};

class TASK_GAME_ENTRY
{
    DECLARE_SINGLETON(TASK_GAME_ENTRY)
public:
    bool LoadConfig(const char* path);
    const TASK_GAME_ROOT_STRUCT & GetStruct() const { return m_Struct;}
private:
    TASK_GAME_ROOT_STRUCT m_Struct;
};
#endif //task_game_config_h__
