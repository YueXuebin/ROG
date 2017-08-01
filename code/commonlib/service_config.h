//auto created by structual_xml.py
#ifndef service_config_h__
#define service_config_h__

struct SERVICE_ROOT_SERVICE_STRUCT
{
    int32 id;
    std::string name;

    void clear()
    {
        id = 0;
        name = "";
    }

    void parse(TiXmlElement* xmlRoot);
};

struct SERVICE_ROOT_STRUCT
{
    std::vector<SERVICE_ROOT_SERVICE_STRUCT> service_list;

    void clear()
    {
        service_list.clear();
    }

    void parse(TiXmlElement* xmlRoot);
};

class SERVICE_ENTRY
{
    DECLARE_SINGLETON(SERVICE_ENTRY)
public:
    bool LoadConfig(const char* path);
    const SERVICE_ROOT_STRUCT & GetStruct() const { return m_Struct;}
private:
    SERVICE_ROOT_STRUCT m_Struct;
};
#endif //service_config_h__
