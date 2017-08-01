//auto created by structual_xml.py
#ifndef material_config_h__
#define material_config_h__

struct MATERIAL_ROOT_TEMPLATE_STRUCT
{
    std::string name;
    int32 grade;
    int32 max_stack;
    int32 id;
    int32 sub_type;
    std::string desc;

    void clear()
    {
        name = "";
        grade = 0;
        max_stack = 0;
        id = 0;
        sub_type = 0;
        desc = "";
    }

    void parse(TiXmlElement* xmlRoot);
};

struct MATERIAL_ROOT_STRUCT
{
    std::vector<MATERIAL_ROOT_TEMPLATE_STRUCT> template_list;

    void clear()
    {
        template_list.clear();
    }

    void parse(TiXmlElement* xmlRoot);
};

class MATERIAL_ENTRY
{
    DECLARE_SINGLETON(MATERIAL_ENTRY)
public:
    bool LoadConfig(const char* path);
    const MATERIAL_ROOT_STRUCT & GetStruct() const { return m_Struct;}
private:
    MATERIAL_ROOT_STRUCT m_Struct;
};
#endif //material_config_h__
