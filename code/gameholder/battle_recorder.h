/*
 *	玩家对象的战斗信息处理器
 */
#ifndef battle_recorder_h__
#define battle_recorder_h__

class Scene;
class BattleUnit;
class Player;
struct DungeonScoreData;

class BattleRecorder
{
public:
    BattleRecorder(Player* player);
    ~BattleRecorder();

    void Clear();

    void onEnterScene(Scene* pScene);
    void onLeaveScene(Scene* pScene);
    
    void Update();

    void DoDamage(BattleUnit* sufferer, int32 damage);
    void OnKill(BattleUnit* sufferer);
    void OnDead(BattleUnit* killer);
    
    void ReceiveEvent( uint32 frameCount, uint32 eventId, int32 arg1, int32 arg2, int32 arg3 );

    void GainSoulPoint(int32 val);

    uint32  GetClearTime() {return m_DungeonTime;}
    int32   GetComboKillNumber() {return m_ComboKillNumber;}
    int32   GetComboKillMax() {return m_ComboKillMax > 1 ? m_ComboKillMax : 0;}
    int32   GetOnHitCount() {return m_OnHitCount;}
    int32   GetAbsorbSoul() {return m_GainSoulPoint;}

    uint32  GetAllAttackDamage() {return m_AllAttackDamage;}

    void    SetWorldBossDamage(uint32 damage) {m_WorldBossDamage = damage;}
    uint32  GetWorldBossDamage() {return m_WorldBossDamage;}

    void    SetWorldBossRank(uint32 rank) {m_WorldBossRank = rank;}
    uint32  GetWorldBossRank() {return m_WorldBossRank;}

private:
    void    Reset();

private:
    Player*     m_Player;

    uint32      m_DungeonTime;                              // 副本游戏时间ms
    uint32      m_LastDungeonId;                            // 上一个副本的id

    // 连杀相关
    int32       m_ComboKillCD;              // 连杀CD
    int32       m_ComboKillCount;           // 连杀计数
    int32       m_ComboKillMax;             // 最高连杀数
    int32       m_ComboKillNumber;          // 连杀次数

    // 被击
    int32       m_OnHitCount;               // 被击次数

    // 输出伤害
    uint32      m_AllAttackDamage;          // 总输出伤害

    // 吸收精魂数
    int32       m_GainSoulPoint;            // 吸收精魂数

    // 世界BOSS相关数据
    int32       m_WorldBossDamage;          // 对世界boss的伤害
    int32       m_WorldBossRank;            // 世界BOSS排名

};

#endif // battle_recorder_h__
