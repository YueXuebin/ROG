#ifndef world_boss_config_h__
#define world_boss_config_h__

typedef struct WorldBossReward
{
    void clear()
    {
        rewardTable.Clear();
        rank = 0;
    }

    REWARD_TABLE rewardTable;
    uint32 rank;
} WorldBossReward;

typedef std::map<uint32, WorldBossReward> WorldBossRewardList;

typedef struct WorldBossRewardListByType
{
    void clear()
    {
        WinRewardList.clear();
        FaildRewardList.clear();
    }

    WorldBossRewardList WinRewardList;
    WorldBossRewardList FaildRewardList;
}WorldBossRewardListByType;

class WorldBossConfig
{
public:
    WorldBossConfig();
    ~WorldBossConfig();

    DECLARE_SINGLETON(WorldBossConfig)

    bool LoadWorldBossConfig(const char* path);

    const REWARD_TABLE* GetWorldBossReward(uint32 dongeon_id, uint32 rank, bool Win);

private:
    std::map<uint32, WorldBossRewardListByType> m_mapWorldBossReward; // 奖励表 <地图ID, 奖励>
};

#endif // world_boss_config_h__
