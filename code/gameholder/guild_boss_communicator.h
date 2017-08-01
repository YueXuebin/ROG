/*
 *	魔物讨伐战,属性共享器
 */
#ifndef guild_boss_communicator_h__
#define guild_boss_communicator_h__

#include "battle_unit_communicator.h"

class Player;
class Boss;
class BattleUnit;
class DungeonSet;

class GuildBossCommunicator : public BattleUnitCommunicator
{
public:
    GuildBossCommunicator(DungeonSet* dungeonSet);
    ~GuildBossCommunicator();

    void    Update();

    void    OnConnect(BattleUnit* pBattleUnit);
    void    OnDisconnect(BattleUnit* pBattleUnit);

    void    OnDead(BattleUnit* pAttacker);
    void    OnDamage(BattleUnit* pAttacker, int32 damage);

    void    OnEnd(bool success);

private:
    void    GetPlayersByBoss(std::set<uint64>& scenesPlayers);       ///< 获得BOSS场景中的角色

    void    Clear();

    void    ClearDamageInfo();

    void    SecondUpdate();

    void    UpdateDamage();

    void    BossKillProcess(BattleUnit* KillAttacker);

    void    Reward(bool success, std::set<uint64>& scenesPlayers);

    void    RewardByRank();

    void    SendCrusadeBossRankSave();

    typedef struct GuildBossDamageInfo
    {
        void Clear()
        {
            center_id       = 0;
            player_name     = "";
            guild_id        = 0;
            damage          = 0;
            battle_power    = 0;
            level           = 0;
        }
        uint32      center_id;
        std::string player_name;
        uint64      guild_id;
        uint32      damage;
        int32       battle_power;
        int32       level;
    } GuildBossDamageInfo;
    typedef std::map<uint64, GuildBossDamageInfo> DamageMap;
    typedef std::vector<GuildBossDamageInfo*>     DamageList;

    static bool SortDamage(GuildBossDamageInfo* a, GuildBossDamageInfo* b);
    GuildBossDamageInfo* GetNotifyGuildDamageInfo(WORLD_BOSS_DAMAGE_LIST& list, Player* player);

    bool                m_start;

    int32               m_allDamage;
    uint32              m_bossCreatureID;
    uint64              m_playerID;
    int32               m_secondTimer;

    std::vector<Boss*>  m_bosses;       // 各频道中的此BOSS

    DamageMap           m_damageMap;
    DamageList          m_damageList;
};


#endif // world_prop_communicator_h__
