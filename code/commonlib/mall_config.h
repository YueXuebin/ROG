//auto created by structual_xml.py
#ifndef mall_config_h__
#define mall_config_h__

struct MALL_ROOT_VERSION_STRUCT
{
    int32 number;

    void clear()
    {
        number = 0;
    }

    void parse(TiXmlElement* xmlRoot);
};

struct MALL_ROOT_LIST_PRODUCT_STRUCT
{
    int32 show_vip_price;
    int32 new_flag;
    int32 show_flag;
    int32 item_id;
    int32 home_page;
    int32 product_num;
    int32 vip_price;
    int32 again_discount;
    int32 again_price;
    int32 sell_type;
    int32 price;
    int32 show_price;
    std::string start_time;
    int32 again_start_time;
    int32 show_again_price;
    int32 discount;
    int32 global_limit_num;
    int32 item_num;
    int32 money_type;
    int32 again_end_time;
    int32 product_id;
    int32 hot_flag;
    std::string end_time;
    int32 tag_type;
    int32 present_flag;
    int32 personal_limit_num;

    void clear()
    {
        show_vip_price = 0;
        new_flag = 0;
        show_flag = 0;
        item_id = 0;
        product_num = 0;
        home_page = 0;
        show_price = 0;
        vip_price = 0;
        again_discount = 0;
        again_price = 0;
        sell_type = 0;
        start_time = "";
        price = 0;
        again_start_time = 0;
        again_end_time = 0;
        discount = 0;
        global_limit_num = 0;
        item_num = 0;
        money_type = 0;
        show_again_price = 0;
        product_id = 0;
        hot_flag = 0;
        end_time = "";
        personal_limit_num = 0;
        present_flag = 0;
        tag_type = 0;
    }

    void parse(TiXmlElement* xmlRoot);
};

struct MALL_ROOT_LIST_STRUCT
{
    std::vector<MALL_ROOT_LIST_PRODUCT_STRUCT> product_list;

    void clear()
    {
        product_list.clear();
    }

    void parse(TiXmlElement* xmlRoot);
};

struct MALL_ROOT_STRUCT
{
    MALL_ROOT_VERSION_STRUCT version;
    MALL_ROOT_LIST_STRUCT list;

    void clear()
    {
        version.clear();
        list.clear();
    }

    void parse(TiXmlElement* xmlRoot);
};

class MALL_ENTRY
{
    DECLARE_SINGLETON(MALL_ENTRY)
public:
    bool LoadConfig(const char* path);
    const MALL_ROOT_STRUCT & GetStruct() const { return m_Struct;}
private:
    MALL_ROOT_STRUCT m_Struct;
};
#endif //mall_config_h__
