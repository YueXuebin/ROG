//auto created by structual_xml.py
#ifndef sign_gain_config_h__
#define sign_gain_config_h__

struct SIGN_GAIN_ROOT_MONTH_DAY_STRUCT
{
    int32 itemId;
    int32 itemNum;
    int32 vip;
    int32 id;
    int32 name;

    void clear()
    {
        itemId = 0;
        itemNum = 0;
        vip = 0;
        id = 0;
        name = 0;
    }

    void parse(TiXmlElement* xmlRoot);
};

struct SIGN_GAIN_ROOT_MONTH_STRUCT
{
    std::vector<SIGN_GAIN_ROOT_MONTH_DAY_STRUCT> day_list;
    int32 id;
    std::string name;

    void clear()
    {
        day_list.clear();
        name = "";
        id = 0;
    }

    void parse(TiXmlElement* xmlRoot);
};

struct SIGN_GAIN_ROOT_STRUCT
{
    std::vector<SIGN_GAIN_ROOT_MONTH_STRUCT> month_list;

    void clear()
    {
        month_list.clear();
    }

    void parse(TiXmlElement* xmlRoot);
};

class SIGN_GAIN_ENTRY
{
    DECLARE_SINGLETON(SIGN_GAIN_ENTRY)
public:
    bool LoadConfig(const char* path);
    const SIGN_GAIN_ROOT_STRUCT & GetStruct() const { return m_Struct;}
private:
    SIGN_GAIN_ROOT_STRUCT m_Struct;
};
#endif //sign_gain_config_h__
