#ifndef player_prop_notifier_h__
#define player_prop_notifier_h__

class Player;

/*
 *	玩家角色的属性通知器(只通知给对应的客户端)
 *  注意：prop_notifier里管理的属性，这里就不再管理了
 */
class PlayerPropNotifier
{
public:
    PlayerPropNotifier(Player* owner);
    ~PlayerPropNotifier();

    void SetDirty() {m_Dirty = true;}
    void Update();
    void ForceUpdate();

private:
    void NotifyPlayerProp(PROPERTY_LIST_NTF& list);

private:
    Player* m_Owner;
    bool    m_Dirty;

    //------------------------------------------------------------------------------------------
    // 一级属性
    int32   m_strength;
    int32   m_agility;
    int32   m_spirit;
    int32   m_constitution;

    //------------------------------------------------------------------------------------------
    // 基础属性(level,hp,maxhp,mp,maxmp,energy_shield,max_energy_shield,movespeed,由PropNotifier通知)
    int32   m_res_hp;
    int32   m_res_mp;

    int32   m_Damage;           // 基础伤害

    int32   m_defence;
    int32   m_Dodge;

    int32   m_HitRate;
    int32   m_CriticalAttack;
    int32   m_CriticalAttackDamageAddtion;

    int32   m_AttackSpeed;
    int32   m_CastSpeed;

    int32   m_fire_defence;
    int32   m_ice_defence;
    int32   m_thunder_defence;

    int32   m_attack_block;
    int32   m_magic_block;

    //------------------------------------------------------------------------------------------
    // 其他属性
    int32   m_exp;
    int32   m_max_exp;

    int32   m_game_point;
    int32   m_money_point;
    int32   m_vigor;
    int32   m_career;

};


#endif // player_prop_notifier_h__
