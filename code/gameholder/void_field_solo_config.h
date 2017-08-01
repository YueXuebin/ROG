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

    // �����ҵȼ�����
    int32 GetPlayerLevelNeed() {return m_player_level_need;}

    // ���޹㲥ID
    std::string GetBossBroadcastID() { return m_boss_broadcast_id; }

    // �����������ʱ��
    int32 GetRegularBossRunTime() { return m_regular_boss_run_time; }

    // ��ø����ʱ
    int32 GetResurgenceTime() { return m_resurgence_time; }

    // ��ͨ״̬��ձ���ĸ���(�͵�����Ʒ���ʹ���)
    int32 GetNormalChestsRate() { return m_normal_chests_rate; }

    // ����״̬��ձ���ĸ���(�͵�����Ʒ���ʹ���)
    int32 GetBossChestsRate() { return m_boss_chests_rate; }

    // ���޳�������۳��İٷֱ�
    int32 GetBossOverRangeRate() { return m_boss_overrange_rate; }

    // ���޽�ɫ�������۳�����ֱ�
    int32 GetBossPlayerDeadRate() { return m_boss_player_dead_rate; }

    // ���޳�������۳���ʱ��
    uint32 GetBossOverRangeTime() { return m_boss_overrange_time; }

    // ����bossid
    uint32 GetRareBossID () {return m_rare_boss_id;}

    // �뿪���޷�Χ֪ͨ���ʱ��
    uint32 GetOutScopeInterval () {return m_outscope_notify_interval;}

    // ��ȡ���������ʱ����
    int32 GetRewardExpInterval(int32 state_type);
    // ��ȡ�����������ֵ
    int32 GetRewardExpNum(int32 state_type);

    // ����������һ�˵Ľ���
    REWARD_TABLE& GetBossTopOneRewards() { return m_boss_topone_rewards; }

    // ����������һ�˹��ά��
    REWARD_TABLE& GetBossTopOneGuildRewards() { return m_boss_topone_guild_rewards; }

    // ����ɱBOSS�ߵĽ���
    REWARD_TABLE& GetBossKillerRewards() { return m_boss_killer_rewards; }

    // ��ñ��������Ʒ
    REWARD_ITEM* GetChestsRandItem(int32 num);

    // ��õ��������Ʒ
    REWARD_ITEM* GetDropRandItem(int32 num);

    // ��ù�����Ʒ�ͱ���������
    int32 GetDiffLevelDropRate(int32 diff_level);

    // ���Ƶ�������������
    uint32 GetChannelMaxPlayer() {return m_channel_max_player;}

    //�����ڲ�ͬģʽ���佱�������
    void LoadChestsProbability(TiXmlElement* xmlEle);

    //�õ����侭���೤ʱ��ĵ������
    std::map<uint32, int32>& GetChestsProbabilityByTime();

    Crown::CSDDateTime GetUpdateTime() {return update_time;}

    uint32 GetBossKilledLevelupSeconds() {return m_boss_killed_levelup_seconds;}

private:
    // ���ؽ���������
    bool LoadRewardExpItem(TiXmlElement* xmlEle, int32 state_type);

private:
    int32   m_player_level_need;                    ///< ��ҵȼ�����
    std::string m_boss_broadcast_id;                ///< ���޹㲥ID
    int32   m_regular_boss_run_time;                ///< ��������ʱ��
    int32   m_resurgence_time;                      ///< �����ʱ
    int32   m_normal_chests_rate;                   ///< ��ͨ״̬ɱ�ֵĸ���
    int32   m_boss_chests_rate;                     ///< ����״̬ɱ�ֵĸ���
    int32   m_boss_player_dead_rate;                ///< ���޽�ɫ�������۳�����ֱ�
    int32   m_boss_overrange_rate;                  ///< ���޳�������۳���İٷֱ�
    uint32  m_boss_overrange_time;                  ///< ���޳�������۳���ʱ��
    uint32  m_rare_boss_id;                         ///< ����BOSS id
    uint32  m_outscope_notify_interval;             ///< ����뿪���޷�Χ֪ͨ���ʱ��
    uint32  m_channel_max_player;                   ///< Ƶ�������������
    Crown::CSDDateTime update_time;                 ///< ���������ۼ�ʱ����ʱ��
    uint32  m_boss_killed_levelup_seconds;          ///< BOSS��������������level up

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
    RewardExpItemMap                m_reward_expitems;              ///< ��������

    REWARD_TABLE                    m_boss_topone_rewards;          ///< ����������һ�˵Ľ���
    REWARD_TABLE                    m_boss_topone_guild_rewards;    ///< ����������һ�˹��ά��
    REWARD_TABLE                    m_boss_killer_rewards;          ///< ����ɱBOSS�ߵĽ���
    std::vector<RAND_REWARD_ITEM>   m_chests_items;                 ///< ������Ʒ
    std::vector<RAND_REWARD_ITEM>   m_drop_items;                   ///< ������Ʒ

    typedef std::map<int32, int32> DiffLevelDropRateMap;            ///< ��ͬ�ȼ��������ʣ�<��Һ͹ֵĵȼ���������> �������Ʒ���ø���ֵ
    DiffLevelDropRateMap            m_diff_level_drop_map;          ///< �͵ȼ�����ĵ������

    std::map<uint32, int32>         m_chestsProbabilityMap;     	///< �õ����侭���೤ʱ��ĵ������

};

#endif
