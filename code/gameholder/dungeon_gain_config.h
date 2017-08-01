/*
 *	副本奖励表
 */
#ifndef dungeon_gain_config_h__
#define dungeon_gain_config_h__

typedef struct DungeonGainTemplate
{
    void Clear()
    {
        id              = 0;
        name            = "";
        exp             = 0;
        soul            = 0;
        game_point      = 0;
        gain_items.clear();
    }

    uint32                          id;                 // ID
    std::string                     name;               // 名字
    int32                           exp;                // 经验
    int32                           soul;               // 精魂
    int32                           game_point;         // 钱
    std::vector<REWARD_ITEM>        gain_items;         // 奖励物品
} DungeonGainTemplate;

class DungeonGainConfig
{
public:
    DungeonGainConfig();
    ~DungeonGainConfig();

    DECLARE_SINGLETON(DungeonGainConfig)

    bool LoadConfig(const char* path);

    const DungeonGainTemplate* GetDungeonGainTemplate(uint32 dungeon_gainId);

    // 获取副本道具奖励
    void GetDungeonGainReward(const DungeonGainTemplate* gainTmpl, int32 num, std::vector<REWARD_ITEM>& rewards);

    // 获取副本道具奖励(在排除列表之外的)
    void GetDungeonGainRewardEx(const DungeonGainTemplate* gainTmpl, int32 num, std::vector<REWARD_ITEM>& rewards, const std::vector<REWARD_ITEM>& excludedRewards);

    // 获取副本奖励(道具,经验,金币等)
    void GetRewardTable(const DungeonGainTemplate* gainTmpl, int32 num, REWARD_TABLE& rewardTable);

    // 获取副本奖励(道具,经验,金币等),包括额外的加成
    void GetRewardTableIncludeExtra(const DungeonGainTemplate* gainTmpl, int32 num, uint32 score, uint32 vipLevel, REWARD_TABLE& rewardTable);
    
    // 获取服务奖励加成百分比
    void GetExtraRewardPercent( uint32 score, uint32 vipLevel, real& extraExpPercent, real& extraSoulPercent, real& extraGamePointPercent);

private:
    std::map<uint32, DungeonGainTemplate> m_dungeon_gains;
};

#endif // dungeon_gain_config_h__
