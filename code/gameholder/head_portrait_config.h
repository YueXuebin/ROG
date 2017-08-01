//auto created by structual_xml.py
#ifndef head_portrait_config_h__
#define head_portrait_config_h__

struct HEAD_PORTRAIT_ROOT_HEAD_STRUCT
{
    std::string open;
    int32 id;
    std::string desc;

    void clear()
    {
        open = "";
        id = 0;
        desc = "";
    }

    void parse(TiXmlElement* xmlRoot);
};

struct HEAD_PORTRAIT_ROOT_STRUCT
{
    std::vector<HEAD_PORTRAIT_ROOT_HEAD_STRUCT> head_list;

    void clear()
    {
        head_list.clear();
    }

    void parse(TiXmlElement* xmlRoot);
};

class HEAD_PORTRAIT_ENTRY
{
    DECLARE_SINGLETON(HEAD_PORTRAIT_ENTRY)
public:
    bool LoadConfig(const char* path);
    const HEAD_PORTRAIT_ROOT_STRUCT & GetStruct() const { return m_Struct;}
private:
    HEAD_PORTRAIT_ROOT_STRUCT m_Struct;
};
#endif //head_portrait_config_h__
