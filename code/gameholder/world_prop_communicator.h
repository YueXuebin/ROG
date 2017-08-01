/*
 *	属性共享器
 */
#ifndef world_prop_communicator_h__
#define world_prop_communicator_h__

#include "battle_unit_communicator.h"

class Player;
class Boss;
class BattleUnit;
class DungeonSet;

typedef struct WorldBossDamageInfo
{
    void Clear()
    {
        player_name = "";
        player_id   = 0;
        center_id   = 0;
        damage      = 0;
    }
    std::string player_name;
    uint64      player_id;
    uint32      center_id;
    uint32      damage;
} WorldBossDamageInfo;

class WorldPropCommunicator : public BattleUnitCommunicator
{
public:
    WorldPropCommunicator(DungeonSet* dungeonSet);
    ~WorldPropCommunicator();

    void    Update();

    void    OnConnect(BattleUnit* pBattleUnit);
    void    OnDisconnect(BattleUnit* pBattleUnit);

    virtual void    OnDead(BattleUnit* pAttacker);
    virtual void    OnDamage(BattleUnit* pAttacker, int32 damage);

    void            OnEnd(bool success);

private:
    void    Clear();

    void    ClearDamageInfo();

    void    SecondUpdate();

    void    UpdateDamage();

    void    RewardDamage();

    void    BossKillProcess(BattleUnit* attacker);

private:
    void    FillWorldBossKillInfo(WORLD_BOSS_KILL_INFO_NTF& info, Player* killer);

    void    NotifyWorldBossClear(Player* player, Player* killer);

    bool            m_start;
    uint32          m_bossCreatureID;
    uint64          m_playerID;
    int32           m_SecondTimer;
    uint32          m_allDamage;

    typedef std::map<uint64, WorldBossDamageInfo>   DamageMap;
    DamageMap       m_DamageMap;
    typedef std::vector<WorldBossDamageInfo*>       DamageList;
    DamageList      m_damageList;
};


#endif // world_prop_communicator_h__
