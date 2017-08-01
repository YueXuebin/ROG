/*
 *	世界BOSS管理器
 */
#ifndef world_boss_manager_h__
#define world_boss_manager_h__

class BattleUnit;
class Creature;

typedef struct WorldBossDungeon
{
    uint32  dungeonID;
    bool    opening;

    WorldBossDungeon()
    {
        dungeonID = 0;
        opening = false;
    }
} WorldBossDungeon;


class WorldBossManager
{
public:
    WorldBossManager();
    ~WorldBossManager();

    enum
    {
        MAX_WORLD_BOSS_LEVEL    = 80,
        BOSS_LEVEL_UP_SECONDS   = 900
    };

    DECLARE_SINGLETON(WorldBossManager)

    void OnServerReady();

    void OnRecv(uint32 msgID, void* data);

    void Update();

    void WorldBossInfoLoadReq();                            // 请求世界BOSS信息
    void LoadWorldBoss(WORLD_BOSS_INFO_LIST& info);
    void FillWorldBoss(WORLD_BOSS_INFO_LIST& info);

    void LevelUpBoss(uint32 bossID , uint32 bosstarttime, uint64 levelUpSeconds = BOSS_LEVEL_UP_SECONDS);  // 提高世界BOSS类型
    void SendWorldBossInfoToCenter();                       // 发送世界BOSS信息给Center

    void SetBoss(uint32 save_id, uint32 boss_id, uint32 dead_time=0 ,uint32 uplevel = 0); // 设置 BOSS
    void SetBossDeadTime(uint32 boss_id, uint32 dead_time);
    uint32 GetBossDeadTime(uint32 boss_id);

    // 获得幕光之战的BossID
    uint32 GetBossID_TwilightWar() {return 30200;}      // 三头犬

    void StrongWorldBoss(Creature* battleUnit);

    void StartWorldBossDungeon(uint32 dungeonID, uint32 bossID);
    void EndWorldBossDungeon(uint32 dungeonID);

    //std::map<uint32, WorldBossDungeon>  m_worldBossDungeons;

    typedef std::map<uint32, WORLD_BOSS_INFO> WorldBossInfoMap;
    WorldBossInfoMap                    m_WorldBossInfo;     // 世界BOSS的信息

};


#endif // world_boss_manager_h__
