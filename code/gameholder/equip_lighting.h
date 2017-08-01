#ifndef equip_lighting_h__
#define equip_lighting_h__

typedef struct EquipLightingItem
{
    void Clear()
    {
        level =             0;
        num =               0;
        addProlist.clear();
        
    }
    int32 level;
    int32 num;
    PropertyList addProlist;//填到的其他属性
}EquipLightingItem;

class EquipLightingConfig
{
private:
    typedef std::vector<EquipLightingItem> StarList;                     ///< 升星列表
    typedef std::vector<EquipLightingItem> QualityList;                  ///< 品质列表
    typedef std::vector<EquipLightingItem> JewelList;                    ///< 宝石列表

    void LoadProList(std::string base_value, PropertyList* list);

    StarList starlist;
    QualityList qualitylist;
    JewelList jewellist;

public:
    EquipLightingConfig(void);
    ~EquipLightingConfig(void);

    DECLARE_SINGLETON(EquipLightingConfig)

    bool LoadConfig(const char* path);

    const EquipLightingItem* GetEquipStarLisghtingConfig(int32 starNum);       ///< 升星点灯配置
    const EquipLightingItem* GetEquipQuelityLisghtingConfig(int32 level);      ///< 品质点灯配置
    const EquipLightingItem* GetEquipJewelLisghtingConfig(int32 jewelLevel);   ///< 宝石点灯配置
};

#endif // equip_lighting_h__