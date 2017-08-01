#ifndef fashion_config_h__
#define fashion_config_h__


typedef struct FashionTemplate
{
    uint32                  id;                 // ID
    int32                   type;               // 类型//是购买还是合成
    PropertyList            m_prolist;          // 属性列表
    bool                    b_equp;             // 是否能装备
    std::vector<REWARD_ITEM> vec_compose;       // 合成物品的组成，类型，id，个数
    int32                   m_money;            // 装备合成花费游戏币
    int32                   flag;               // 是购买还是组合


    // 清空
    void Clear()
    {
        id              = 0;
        type            = 0;
        m_prolist.clear();
        b_equp            = false;
        vec_compose.clear();
        m_money         = 0;
    }
} FashionTemplate;

//时装列表
typedef std::vector<FashionTemplate>  VecFashionList;
//装备图鉴
typedef struct EquipAtlas
{
   VecFashionList vecfashoion; //时装列表
    PropertyList addProlist;//填到的其他属性

    void Clear()
    {
        vecfashoion.clear();
        addProlist.clear();
    }
}EquipAtlas;

typedef std::map<uint32,EquipAtlas> MapFashionAtlas;

typedef std::vector<REWARD_ITEM>  FashionItemList;
typedef std::vector<FASHION_TREASURE_CONFIG> FashionTreasureConfigItemList;

typedef struct FashionLevelUp
{
    int32 level;
    FashionItemList itemList;
    PropertyList prolist;
    void Clear()
    {
        level = 0;
        itemList.clear();
        prolist.clear();
    }
}FashionLevelUp;

typedef struct IncarnationItem
{
    int32 incarnation_id;
    REWARD_ITEM item;
    void Clear()
    {
        incarnation_id = 0;
        item.Clear();
    }
}IncarnationItem;

typedef std::vector<FashionLevelUp>  FashionLevelList;

typedef struct FashionItem
{
    int32 id;
    int32 quality;
    FashionItemList openList;
    FashionItemList incarnationCard;
    PropertyList prolist;
    FashionLevelList levelUpList;
    void Clear()
    {
        id = 0;
        quality = 0;
        openList.clear();
        incarnationCard.clear();
        prolist.clear();
        levelUpList.clear();
    }
}FashionItem;

typedef std::vector<FashionItem>  FashionConfList;

typedef struct SynergismItem
{
    int32 id;
    PropertyList prolist;
    PropertyList fashuins;
    void Clear()
    {
        id = 0;
        prolist.clear();
        fashuins.clear();
    }
}SynergismItem;

typedef std::vector<SynergismItem>  SynergismList;
typedef std::vector<IncarnationItem>  IncarnationItemList;

typedef struct FashionTreasureItem
{
    int32 type;
    int32 package_id;
    FashionItemList itemList;
    void Clear()
    {
        type = 0;
        package_id = 0;
        itemList.clear();
    }
}FashionTreasureItem;


typedef std::vector<FashionTreasureItem> TreasureList;

class FashionConfig
{
public:
    FashionConfig();
    ~FashionConfig();

    DECLARE_SINGLETON(FashionConfig) 

    bool LoadConfig(const char* path);

    const FashionItem* GetNewFashion(int32 id);                           // 获取时装信息
    const FashionItem* GetIncarnationCardFashionItem(int32 id);           // 获取幻化卡对应的时装信息
    const FashionLevelUp* GetFashionNextLevel(int32 id, int32 level);     // 获取时装下级升级条件

    const FashionTreasureItem* GetFashionTreasureNeed(int32 type);        // 获取宝藏抽取配置

    int32 GetGoldTreasureFreeTime() { return m_free_time; }             // 免费抽取间隔时间

    int32 GetBronzFirstID() { return m_bronzFirstID; }             // 铜箱第一次抽取物品
    int32 GetGoldFirstID() { return m_goldFirstID; }             // 金箱第一次抽取物品

    const FashionTreasureConfigItemList& GetBronzList() { return m_bronzList; }
    const FashionTreasureConfigItemList& GetBronzGoodList() { return m_bronzGoodList; }
    const FashionTreasureConfigItemList& GetGoldzList() { return m_goldzList; }
    const FashionTreasureConfigItemList& GetGoldzGoodList() { return m_goldzGoodList; }

    //const struct ItemTemplate* GetSaleItem(uint32 saleID, uint32 itemID);

    const SynergismList& GetSynergismList() { return m_synergismLIist; }

    int32 GetMaxQuality() { return m_maxQuality;}           ////得到最大化身的星级数

    const IncarnationItem* GetIncarnationItemFragment( int32 id );           // 获取化身兑换的碎片

private:
    PropertyList LoadFashionProList(const std::string& base_value);                     // 加载时装属性
   
    void LoadComposeItem(const std::string& base_value, FashionTemplate* fishion);      // 加载合成物品

    bool LoadFashionsConfig(const char* path);                                          // 读取新的化身配置
    void LoadFashionItemList(FashionItemList* lsit, const std::string& base_value);     // 解析升级物品
    void LoadProList(const std::string& base_value, PropertyList* list);                // 解析属性列表

    bool LoadFashionTreasureConfig(const char* path);                                   // 解析时装宝库

    bool LoadSaleConfig(const char* path);                                              // 解析购买库
    void LoadFashionTreasureList(FashionTreasureConfigItemList* rewardList, const std::string& base_value);     // 解析化身宝藏物品
    uint32 GetTimeUint(const std::string& base_value );                                   // 解析时间

private:
    FashionConfList m_fashionList;                                                      // 化身列表
    SynergismList m_synergismLIist;                                                     // 连携列表

    TreasureList m_treasureList;                                                        // 宝藏抽签条件列表

    FashionTreasureConfigItemList m_bronzList;                                          // 青铜宝藏抽取列表
    FashionTreasureConfigItemList m_bronzGoodList;                                      // 青铜宝藏高级抽取列表
    FashionTreasureConfigItemList m_goldzList;                                          // 黄金宝藏抽取列表
    FashionTreasureConfigItemList m_goldzGoodList;                                      // 黄金宝藏高级抽取列表

    int32   m_free_time;                                                                // 免费抽取间隔时间
    int32   m_bronzFirstID;                                                             // 铜箱第一次抽取物品
    int32   m_goldFirstID;                                                              // 金箱第一次抽取物品

    int32   m_maxQuality;                                                               // 最大化身的星级数

    //typedef std::vector<const struct ItemTemplate*> SaleList;
    //std::map<uint32, SaleList> m_sale_map;

    IncarnationItemList  m_incarnationItemList;
};

#endif // fashion_config_h__
