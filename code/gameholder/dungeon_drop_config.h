//auto created by structual_xml.py
#ifndef dungeon_drop_config_h__
#define dungeon_drop_config_h__

struct DUNGEON_DROP_ROOT_DROP_STRUCT
{
    int32 package_id;
    int32 drop_probability;
    int32 id;

    void clear()
    {
        package_id = 0;
        drop_probability = 0;
        id = 0;
    }

    void parse(TiXmlElement* xmlRoot);
};

struct DUNGEON_DROP_ROOT_STRUCT
{
    std::vector<DUNGEON_DROP_ROOT_DROP_STRUCT> drop_list;

    void clear()
    {
        drop_list.clear();
    }

    void parse(TiXmlElement* xmlRoot);
};

class DUNGEON_DROP_ENTRY
{
    DECLARE_SINGLETON(DUNGEON_DROP_ENTRY)
public:
    bool LoadConfig(const char* path);
    const DUNGEON_DROP_ROOT_STRUCT & GetStruct() const { return m_Struct;}
private:
    DUNGEON_DROP_ROOT_STRUCT m_Struct;
};
#endif //dungeon_drop_config_h__
