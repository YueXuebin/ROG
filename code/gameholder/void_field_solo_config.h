/******************************************************************************
Copyright (C) Doingame Corporation. All rights reserved.
* @brief void field solo config
******************************************************************************/

#ifndef void_field_solo_config_h__
#define void_field_solo_config_h__

class VoidFieldSoloConfig
{
public:
    VoidFieldSoloConfig();
    virtual ~VoidFieldSoloConfig();

    DECLARE_SINGLETON(VoidFieldSoloConfig)

    void Clear();

    bool LoadConfig(const char* path);

    void LoadChestsItems(TiXmlElement* xmlEle);

    void LoadDropItems(TiXmlElement* xmlEle);

    void LoadDiffLevelDropRate(TiXmlElement* xmlEle);

    // 获得玩家等级需求
    int32 GetPlayerLevelNeed() {return m_player_level_need;}

    // 珍兽广播ID
    std::string GetBossBroadcastID() { return m_boss_broadcast_id; }

    // 获得珍兽运行时间
    int32 GetRegularBossRunTime() { return m_regular_boss_run_time; }

    // 获得复活倒计时
    int32 GetResurgenceTime() { return m_resurgence_time; }

    // 普通状态虚空宝箱的概率(和掉落物品概率公用)
    int32 GetNormalChestsRate() { return m_normal_chests_rate; }

    // 珍兽状态虚空宝箱的概率(和掉落物品概率公用)
    int32 GetBossChestsRate() { return m_boss_chests_rate; }

    // 珍兽超出距离扣除的百分比
    int32 GetBossOverRangeRate() { return m_boss_overrange_rate; }

    // 珍兽角色被打死扣除的万分比
    int32 GetBossPlayerDeadRate() { return m_boss_player_dead_rate; }

    // 珍兽超出距离扣除的时长
    uint32 GetBossOverRangeTime() { return m_boss_overrange_time; }

    // 珍兽bossid
    uint32 GetRareBossID () {return m_rare_boss_id;}

    // 离开珍兽范围通知间隔时间
    uint32 GetOutScopeInterval () {return m_outscope_notify_interval;}

    // 获取奖励经验的时间间隔
    int32 GetRewardExpInterval(int32 state_type);
    // 获取奖励经验的数值
    int32 GetRewardExpNum(int32 state_type);

    // 积分排名第一人的奖励
    REWARD_TABLE& GetBossTopOneRewards() { return m_boss_topone_rewards; }

    // 积分排名第一人公会奖励
    REWARD_TABLE& GetBossTopOneGuildRewards() { return m_boss_topone_guild_rewards; }

    // 最后击杀BOSS者的奖励
    REWARD_TABLE& GetBossKillerRewards() { return m_boss_killer_rewards; }

    // 获得宝箱随机物品
    REWARD_ITEM* GetChestsRandItem(int32 num);

    // 获得掉落随机物品
    REWARD_ITEM* GetDropRandItem(int32 num);

    // 获得怪物物品和宝箱掉落概率
    int32 GetDiffLevelDropRate(int32 diff_level);

    // 获得频道最大人数限制
    uint32 GetChannelMaxPlayer() {return m_channel_max_player;}

    //加载在不同模式宝箱奖掉落概率
    void LoadChestsProbability(TiXmlElement* xmlEle);

    //得到掉落经过多长时间的掉落概率
    std::map<uint32, int32>& GetChestsProbabilityByTime();

    Crown::CSDDateTime GetUpdateTime() {return update_time;}

    uint32 GetBossKilledLevelupSeconds() {return m_boss_killed_levelup_seconds;}

private:
    // 加载奖励经验项
    bool LoadRewardExpItem(TiXmlElement* xmlEle, int32 state_type);

private:
    int32   m_player_level_need;                    ///< 玩家等级需求
    std::string m_boss_broadcast_id;                ///< 珍兽广播ID
    int32   m_regular_boss_run_time;                ///< 珍兽运行时间
    int32   m_resurgence_time;                      ///< 复活倒计时
    int32   m_normal_chests_rate;                   ///< 普通状态杀怪的概率
    int32   m_boss_chests_rate;                     ///< 珍兽状态杀怪的概率
    int32   m_boss_player_dead_rate;                ///< 珍兽角色被打死扣除的万分比
    int32   m_boss_overrange_rate;                  ///< 珍兽超出距离扣除后的百分比
    uint32  m_boss_overrange_time;                  ///< 珍兽超出距离扣除的时长
    uint32  m_rare_boss_id;                         ///< 珍兽BOSS id
    uint32  m_outscope_notify_interval;             ///< 玩家离开珍兽范围通知间隔时间
    uint32  m_channel_max_player;                   ///< 频道最大人数限制
    Crown::CSDDateTime update_time;                 ///< 清空在虚空累计时常的时间
    uint32  m_boss_killed_levelup_seconds;          ///< BOSS多少秒内死亡则level up

    typedef struct REWARD_EXP_ITEM 
    {
        void Clear()
        {
            interval = 0;
            exp = 0;
        }

        int32 interval;
        int32 exp;
    } REWARD_EXP_ITEM;

    typedef std::map<int32, REWARD_EXP_ITEM> RewardExpItemMap;
    RewardExpItemMap                m_reward_expitems;              ///< 奖励经验

    REWARD_TABLE                    m_boss_topone_rewards;          ///< 积分排名第一人的奖励
    REWARD_TABLE                    m_boss_topone_guild_rewards;    ///< 积分排名第一人公会奖励
    REWARD_TABLE                    m_boss_killer_rewards;          ///< 最后击杀BOSS者的奖励
    std::vector<RAND_REWARD_ITEM>   m_chests_items;                 ///< 宝箱物品
    std::vector<RAND_REWARD_ITEM>   m_drop_items;                   ///< 掉落物品

    typedef std::map<int32, int32> DiffLevelDropRateMap;            ///< 不同等级差掉落概率：<玩家和怪的等级差，掉落概率> 宝箱和物品公用概率值
    DiffLevelDropRateMap            m_diff_level_drop_map;          ///< 低等级怪物的掉落概率

    std::map<uint32, int32>         m_chestsProbabilityMap;     	///< 得到掉落经过多长时间的掉落概率

};

#endif
