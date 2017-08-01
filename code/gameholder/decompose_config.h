//auto created by structual_xml.py
#ifndef decompose_config_h__
#define decompose_config_h__

struct DECOMPOSE_ROOT_FORMULA_STRUCT
{
    int32 item3_id;
    int32 item_type;
    int32 item1_id;
    int32 item2_id;
    int32 item2_num;
    int32 item4_id;
    int32 item1_num;
    int32 item_quality;
    int32 item3_num;
    int32 id;
    int32 item4_num;

    void clear()
    {
        item3_id = 0;
        item2_num = 0;
        item1_id = 0;
        item2_id = 0;
        item_type = 0;
        item4_id = 0;
        item1_num = 0;
        item_quality = 0;
        item3_num = 0;
        id = 0;
        item4_num = 0;
    }

    void parse(TiXmlElement* xmlRoot);
};

struct DECOMPOSE_ROOT_STRUCT
{
    std::vector<DECOMPOSE_ROOT_FORMULA_STRUCT> formula_list;

    void clear()
    {
        formula_list.clear();
    }

    void parse(TiXmlElement* xmlRoot);
};

class DECOMPOSE_ENTRY
{
    DECLARE_SINGLETON(DECOMPOSE_ENTRY)
public:
    bool LoadConfig(const char* path);
    const DECOMPOSE_ROOT_STRUCT & GetStruct() const { return m_Struct;}
private:
    DECOMPOSE_ROOT_STRUCT m_Struct;
};
#endif //decompose_config_h__
