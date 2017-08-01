/*
 *	buff控制器
 */
#ifndef buff_controller_h__
#define buff_controller_h__

#include "battle_define.h"

class Buff;
class BattleUnit;
class BuffData;
class AttackSkill;

class BuffController
{
public:
    BuffController(BattleUnit* owner);
    ~BuffController();

    void    FillBuffState(STATE_INFO& info) const;
    void    LoadBuffState(const STATE_INFO& info);

    void    Update(uint32 delta_time);

    Buff*   GetBuff(uint32 buffId);
    Buff*   GetBuff(uint32 buffId, int32 buff_val1, int32 buff_val2);
    void    AddBuff(uint32 buff_id, uint32 buff_time, BattleUnit* pCaster, int32 buff_val1, int32 buff_val2);
    void    DelBuff(uint32 buff_id);

    bool    GetPropertyDirty() { return m_BuffPropertyDirty; }
    void    SetPropertyDirty() { m_BuffPropertyDirty = true; }

    void    AddBuffUpdateDamage(const DamageValue& damage, uint32 damageCasterId);
    void    AddBuffUpdateHeal(int32 heal, uint32 healCasterId);

    void    CalcProperty();

    void    ClearAllBuff();

    void    FirstEnterGameInitBuff();                           // 第一次进入游戏初始化普通类 buff

    void    FilterDamage(DAMAGE_INFO& damage);

    bool    CanDead();

    bool    IsImmortal();
    bool    IsShock();

    bool    IsExpMultiple(int32* pMultiple);
    bool    IsDropQuality(int32* pMultiple);

    void    GetBuffIDList(std::vector<uint32>* buffIDList);

private:
    void    AddBuffByData(const BuffData* pBuffData, uint32 buff_time, BattleUnit* pCaster, int32 buff_val1, int32 buff_val2);

private:
    BattleUnit* m_Owner;

    typedef std::vector<Buff*>  BuffList;
    BuffList    m_Buffs;                // 战斗对象的所有buff

    typedef std::map<uint32, int32> BuffPropertyMap;
    BuffPropertyMap m_PropertyMap;

    bool        m_BuffPropertyDirty;    // buff影响的属性发生改变

    DamageResult m_BuffUpdateDamage;    // buff在一个Update中产生的伤害
    uint32      m_DamageCasterId;       // buff伤害的施法者
    int32       m_BuffUpdateHeal;       // buff在一个Update中产生的治疗
    uint32      m_HealCasterId;         // buff治疗的施法者
};


#endif // buff_controller_h__
