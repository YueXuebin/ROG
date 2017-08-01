// 圣剑
#ifndef sword_config_h__
#define sword_config_h__

typedef struct SwordStarTmpl
{
    void Clear()
    {
        star = 0;
        need_items.Clear();
        level_propety.clear();
    }

    uint32              star;
    REWARD_TABLE        need_items;
    PropertyList        level_propety;
}SwordStarTmpl;

typedef struct SwordLevelTmpl
{
    void Clear()
    {
        level = 0;
        need_items.Clear();
        star_list.clear();
    }

    uint32              level;
    REWARD_TABLE        need_items;
    std::map<uint32, SwordStarTmpl> star_list;
}SwordLevelTmpl;

typedef struct SwordEquipTmpl
{
    void Clear()
    {
        id = 0;
        item_id = 0;
        level_list.clear();
        lock_list.clear();
    }

    uint32              id;
    uint32              item_id;
    std::map<uint32, SwordLevelTmpl>    level_list;
    std::map<uint32, int32>             lock_list;
}SwordEquipTmpl;

class SwordConfig
{
public:
    SwordConfig(void);
    ~SwordConfig(void);

    DECLARE_SINGLETON(SwordConfig);

    bool LoadConfig(const char* path);

    const std::map<uint32, SwordEquipTmpl>& GetSwordMap() {return m_sword;}
    const SwordEquipTmpl* GetSwordTmpl(uint32 swordID);
    const SwordLevelTmpl* GetLevelTmpl(uint32 swordID, uint32 level);
    const SwordStarTmpl* GetStarTmpl(uint32 swordID, uint32 level, uint32 star);

    int32 GetMaxLevel(uint32 swordID);
    int32 GetMaxStarTmpl(uint32 swordID, uint32 level);
private:
    bool LoadSwordEquip(TiXmlElement* swordEquipEle);
private:
    std::map<uint32, SwordEquipTmpl>     m_sword;                    // 模板

};


#endif // sword_config_h__