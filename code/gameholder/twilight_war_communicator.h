/*
 *	暮光之战属性共享器
 */
#ifndef twilight_war_communicator_h__
#define twilight_war_communicator_h__

#include "battle_unit_communicator.h"

class Boss;
class BattleUnit;
class DungeonSet;
class Player;

struct TwilightWarRankReward;

class TwilightWarCommunicator : public BattleUnitCommunicator
{
public:
    TwilightWarCommunicator(DungeonSet* dungeonSet);
    ~TwilightWarCommunicator();

    void    Update();

    void    OnConnect(BattleUnit* pBattleUnit);
    void    OnDisconnect(BattleUnit* pBattleUnit);

    void    OnDead(BattleUnit* pAttacker);
    void    OnDamage(BattleUnit* pAttacker, int32 damage);

    void    OnEnd(bool success);

private:

    void    Clear();

    void    ClearDamageInfo();

    void    SecondUpdate();

    void    UpdateDamage();

    void    BossKillProcess(BattleUnit* KillAttacker);                  ///< BOSS 被击杀

    void    SendTwilightWarRankSave();                                  ///< 通知 CS 服务器保存排行信息

    void    BossLevelUp();                                              ///< BOSS 升级

    void    RewardByRank(std::set<uint64>& scenesPlayers);              ///< 根据排行发放奖励

    typedef struct TwilightWarDamageInfo
    {
        void Clear()
        {
            guild_name      = "";
            guild_id        = 0;
            damage          = 0;
            battle_power    = 0;
            level           = 0;
        }
        std::string guild_name;
        uint64      guild_id;
        uint32      damage;
        int32       battle_power;
        int32       level;
    } TwilightWarDamageInfo;
    typedef std::map<uint64, TwilightWarDamageInfo> DamageMap;
    typedef std::vector<TwilightWarDamageInfo*>     DamageList;

    bool                m_start;
    std::string         m_bossName;
    std::string         m_attackGuild;
    uint32              m_allDamage;
    uint32              m_bossCreatureID;
    uint64              m_playerID;
    int32               m_SecondTimer;

    static bool SortDamage(TwilightWarDamageInfo* a, TwilightWarDamageInfo* b);

    TwilightWarDamageInfo* GetNotifyGuildDamageInfo(WORLD_BOSS_DAMAGE_LIST& list, Player* player);

    std::vector<Boss*>  m_bosses;       // 各频道中的此BOSS

    DamageMap           m_damageMap;
    DamageList          m_damageList;
};


#endif // twilight_war_communicator_h__
