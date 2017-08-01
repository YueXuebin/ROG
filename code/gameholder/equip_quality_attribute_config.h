//auto created by structual_xml.py
#ifndef equip_quality_attribute_config_h__
#define equip_quality_attribute_config_h__

struct EQUIP_QUALITY_ATTRIBUTE_ROOT_PROP_STRUCT
{
    int32 class_14;
    int32 class_10;
    int32 class_11;
    int32 class_12;
    int32 class_13;
    int32 class_2;
    int32 class_3;
    int32 class_1;
    int32 class_6;
    int32 class_7;
    int32 class_4;
    int32 class_5;
    int32 class_8;
    int32 class_9;
    int32 id;

    void clear()
    {
        class_14 = 0;
        class_10 = 0;
        class_11 = 0;
        class_12 = 0;
        class_13 = 0;
        class_2 = 0;
        class_3 = 0;
        class_1 = 0;
        class_6 = 0;
        class_7 = 0;
        class_4 = 0;
        class_5 = 0;
        class_8 = 0;
        class_9 = 0;
        id = 0;
    }

    void parse(TiXmlElement* xmlRoot);
};

struct EQUIP_QUALITY_ATTRIBUTE_ROOT_STRUCT
{
    std::vector<EQUIP_QUALITY_ATTRIBUTE_ROOT_PROP_STRUCT> prop_list;

    void clear()
    {
        prop_list.clear();
    }

    void parse(TiXmlElement* xmlRoot);
};

class EQUIP_QUALITY_ATTRIBUTE_ENTRY
{
    DECLARE_SINGLETON(EQUIP_QUALITY_ATTRIBUTE_ENTRY)
public:
    bool LoadConfig(const char* path);
    const EQUIP_QUALITY_ATTRIBUTE_ROOT_STRUCT & GetStruct() const { return m_Struct;}
private:
    EQUIP_QUALITY_ATTRIBUTE_ROOT_STRUCT m_Struct;
};
#endif //equip_quality_attribute_config_h__
