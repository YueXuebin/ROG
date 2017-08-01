#include "gameholder_pch.h"
#include "player_prop_notifier.h"
#include "player.h"
#include "multiserver_battle_info_manager.h"

PlayerPropNotifier::PlayerPropNotifier( Player* owner ) :
m_Owner(owner)
{
    m_Dirty = false;

    m_strength = 0;
    m_agility = 0;
    m_spirit = 0;
    m_constitution = 0;

    m_res_hp = 0;
    m_res_mp = 0;

    m_Damage = 0;
    m_defence = 0;
    m_Dodge = 0;

    m_HitRate = 0;
    m_CriticalAttack = 0;
    m_CriticalAttackDamageAddtion = 0;
    m_AttackSpeed = 0;
    m_CastSpeed = 0;

    m_fire_defence = 0;
    m_ice_defence = 0;
    m_thunder_defence = 0;

    m_attack_block = 0;
    m_magic_block = 0;

    m_exp = 0;
    m_max_exp = 0;

    m_game_point = 0;
    m_money_point = 0;
    m_vigor = 0;
    m_career = 0;

}

PlayerPropNotifier::~PlayerPropNotifier()
{

}

void PlayerPropNotifier::Update()
{
    if(!m_Dirty)
        return;
    m_Dirty = false;

    ForceUpdate();
}

