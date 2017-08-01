#ifndef zombie_config_h_
#define zombie_config_h_

struct ZombieDungeonInfo
{
    void Clear()
    {
        pre_dungeon_id = 0;
    }

    uint32 pre_dungeon_id;  //要求必须先完成的副本id
};

class ZombieConfig
{
public:
    typedef std::map<uint32, ZombieDungeonInfo> ZombieDungeonMap;     //dungeon_id-信息

    ZombieConfig();
    ~ZombieConfig();

    DECLARE_SINGLETON(ZombieConfig);

    bool LoadConfig(const char* path);

    const ZombieDungeonInfo* GetZombieDungeonInfo(uint32 dungeonId);

    uint32 GetMultipleSeconds() { return m_multipleSeconds; }

    uint32 GetNeedLiveSeconds() { return m_needLiveSeconds; }

    uint32 GetMaxCumulativeSeconds() { return m_maxCumulativeSeconds; }

    const ZombieConfig::ZombieDungeonMap& GetDungeonInfoMap() { return m_zombieDungeons; }

    const std::vector<REWARD_ITEM>& GetChestRewards() { return m_chestRewards; }

private:
    ZombieDungeonMap m_zombieDungeons;

    uint32 m_multipleSeconds;
    uint32 m_needLiveSeconds;
    uint32 m_maxCumulativeSeconds;

    std::vector<REWARD_ITEM> m_chestRewards;
};

#endif //zombie_config_h_