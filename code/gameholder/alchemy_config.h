//auto created by structual_xml.py
#ifndef alchemy_config_h__
#define alchemy_config_h__

struct ALCHEMY_ROOT_ALCHEMY_STRUCT
{
    int32 dst_quality;
    int32 dst_item_id;
    int32 item1_id;
    int32 item2_id;
    int32 item2_num;
    int32 item4_id;
    int32 item1_num;
    int32 item3_id;
    int32 item3_num;
    int32 id;
    int32 item4_num;

    void clear()
    {
        dst_quality = 0;
        dst_item_id = 0;
        item1_id = 0;
        item2_id = 0;
        item2_num = 0;
        item4_id = 0;
        item1_num = 0;
        item3_id = 0;
        item3_num = 0;
        id = 0;
        item4_num = 0;
    }

    void parse(TiXmlElement* xmlRoot);
};

struct ALCHEMY_ROOT_STRUCT
{
    std::vector<ALCHEMY_ROOT_ALCHEMY_STRUCT> alchemy_list;

    void clear()
    {
        alchemy_list.clear();
    }

    void parse(TiXmlElement* xmlRoot);
};

class ALCHEMY_ENTRY
{
    DECLARE_SINGLETON(ALCHEMY_ENTRY)
public:
    bool LoadConfig(const char* path);
    const ALCHEMY_ROOT_STRUCT & GetStruct() const { return m_Struct;}
private:
    ALCHEMY_ROOT_STRUCT m_Struct;
};
#endif //alchemy_config_h__
