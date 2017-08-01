//auto created by structual_xml.py
#ifndef equip_wash_up_config_h__
#define equip_wash_up_config_h__

struct EQUIP_WASH_UP_ROOT_HEAD_STRUCT
{
    int32 price;
    int32 num;
    int32 id;

    void clear()
    {
        price = 0;
        num = 0;
        id = 0;
    }

    void parse(TiXmlElement* xmlRoot);
};

struct EQUIP_WASH_UP_ROOT_STRUCT
{
    std::vector<EQUIP_WASH_UP_ROOT_HEAD_STRUCT> head_list;

    void clear()
    {
        head_list.clear();
    }

    void parse(TiXmlElement* xmlRoot);
};

class EQUIP_WASH_UP_ENTRY
{
    DECLARE_SINGLETON(EQUIP_WASH_UP_ENTRY)
public:
    bool LoadConfig(const char* path);
    const EQUIP_WASH_UP_ROOT_STRUCT & GetStruct() const { return m_Struct;}
private:
    EQUIP_WASH_UP_ROOT_STRUCT m_Struct;
};
#endif //equip_wash_up_config_h__
