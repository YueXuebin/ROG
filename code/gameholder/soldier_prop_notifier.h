#ifndef soldier_prop_notifier_h__
#define soldier_prop_notifier_h__

class Soldier;

/*
 *	佣兵的属性通知器(只通知给对应的客户端)
 *  注意：所有的属性都要再这里通知(和player的不同),不能依赖场景通知机制。
 */
class SoldierPropNotifier
{
public:
    SoldierPropNotifier(Soldier* pOwner);
    ~SoldierPropNotifier();

    void SetDirty() {m_Dirty = true;}
    void Update();
    void ForceUpdate();

private:
    void NotifyPlayerProp(PROPERTY_LIST_NTF& list);

private:
    Soldier*    m_pOwner;
    bool        m_Dirty;

    //------------------------------------------------------------------------------------------
    // 一级属性
    int32       m_strength;
    int32       m_agility;
    int32       m_spirit;
    int32       m_constitution;

    //------------------------------------------------------------------------------------------
    // 基础属性(level,hp,maxhp,mp,maxmp,energy_shield,max_energy_shield,movespeed,由PropNotifier通知)
    int32       m_hp;
    int32       m_mp;
    int32       m_es;

    int32       m_maxHp;
    int32       m_maxMp;
    int32       m_maxEs;
    
    int32       m_moveSpeed;

    int32       m_res_hp;
    int32       m_res_mp;

    int32       m_Damage;           // 基础伤害

    int32       m_defence;
    int32       m_Dodge;

    int32       m_HitRate;
    int32       m_CriticalAttack;
    int32       m_CriticalAttackDamageAddtion;

    int32       m_AttackSpeed;
    int32       m_CastSpeed;

    int32       m_fire_defence;
    int32       m_ice_defence;
    int32       m_thunder_defence;

    int32       m_attack_block;
    int32       m_magic_block;

    //------------------------------------------------------------------------------------------
    // 其他属性
    int32       m_level;
    int32       m_exp;
    int32       m_max_exp;
    int32       m_equip_animation;

};


#endif // soldier_prop_notifier_h__
