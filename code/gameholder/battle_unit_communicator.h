/*
 *	属性共享器
 */
#ifndef battle_uint_communicator_h__
#define battle_uint_communicator_h__

class BattleUnit;
class DungeonSet;

class BattleUnitCommunicator
{
public:
    BattleUnitCommunicator(int32 type, DungeonSet* dungeonSet);
    virtual ~BattleUnitCommunicator();

    int32           GetType() { return m_type; }
    void            SetType(int32 val) { m_type = val; }

    virtual void    Update() = 0;

    void            Connect(BattleUnit* pBattleUnit);
    void            Disconnect();

    virtual void    OnDead(BattleUnit* pAttacker) = 0;
    virtual void    OnDamage(BattleUnit* pAttacker, int32 damage) = 0;
    virtual void    OnEnd(bool success) = 0;

    void            GetScenesPlayers(std::set<uint64>& scenesPlayers);          ///< 获得所有场景中的角色

    void            OnDestroy();

    uint32          GetDungeonID();
    DungeonSet*     GetDungeonSet() { return m_dungeonSet; }

    bool            HasBattleUnit(BattleUnit* battleUnit);
    uint32          GetBattleUnitNum();
    void            DelBattleUnit(BattleUnit* battleUnit);

    // 共享属性
    void            SetHp(int32 hp);
    int32           GetHp();

    void            SetMaxHp(int32 maxhp);
    int32           GetMaxHp();

    void            SetProp(uint32 propertyId, int32 val);
    int32           GetProp(uint32 propertyId);

    enum
    {
        GUILD_BOSS_COMMUNICATOR         = 1,
        TWILIGHT_WAR_COMMUNICATOR       = 2,
        WORLD_BOSS_COMMUNICATOR         = 3
    };

protected:
    virtual void    OnConnect(BattleUnit* pBattleUnit) = 0;
    virtual void    OnDisconnect(BattleUnit* pBattleUnit) = 0;

    int32                       m_type;             // 连通器的类型
    DungeonSet*                 m_dungeonSet;       // 副本集
    int32                       m_hp;               // HP值
    int32                       m_maxhp;            // 最大HP值
    std::vector<BattleUnit*>    m_BattleUnits;      // 各频道中的此BattleUnit
    int32                       m_startcreatebooss; // 开始创建boss时间
};

#endif // battle_unit_communicator_h__