void PlayerPropNotifier::ForceUpdate()
{
    // 通知改变的属性给对应的玩家
    PROPERTY_LIST_NTF prop_list;
    PROPERTY_NTF val;
    val.str ="";

    //------------------------------------------------------------------------
    // 一级属性
    if(m_strength != m_Owner->GetStrength())
    {
        val.Clear();
        m_strength = m_Owner->GetStrength();
        val.property_id = PROP_STRENGTH;
        val.value = m_strength;
        prop_list.list.push_back(val);
    }

    if(m_agility != m_Owner->GetAgility())
    {
        val.Clear();
        m_agility = m_Owner->GetAgility();
        val.property_id = PROP_AGILITY;
        val.value = m_agility;
        prop_list.list.push_back(val);
    }

    if(m_spirit != m_Owner->GetSpirit())
    {
        val.Clear();
        m_spirit = m_Owner->GetSpirit();
        val.property_id = PROP_SPIRIT;
        val.value = m_spirit;
        prop_list.list.push_back(val);
    }

    if(m_constitution != m_Owner->GetConstitution())
    {
        m_constitution = m_Owner->GetConstitution();
        val.property_id = PROP_CONSTITUTION;
        val.value = m_constitution;
        prop_list.list.push_back(val);
    }

    //------------------------------------------------------------------------
    // 基础属性
    if(m_res_hp != m_Owner->GetHpRecovery())
    {
        m_res_hp = m_Owner->GetHpRecovery();
        val.property_id = PROP_HP_REC_RATE;
        val.value = m_res_hp;
        prop_list.list.push_back(val);
    }

    if(m_res_mp != m_Owner->GetMpRecovery())
    {
        m_res_mp = m_Owner->GetMpRecovery();
        val.property_id = PROP_MP_REC_RATE;
        val.value = m_res_mp;
        prop_list.list.push_back(val);
    }
    // 基础伤害
    if(m_Damage != m_Owner->GetBaseDamage())
    {
        m_Damage = m_Owner->GetBaseDamage();
        val.property_id = PROP_PHYSICS_DAMAGE;
        val.value = m_Damage;
        prop_list.list.push_back(val);
    }
    // 护甲
    if(m_defence != m_Owner->GetArmor())
    {
        m_defence = m_Owner->GetArmor();
        val.property_id = PROP_ARMOR;
        val.value = m_defence;
        prop_list.list.push_back(val);
    }
    // 闪避
    if(m_Dodge != m_Owner->GetDodge())
    {
        m_Dodge = m_Owner->GetDodge();
        val.property_id = PROP_DODGE;
        val.value = m_Dodge;
        prop_list.list.push_back(val);
    }
    // 命中
    if(m_HitRate != m_Owner->GetHit())
    {
        m_HitRate = m_Owner->GetHit();
        val.property_id = PROP_HIT_RATE;
        val.value = m_HitRate;
        prop_list.list.push_back(val);
    }
    // 暴击
    if(m_CriticalAttack != m_Owner->GetCrit())
    {
        m_CriticalAttack = m_Owner->GetCrit();
        val.property_id = PROP_CRITICAL_ATTACK;
        val.value = m_CriticalAttack;
        prop_list.list.push_back(val);
    }
    // 暴击伤害
    if(m_CriticalAttackDamageAddtion != m_Owner->GetCritDamage())
    {
        m_CriticalAttackDamageAddtion = m_Owner->GetCritDamage();
        val.property_id = PROP_CRITICAL_DAMAGE_ADDITION;
        val.value = m_CriticalAttackDamageAddtion;
        prop_list.list.push_back(val);
    }
    // 攻击速度
    if(m_AttackSpeed != m_Owner->GetAttackSpeed())
    {
        m_AttackSpeed = m_Owner->GetAttackSpeed();
        val.property_id = PROP_ATTACK_PER_SEC;
        val.value = m_AttackSpeed;
        prop_list.list.push_back(val);
    }
    // 施法速度
    if(m_CastSpeed != m_Owner->GetCastSpeed())
    {
        m_CastSpeed = m_Owner->GetCastSpeed();
        val.property_id = PROP_CAST_PER_SEC;
        val.value = m_CastSpeed;
        prop_list.list.push_back(val);
    }
    // 火抗
    if(m_fire_defence != m_Owner->GetFireResistance())
    {
        m_fire_defence = m_Owner->GetFireResistance();
        val.property_id = PROP_FIRE_DEFENCE;
        val.value = m_fire_defence;
        prop_list.list.push_back(val);
    }
    // 冰抗
    if(m_ice_defence != m_Owner->GetIceResistance())
    {
        m_ice_defence = m_Owner->GetIceResistance();
        val.property_id = PROP_ICE_DEFENCE;
        val.value = m_ice_defence;
        prop_list.list.push_back(val);
    }
    // 雷抗
    if(m_thunder_defence != m_Owner->GetLightningResistance())
    {
        m_thunder_defence = m_Owner->GetLightningResistance();
        val.property_id = PROP_THUNDER_DEFENCE;
        val.value = m_thunder_defence;
        prop_list.list.push_back(val);
    }
    // 攻击格挡
    if (m_attack_block != m_Owner->GetAttackBlock())
    {
        m_attack_block = m_Owner->GetAttackBlock();
        val.property_id = PROP_BLOCK;
        val.value = m_attack_block;
        prop_list.list.push_back(val);
    }
    // 法术格挡
    if (m_magic_block != m_Owner->GetMagicBlock())
    {
        m_magic_block = m_Owner->GetMagicBlock();
        val.property_id = PROP_MAGIC_BLOCK;
        val.value = m_magic_block;
        prop_list.list.push_back(val);
    }

    //------------------------------------------------------------------------
    // 其他属性
    if(m_exp != m_Owner->GetExp())
    {
        m_exp = m_Owner->GetExp();
        val.property_id = PROP_EXP;
        val.value = m_exp;
        prop_list.list.push_back(val);
    }

    if(m_max_exp != m_Owner->GetMaxExp())
    {
        m_max_exp = m_Owner->GetMaxExp();
        val.property_id = PROP_MAX_EXP;
        val.value = m_max_exp;
        prop_list.list.push_back(val);
    }

    if(m_game_point != m_Owner->GetGamePoint())
    {
        m_game_point = m_Owner->GetGamePoint();
        val.property_id = PROP_GAMEPOINT;
        val.value = m_game_point;
        prop_list.list.push_back(val);
    }

    if(m_money_point != m_Owner->GetMoneyPoint())
    {
        m_money_point = m_Owner->GetMoneyPoint();
        val.property_id = PROP_MONEYPOINT;
        val.value = m_money_point;
        prop_list.list.push_back(val);
    }

    if(m_vigor != m_Owner->GetVigorValue())
    {
        m_vigor = m_Owner->GetVigorValue();
        val.property_id = PROP_VIGOR;
        val.value = m_vigor;
        prop_list.list.push_back(val);
    }

    if(m_career != m_Owner->GetCareer())
    {
        m_career = m_Owner->GetCareer();
        val.property_id = PROP_CAREER;
        val.value = m_career;
        prop_list.list.push_back(val);
    }

    if(prop_list.list.size() > 0)
        NotifyPlayerProp(prop_list);
}

void PlayerPropNotifier::NotifyPlayerProp(PROPERTY_LIST_NTF& list)
{
    list.object_id = m_Owner->GetId();
    m_Owner->SendToGate(MSG_PLAYER_PROP_LIST, &list);
}