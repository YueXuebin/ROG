/*
 * 勇敢的心表
 */
#ifndef brave_heart_config_h__
#define brave_heart_config_h__

typedef struct DiffTemplate
{
    int32                   id;                 ///< id
    int32                   dungeon_id;         ///< 副本
    int32                   boss_id;            ///< boss icon
    int32                   heart_point;        ///< 获得的心点

    void Clear()
    {
        id                  = 0;
        dungeon_id          = 0;
        boss_id             = 0;
        heart_point         = 0;
    }
} DiffTemplate;

typedef struct AttrTemplate
{
    int32                   heart_point;
    int32                   attr_value;

    void Clear()
    {
        heart_point         = 0;
        attr_value          = 0;
    }
} AttrTemplate;

typedef struct RedeemTemplate
{
    int32                           id;
    std::vector<AttrTemplate>       attrs;

    void Clear()
    {
        id                  = 0;
        attrs.clear();
    }
} RedeemTemplate;


typedef struct StageTemplate
{
    int32                   id;                 ///< 
    std::vector<DiffTemplate> diffs;            ///< 
    std::vector<RedeemTemplate> redeems;        ///< 兑换属性列表

    void Clear()
    {
        id                  = 0;
        diffs.clear();
        redeems.clear();
    }
} StageTemplate;

typedef struct TierTemplate
{
    int32                   id;                 ///< 
    std::vector<StageTemplate> stages;          ///< 

    void Clear()
    {
        id                  = 0;
        stages.clear();
    }
} TierTemplate;

typedef std::vector<TierTemplate>  TierList;

////////////////////////////////////////////////////////////////////////////

typedef struct RewardTemplate
{
    int32                           heart_point;        ///< 满多少心点奖励
    std::vector<REWARD_ITEM>        gain_items;         ///< 奖励物品

    void Clear()
    {
        heart_point         = 0;
        gain_items.clear();
    }
} RewardTemplate;

typedef struct TierRewardTemplate
{
    int32                   id;                     ///< tier id
    std::vector<RewardTemplate>     rewardList;     ///< 奖励列表

    void Clear()
    {
        id                  = 0;
        rewardList.clear();
    }
} TierRewardTemplate;

typedef std::vector<TierRewardTemplate> TierRewardList;


typedef struct CountInfoTemplate
{
    int32   num;                        ///< 次数
    int32   money;                      ///< 魔石
    int32   card;                       ///< 卡片

    void Clear()
    {
        num     = 0;
        money   = 0;
        card    = 0;
    }
} CountInfoTemplate;

typedef std::vector<CountInfoTemplate> CountInfoList;
////////////////////////////////////////////////////////////////////////////

class BraveHeartConfig
{
public:
    BraveHeartConfig();
    ~BraveHeartConfig();

    DECLARE_SINGLETON(BraveHeartConfig) 

    bool LoadConfig(const char* path);

private:
    bool LoadAttrRedeems(const std::string& attrStr, RedeemTemplate& redeem);

public:
    int32 GetDungeonId(int32 mTierId, int32 mStageId, int32 mDiff);
    int32 GetHeartPoint(int32 mTierId, int32 mStageId, int32 mDiff);
    int32 GetRedeemAttrValue(int32 mTierId, int32 mStageId, int32 mAttrId, int32 mHeartPointCost);
    void GetRewardTable(int32 mTierId, int32 mHeartPointSum, REWARD_TABLE& rewardTable);
    const CountInfoTemplate* GetCountInfo(int32 num);

private:
    TierList m_tierList;                // Tier列表
    TierRewardList m_rewardList;        // 奖励列表
    CountInfoList m_countList;          // 次数购买信息
};

#endif // brave_heart_config_h__