/******************************************************************************
Copyright (C) Doingame Corporation. All rights reserved.
* @brief 加载游戏配置
******************************************************************************/

#ifndef world_map_config_h__
#define world_map_config_h__

class WorldMapConfig
{
public:
    struct BOSS_ITEM 
    {
        void Clear()
        {
            dungeon_id = 0;
            scene_id = 0;
            boss_id = 0;
            enter_level = 0;
            boss_name = "";
        }
        uint32 dungeon_id;
        uint32 scene_id;
        uint32 boss_id;
        uint32 enter_level;
        std::string boss_name;
    };

    // new type define
    typedef std::map<uint32, BOSS_ITEM> BossItemMap;      ///< 野外Boss关卡 <副本ID, 详细信息>
    typedef std::map<uint32, std::string> RefreshTimeMap; ///< Boss刷新时间<order,time_point>
    typedef std::vector<RAND_REWARD_ITEM> RandRewardVec;  ///< 随机物品宝箱
    typedef std::map<uint32, RandRewardVec> BossRandRewardMap;///< Boss随机物品<bossID, 随机物品宝箱>
    typedef std::map<uint32, REWARD_TABLE> RankRewarMap;      ///< 排行奖励物品<order, 奖励宝箱>
    typedef std::map<uint32, RankRewarMap> BossRankRewardMap; ///< Boss排行奖励物品<bossID,排行奖励>

    //base method
    WorldMapConfig();
    virtual ~WorldMapConfig();

    DECLARE_SINGLETON(WorldMapConfig)

    void Clear();

    bool LoadConfig(const char* path);

    // interface for external
    const BossItemMap& GetBossConfig() const {return m_boss_map;}
    const RefreshTimeMap& GetRefreshTimeConfig() const {return m_refresh_time_map;}

    uint32 GetBossIDByUnionID(uint32 unionID) const;
    const REWARD_ITEM* GetBossDeadRandItem(uint32 bossID, int32 num) const;            // 获得Boss死亡随机物品
    const REWARD_ITEM* GetBossCorpseRandItem(uint32 bossID, int32 num) const;          // 获得Boss鞭尸随机物品
    const REWARD_ITEM* GetMonsterDeadRandItem(int32 num) const;         // 获得小怪死亡随机物品

    const REWARD_TABLE* GetDamageRankRewardItems(uint32 bossID, uint32 rank) const;
    uint32 GetHookExpValue() const {return m_hook_exp_value;}
    uint32 GetHookExpInterval() const {return m_hook_exp_interval;}
    uint32 GetBossLeakExpMulti() const {return m_boss_leak_exp_multi;}
    uint32 GetBossLeakDuration() const {return m_boss_leak_duration;}
    uint32 GetBossScourgeCorpseDuration() const {return m_boss_scourge_corpse_duration;}
    uint32 GetLeveSceneBossOverTime() const{return m_leve_scene_boss_over_time;}
    int32 GetLeveSceneBossOverRate() const {return m_leve_scene_boss_over_rate;}
    int32 GetBossPlayerDeadRate() const{ return m_boss_player_dead_rate; }

    int32 GetBossDeadPickTime() const {return m_boss_dead_pick_time;}             ///boss死亡玩家可拾取掉落物品的时间
    int32 GetBossDeadLifeTime() const {return m_boss_dead_life_time;}             ///boss死亡掉落物品系统时间
    int32 GetBossCorpsePickTime() const {return m_boss_corpse_pick_time;}           //boss鞭尸玩家可拾取掉落物品的时间
    int32 GetBossCorpseLifeTime() const {return m_boss_corpse_life_time;}           //boss鞭尸掉落物品系统时间-
    int32 GetKillMosterPickTime() const {return m_kill_moster_pick_time;}           //杀死小怪可拾取掉落物品的时间
    int32 GetkillMosterLifeTime() const {return m_kill_moster_life_time; }          //杀死小怪掉落物品系统时间
private:
    bool LoadBossBaseConfig(TiXmlElement* xmlEle);// 加载Boss基本信息
    bool LoadRefreshTimeConfig(TiXmlElement* xmlEle);// 加载Boss刷新时间
    bool LoadBossDeadDropItemsConfig(TiXmlElement* xmlEle);// 加载Boss死亡掉落物品
    bool LoadBossCorpseDropItemsConfig(TiXmlElement* xmlEle);// 加载Boss鞭尸掉落物品
    bool LoadMonsterDeadDropItemsConfig(TiXmlElement* xmlEle);// 加载小怪死亡掉落物品
    bool LoadDamageRankRewardItemsConfig(TiXmlElement* xmlEle);// 加载伤害排名奖励物品

private:
    BossItemMap                                     m_boss_map;                      ///< Boss基本信息
    RefreshTimeMap                                  m_refresh_time_map;              ///< Boss刷新时间

    // 掉落物品和奖励物品
    BossRandRewardMap                               m_boss_dead_drop_items;          ///< Boss死亡掉落物品
    BossRandRewardMap                               m_boss_corpse_drop_items;        ///< Boss鞭尸掉落物品
    RandRewardVec                                   m_monster_dead_drop_items;       ///< 小怪死亡掉落物品
    BossRankRewardMap                               m_damage_rank_reward_items;      ///< 伤害排名奖励物品

    uint32                                           m_hook_exp_value;                ///< 挂机经验数量
    uint32                                           m_hook_exp_interval;             ///< 挂机经验间隔

    uint32                                          m_leve_scene_boss_over_time;     ///<.离开地图多长时间后仇恨值减少
    int32                                           m_leve_scene_boss_over_rate;     ///<.离开地图多长时间后仇恨值减少的万分比
    int32                                           m_boss_player_dead_rate;         ///<.boss杀死玩家的百分比
    int32                                           m_boss_dead_pick_time;           ///boss死亡玩家可拾取掉落物品的时间
    int32                                           m_boss_dead_life_time;          ///boss死亡掉落物品系统时间
    int32                                           m_boss_corpse_pick_time;        //boss鞭尸玩家可拾取掉落物品的时间
    int32                                           m_boss_corpse_life_time;        //boss鞭尸掉落物品系统时间-
    int32                                           m_kill_moster_pick_time;        //杀死小怪可拾取掉落物品的时间
    int32                                           m_kill_moster_life_time;        //杀死小怪掉落物品系统时间-

    uint32                                           m_boss_leak_exp_multi;           ///< Boss霸气侧漏经验倍率
    uint32                                           m_boss_leak_duration;            ///< Boss霸气侧漏时间
    uint32                                           m_boss_scourge_corpse_duration;  ///< Boss鞭尸时间
};

#endif // world_map_config_h__
