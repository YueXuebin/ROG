/******************************************************************************
Copyright (C) Doingame Corporation. All rights reserved.
* @brief ������Ϸ����
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
    typedef std::map<uint32, BOSS_ITEM> BossItemMap;      ///< Ұ��Boss�ؿ� <����ID, ��ϸ��Ϣ>
    typedef std::map<uint32, std::string> RefreshTimeMap; ///< Bossˢ��ʱ��<order,time_point>
    typedef std::vector<RAND_REWARD_ITEM> RandRewardVec;  ///< �����Ʒ����
    typedef std::map<uint32, RandRewardVec> BossRandRewardMap;///< Boss�����Ʒ<bossID, �����Ʒ����>
    typedef std::map<uint32, REWARD_TABLE> RankRewarMap;      ///< ���н�����Ʒ<order, ��������>
    typedef std::map<uint32, RankRewarMap> BossRankRewardMap; ///< Boss���н�����Ʒ<bossID,���н���>

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
    const REWARD_ITEM* GetBossDeadRandItem(uint32 bossID, int32 num) const;            // ���Boss���������Ʒ
    const REWARD_ITEM* GetBossCorpseRandItem(uint32 bossID, int32 num) const;          // ���Boss��ʬ�����Ʒ
    const REWARD_ITEM* GetMonsterDeadRandItem(int32 num) const;         // ���С�����������Ʒ

    const REWARD_TABLE* GetDamageRankRewardItems(uint32 bossID, uint32 rank) const;
    uint32 GetHookExpValue() const {return m_hook_exp_value;}
    uint32 GetHookExpInterval() const {return m_hook_exp_interval;}
    uint32 GetBossLeakExpMulti() const {return m_boss_leak_exp_multi;}
    uint32 GetBossLeakDuration() const {return m_boss_leak_duration;}
    uint32 GetBossScourgeCorpseDuration() const {return m_boss_scourge_corpse_duration;}
    uint32 GetLeveSceneBossOverTime() const{return m_leve_scene_boss_over_time;}
    int32 GetLeveSceneBossOverRate() const {return m_leve_scene_boss_over_rate;}
    int32 GetBossPlayerDeadRate() const{ return m_boss_player_dead_rate; }

    int32 GetBossDeadPickTime() const {return m_boss_dead_pick_time;}             ///boss������ҿ�ʰȡ������Ʒ��ʱ��
    int32 GetBossDeadLifeTime() const {return m_boss_dead_life_time;}             ///boss����������Ʒϵͳʱ��
    int32 GetBossCorpsePickTime() const {return m_boss_corpse_pick_time;}           //boss��ʬ��ҿ�ʰȡ������Ʒ��ʱ��
    int32 GetBossCorpseLifeTime() const {return m_boss_corpse_life_time;}           //boss��ʬ������Ʒϵͳʱ��-
    int32 GetKillMosterPickTime() const {return m_kill_moster_pick_time;}           //ɱ��С�ֿ�ʰȡ������Ʒ��ʱ��
    int32 GetkillMosterLifeTime() const {return m_kill_moster_life_time; }          //ɱ��С�ֵ�����Ʒϵͳʱ��
private:
    bool LoadBossBaseConfig(TiXmlElement* xmlEle);// ����Boss������Ϣ
    bool LoadRefreshTimeConfig(TiXmlElement* xmlEle);// ����Bossˢ��ʱ��
    bool LoadBossDeadDropItemsConfig(TiXmlElement* xmlEle);// ����Boss����������Ʒ
    bool LoadBossCorpseDropItemsConfig(TiXmlElement* xmlEle);// ����Boss��ʬ������Ʒ
    bool LoadMonsterDeadDropItemsConfig(TiXmlElement* xmlEle);// ����С������������Ʒ
    bool LoadDamageRankRewardItemsConfig(TiXmlElement* xmlEle);// �����˺�����������Ʒ

private:
    BossItemMap                                     m_boss_map;                      ///< Boss������Ϣ
    RefreshTimeMap                                  m_refresh_time_map;              ///< Bossˢ��ʱ��

    // ������Ʒ�ͽ�����Ʒ
    BossRandRewardMap                               m_boss_dead_drop_items;          ///< Boss����������Ʒ
    BossRandRewardMap                               m_boss_corpse_drop_items;        ///< Boss��ʬ������Ʒ
    RandRewardVec                                   m_monster_dead_drop_items;       ///< С������������Ʒ
    BossRankRewardMap                               m_damage_rank_reward_items;      ///< �˺�����������Ʒ

    uint32                                           m_hook_exp_value;                ///< �һ���������
    uint32                                           m_hook_exp_interval;             ///< �һ�������

    uint32                                          m_leve_scene_boss_over_time;     ///<.�뿪��ͼ�೤ʱ�����ֵ����
    int32                                           m_leve_scene_boss_over_rate;     ///<.�뿪��ͼ�೤ʱ�����ֵ���ٵ���ֱ�
    int32                                           m_boss_player_dead_rate;         ///<.bossɱ����ҵİٷֱ�
    int32                                           m_boss_dead_pick_time;           ///boss������ҿ�ʰȡ������Ʒ��ʱ��
    int32                                           m_boss_dead_life_time;          ///boss����������Ʒϵͳʱ��
    int32                                           m_boss_corpse_pick_time;        //boss��ʬ��ҿ�ʰȡ������Ʒ��ʱ��
    int32                                           m_boss_corpse_life_time;        //boss��ʬ������Ʒϵͳʱ��-
    int32                                           m_kill_moster_pick_time;        //ɱ��С�ֿ�ʰȡ������Ʒ��ʱ��
    int32                                           m_kill_moster_life_time;        //ɱ��С�ֵ�����Ʒϵͳʱ��-

    uint32                                           m_boss_leak_exp_multi;           ///< Boss������©���鱶��
    uint32                                           m_boss_leak_duration;            ///< Boss������©ʱ��
    uint32                                           m_boss_scourge_corpse_duration;  ///< Boss��ʬʱ��
};

#endif // world_map_config_h__
