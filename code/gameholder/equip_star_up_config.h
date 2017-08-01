//auto created by structual_xml.py
#ifndef equip_star_up_config_h__
#define equip_star_up_config_h__

struct EQUIP_STAR_UP_ROOT_HEAD_STRUCT
{
    int32 star;
    int32 price;
    int32 percent;
    int32 success_rate;
    int32 num;
    int32 id;

    void clear()
    {
        star = 0;
        price = 0;
        percent = 0;
        success_rate = 0;
        num = 0;
        id = 0;
    }

    void parse(TiXmlElement* xmlRoot);
};

struct EQUIP_STAR_UP_ROOT_STRUCT
{
    std::vector<EQUIP_STAR_UP_ROOT_HEAD_STRUCT> head_list;

    void clear()
    {
        head_list.clear();
    }

    void parse(TiXmlElement* xmlRoot);
};

class EQUIP_STAR_UP_ENTRY
{
    DECLARE_SINGLETON(EQUIP_STAR_UP_ENTRY)
public:
    bool LoadConfig(const char* path);
    const EQUIP_STAR_UP_ROOT_STRUCT & GetStruct() const { return m_Struct;}
private:
    EQUIP_STAR_UP_ROOT_STRUCT m_Struct;
};
#endif //equip_star_up_config_h__
