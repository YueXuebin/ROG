#include "gameholder_pch.h"
#include "soldier_prop_notifier.h"
#include "soldier.h"

SoldierPropNotifier::SoldierPropNotifier(Soldier* pOwner) :
m_pOwner(pOwner)
{
    m_Dirty = false;

    m_strength = 0;
    m_agility = 0;
    m_spirit = 0;
    m_constitution = 0;

    m_hp = 0;
    m_mp = 0;
    m_es = 0;

    m_maxHp = 0;
    m_maxMp = 0;
    m_maxEs = 0;

    m_moveSpeed = 0;

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
    m_level = 0;
    m_equip_animation = 0;
}

SoldierPropNotifier::~SoldierPropNotifier()
{

}

void SoldierPropNotifier::Update()
{
    if(!m_Dirty) return;

    m_Dirty = false;

    ForceUpdate();
}

void SoldierPropNotifier::ForceUpdate()
{
    // 通知改变的属性给对应的玩家
    PROPERTY_LIST_NTF prop_list;
    PROPERTY_NTF val;
    val.str ="";

    //------------------------------------------------------------------------
    // 一级属性
    if(m_strength != m_pOwner->GetStrength())
    {
        val.Clear();
        m_strength = m_pOwner->GetStrength();
        val.property_id = PROP_STRENGTH;
        val.value = m_strength;
        prop_list.list.push_back(val);
    }

    if(m_agility != m_pOwner->GetAgility())
    {
        val.Clear();
        m_agility = m_pOwner->GetAgility();
        val.property_id = PROP_AGILITY;
        val.value = m_agility;
        prop_list.list.push_back(val);
    }

    if(m_spirit != m_pOwner->GetSpirit())
    {
        val.Clear();
        m_spirit = m_pOwner->GetSpirit();
        val.property_id = PROP_SPIRIT;
        val.value = m_spirit;
        prop_list.list.push_back(val);
    }

    if(m_constitution != m_pOwner->GetConstitution())
    {
        m_constitution = m_pOwner->GetConstitution();
        val.property_id = PROP_CONSTITUTION;
        val.value = m_constitution;
        prop_list.list.push_back(val);
    }

    //------------------------------------------------------------------------
    // 基础属性
    if(m_hp != m_pOwner->GetHP())
    {
        m_hp = m_pOwner->GetHP();
        val.property_id = PROP_HP;
        val.value = m_hp;
        prop_list.list.push_back(val);
    }

    if(m_mp != m_pOwner->GetMP())
    {
        m_mp = m_pOwner->GetMP();
        val.property_id = PROP_MP;
        val.value = m_mp;
        prop_list.list.push_back(val);
    }

    if(m_es != m_pOwner->GetEnergyShield())
    {
        m_es = m_pOwner->GetEnergyShield();
        val.property_id = PROP_ENERGY_SHIELD;
        val.value = m_es;
        prop_list.list.push_back(val);
    }

    if(m_maxHp != m_pOwner->GetMaxHp())
    {
        m_maxHp = m_pOwner->GetMaxHp();
        val.property_id = PROP_MAXHP;
        val.value = m_maxHp;
        prop_list.list.push_back(val);
    }

    if(m_maxMp != m_pOwner->GetMaxMp())
    {
        m_maxMp = m_pOwner->GetMaxMp();
        val.property_id = PROP_MAXMP;
        val.value = m_maxMp;
        prop_list.list.push_back(val);
    }

    if(m_maxEs != m_pOwner->GetMaxEnergyShield())
    {
        m_maxEs = m_pOwner->GetMaxEnergyShield();
        val.property_id = PROP_MAX_ENERGY_SHIELD;
        val.value = m_maxEs;
        prop_list.list.push_back(val);
    }

    if(m_moveSpeed != m_pOwner->GetMoveSpeed())
    {
        m_moveSpeed = m_pOwner->GetMoveSpeed();
        val.property_id = PROP_MOVE_SPEED;
        val.value = m_moveSpeed;
        prop_list.list.push_back(val);
    }

    if(m_res_hp != m_pOwner->GetHpRecovery())
    {
        m_res_hp = m_pOwner->GetHpRecovery();
        val.property_id = PROP_HP_REC_RATE;
        val.value = m_res_hp;
        prop_list.list.push_back(val);
    }

    if(m_res_mp != m_pOwner->GetMpRecovery())
    {
        m_res_mp = m_pOwner->GetMpRecovery();
        val.property_id = PROP_MP_REC_RATE;
        val.value = m_res_mp;
        prop_list.list.push_back(val);
    }
    // 基础伤害
    if(m_Damage != m_pOwner->GetBaseDamage())
    {
        m_Damage = m_pOwner->GetBaseDamage();
        val.property_id = PROP_PHYSICS_DAMAGE;
        val.value = m_Damage;
        prop_list.list.push_back(val);
    }
    // 护甲
    if(m_defence != m_pOwner->GetArmor())
    {
        m_defence = m_pOwner->GetArmor();
        val.property_id = PROP_ARMOR;
        val.value = m_defence;
        prop_list.list.push_back(val);
    }
    // 闪避
    if(m_Dodge != m_pOwner->GetDodge())
    {
        m_Dodge = m_pOwner->GetDodge();
        val.property_id = PROP_DODGE;
        val.value = m_Dodge;
        prop_list.list.push_back(val);
    }
    // 命中
    if(m_HitRate != m_pOwner->GetHit())
    {
        m_HitRate = m_pOwner->GetHit();
        val.property_id = PROP_HIT_RATE;
        val.value = m_HitRate;
        prop_list.list.push_back(val);
    }
    // 暴击
    if(m_CriticalAttack != m_pOwner->GetCrit())
    {
        m_CriticalAttack = m_pOwner->GetCrit();
        val.property_id = PROP_CRITICAL_ATTACK;
        val.value = m_CriticalAttack;
        prop_list.list.push_back(val);
    }
    // 暴击伤害
    if(m_CriticalAttackDamageAddtion != m_pOwner->GetCritDamage())
    {
        m_CriticalAttackDamageAddtion = m_pOwner->GetCritDamage();
        val.property_id = PROP_CRITICAL_DAMAGE_ADDITION;
        val.value = m_CriticalAttackDamageAddtion;
        prop_list.list.push_back(val);
    }
    // 攻击速度
    if(m_AttackSpeed != m_pOwner->GetAttackSpeed())
    {
        m_AttackSpeed = m_pOwner->GetAttackSpeed();
        val.property_id = PROP_ATTACK_PER_SEC;
        val.value = m_AttackSpeed;
        prop_list.list.push_back(val);
    }
    // 施法速度
    if(m_CastSpeed != m_pOwner->GetCastSpeed())
    {
        m_CastSpeed = m_pOwner->GetCastSpeed();
        val.property_id = PROP_CAST_PER_SEC;
        val.value = m_CastSpeed;
        prop_list.list.push_back(val);
    }
    // 火抗
    if(m_fire_defence != m_pOwner->GetFireResistance())
    {
        m_fire_defence = m_pOwner->GetFireResistance();
        val.property_id = PROP_FIRE_DEFENCE;
        val.value = m_fire_defence;
        prop_list.list.push_back(val);
    }
    // 冰抗
    if(m_ice_defence != m_pOwner->GetIceResistance())
    {
        m_ice_defence = m_pOwner->GetIceResistance();
        val.property_id = PROP_ICE_DEFENCE;
        val.value = m_ice_defence;
        prop_list.list.push_back(val);
    }
    // 雷抗
    if(m_thunder_defence != m_pOwner->GetLightningResistance())
    {
        m_thunder_defence = m_pOwner->GetLightningResistance();
        val.property_id = PROP_THUNDER_DEFENCE;
        val.value = m_thunder_defence;
        prop_list.list.push_back(val);
    }
    // 攻击格挡
    if (m_attack_block != m_pOwner->GetAttackBlock())
    {
        m_attack_block = m_pOwner->GetAttackBlock();
        val.property_id = PROP_BLOCK;
        val.value = m_attack_block;
        prop_list.list.push_back(val);
    }
    // 法术格挡
    if (m_magic_block != m_pOwner->GetMagicBlock())
    {
        m_magic_block = m_pOwner->GetMagicBlock();
        val.property_id = PROP_MAGIC_BLOCK;
        val.value = m_magic_block;
        prop_list.list.push_back(val);
    }

    //------------------------------------------------------------------------
    // 其他属性
    if(m_exp != m_pOwner->GetExp())
    {
        m_exp = m_pOwner->GetExp();
        val.property_id = PROP_EXP;
        val.value = m_exp;
        prop_list.list.push_back(val);
    }

    if(m_max_exp != m_pOwner->GetMaxExp())
    {
        m_max_exp = m_pOwner->GetMaxExp();
        val.property_id = PROP_MAX_EXP;
        val.value = m_max_exp;
        prop_list.list.push_back(val);
    }

    if(m_level != m_pOwner->GetLevel())
    {
        m_level = m_pOwner->GetLevel();
        val.property_id = PROP_LEVEL;
        val.value = m_level;
        prop_list.list.push_back(val);
    }

    if(m_equip_animation != m_pOwner->GetEquipAnimation())
    {
        m_equip_animation = m_pOwner->GetEquipAnimation();
        val.property_id = PROP_EQUIP_ANIMATION;
        val.value = m_equip_animation;
        prop_list.list.push_back(val);
    }


    if(prop_list.list.empty()) return;

    NotifyPlayerProp(prop_list);
}

void SoldierPropNotifier::NotifyPlayerProp(PROPERTY_LIST_NTF& list)
{
    list.object_id = m_pOwner->GetId();
    m_pOwner->SendToGate(MSG_PLAYER_PROP_LIST, &list);
}