//auto created by structual_xml.py
#ifndef random_package_config_h__
#define random_package_config_h__

struct RANDOM_PACKAGE_ROOT_PACKAGE_ITEM_STRUCT
{
    std::string server_cd;
    std::string reward_name;
    int32 get_times;
    int32 card_id;
    int32 min_val;
    int32 max_val;
    int32 rate;
    std::string personal_cd;
    int32 is_repeat;
    int32 is_send_notice;
    int32 reward_id;

    void clear()
    {
        server_cd = "";
        reward_name = "";
        get_times = 0;
        card_id = 0;
        min_val = 0;
        max_val = 0;
        rate = 0;
        personal_cd = "";
        is_repeat = 0;
        is_send_notice = 0;
        reward_id = 0;
    }

    void parse(TiXmlElement* xmlRoot);
};

struct RANDOM_PACKAGE_ROOT_PACKAGE_STRUCT
{
    std::vector<RANDOM_PACKAGE_ROOT_PACKAGE_ITEM_STRUCT> item_list;
    int32 random_num;
    int32 id;
    std::string package_name;

    void clear()
    {
        item_list.clear();
        random_num = 0;
        id = 0;
        package_name = "";
    }

    void parse(TiXmlElement* xmlRoot);
};

struct RANDOM_PACKAGE_ROOT_STRUCT
{
    std::vector<RANDOM_PACKAGE_ROOT_PACKAGE_STRUCT> package_list;

    void clear()
    {
        package_list.clear();
    }

    void parse(TiXmlElement* xmlRoot);
};

class RANDOM_PACKAGE_ENTRY
{
    DECLARE_SINGLETON(RANDOM_PACKAGE_ENTRY)
public:
    bool LoadConfig(const char* path);
    const RANDOM_PACKAGE_ROOT_STRUCT & GetStruct() const { return m_Struct;}
private:
    RANDOM_PACKAGE_ROOT_STRUCT m_Struct;
};
#endif //random_package_config_h__
