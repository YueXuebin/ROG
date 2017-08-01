#ifndef buff_h__
#define buff_h__

class BuffData;
class BattleUnit;
class BuffFuncBase;


/*
 *	buff对象
 */
class Buff
{
public:
    Buff(const BuffData* pData, uint32 CasterId, int32 buff_val1, int32 buff_val2);
    virtual ~Buff();

    bool Init(BattleUnit* pOwner);
    virtual void Update(uint32 delta_time);
    virtual bool IsAura() {return false;}

    void    SetLifeTime(int32 lifeTime);
    void    SetRemainTime(int32 lifeTime);
    void    ResetLiftTime(int32 lifeTime);
    int32   GetLifeTime();
    int32   GetRemainTime();
    
    void    SetEndTime(uint32 val);

    void    AddOverlapNum();
    int32   GetOverlapNum();

    int32   GetId();                // buff id
    int32   GetType();

    void    SetBuffVal1(int32 val);
    int32   GetBuffVal1();

    void    SetBuffVal2(int32 val);
    int32   GetBuffVal2();

    void    OnUpdateValue();

    const BuffData* GetBuffData() {return m_myBuffData;}

    BattleUnit* GetOwner() {return m_Owner;}
    BattleUnit* GetCaster();
    uint32      GetCasterID() {return m_CasterId;}

    const std::vector<BuffFuncBase*>   GetBuffFuncs() {return m_BuffFuncs;}

    DAMAGE_INFO   FilterDamage(DAMAGE_INFO damage);

    bool    CanDead();

    bool    IsImmortal();
    bool    IsShock();

    bool    IsExpMultiple(int32* pMultiple);
    bool    IsDropQuality(int32* pMultiple);

protected:
    BattleUnit*     m_Owner;        // 该buff属于谁
    uint32          m_CasterId;     // 该buff是谁放的

    BuffData*       m_myBuffData;   // 属于此对象的buff数据(会因角色属性而不同)

    std::vector<BuffFuncBase*>   m_BuffFuncs;     // buff功能
};


#endif // buff_h__
