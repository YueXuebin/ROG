#include "gameholder_pch.h"
#include "battle_manager.h"
#include "player.h"
#include "monster.h"
#include "boss.h"
#include "skill_object.h"
#include "scene.h"
#include "event_define.h"
#include "skill_config.h"
#include "skill_define.h"
#include "pet.h"
#include "regular.h"
#include "skill_info_manager.h"
#include "formula_manager.h"
#include "passive_skill_manager.h"
#include "game_util.h"
#include "property_set_calculater.h"
#include "skill_config_manager.h"
#include "buff_controller.h"


extern bool g_InBuffUpdate;

IMPLEMENT_SINGLETON(BattleManager)

BattleManager::BattleManager()
{

}

BattleManager::~BattleManager()
{

}

bool BattleManager::Init()
{
    return true;
}

// 计算攻击伤害
uint32 BattleManager::Attack( BattleUnit* Attacker, BattleUnit* Defender, SkillObject* pSkill )
{
    if (!Attacker || !Defender || !pSkill)
    {
        CnAssert(Attacker);
        CnAssert(Defender);
        CnAssert(pSkill);
        return false;
    }

    DamageValue skillDamage;

    const PropertySet& m_SkillProps = pSkill->GetSkillArgument().m_SkillProps;
    skillDamage.physics_value = m_SkillProps.GetValue(PROP_PHYSICS_DAMAGE);
    skillDamage.fire_value = m_SkillProps.GetValue(PROP_FIRE_DAMAGE);
    skillDamage.ice_value = m_SkillProps.GetValue(PROP_ICE_DAMAGE);
    skillDamage.lightning_value = m_SkillProps.GetValue(PROP_THUNDER_DAMAGE);
    skillDamage.poison_value = m_SkillProps.GetValue(PROP_POISON_DAMAGE);

    if(skillDamage.sum() == 0)
    {
        return false;         // 技能没有伤害
    }

    const SKILL_ROOT_SKILL_STRUCT* pSkillConfig = SkillConfigManager::Instance()->GetSkillConfigData(pSkill->GetSkillId());
    if(!pSkillConfig)
    {
        CnAssert(false);
        CnWarn("skill %d config not exist\n", pSkill->GetSkillId());
        return false;
    }

    DamageResult damageResult;
    int32 sub_physics_hurt = 0;
    int32 sub_magic_hurt = 0;

    // 伤害免疫
    if(Defender->IsImmortal())
    {
        damageResult.damage_type = DAMAGE_TYPE_IMMORTAL;
        goto _END;
    }

    switch(pSkillConfig->attack_type)
    {
    case SKILL_TYPE_ATTACK:                         // 攻击类
        {
            // 命中、闪避
            int32 hit = int32(((real)Attacker->GetHit()) / ((real)Attacker->GetHit() + (real)Defender->GetDodge() * 0.25f) * BASE_RATE_NUM);
            hit = Crown::SDRange(1000, hit, 10000);    
            int32 nRandom = Crown::SDRandom(BASE_RATE_NUM) + 1;     //[1, 10000]
            if (nRandom > hit)
            {
                damageResult.damage_type = DAMAGE_TYPE_MISS;
                goto _END;
            }

            // 攻击格挡
            nRandom = Crown::SDRandom(100) + 1;
            if (nRandom <= Crown::SDMax(0, Defender->GetAttackBlock()))
            {
                damageResult.damage_type = DAMAGE_TYPE_BLOCK;
            }
        }
        break;
    case SKILL_TYPE_MAGIC:
        {
            // 法术格挡
            int32 nRandom = Crown::SDRandom(100) + 1;
            if (nRandom <= Crown::SDMax(0, Defender->GetMagicBlock()))
            {
                damageResult.damage_type = DAMAGE_TYPE_BLOCK;
            }
        }
        break;
    default:
        CnAssert(false);
        break;
    }

    // 闪避直接跳过伤害计算
    // 格挡让要进行伤害计算

    // 暴击
    if (pSkill->GetSkillArgument().m_IsCritical)
    {
        skillDamage *= ToPercent(Attacker->GetCritDamage());
        if (DAMAGE_TYPE_BLOCK != damageResult.damage_type)
        {
            damageResult.damage_type = DAMAGE_TYPE_CRITICAL;
        }
    }
    else
    {
        if (DAMAGE_TYPE_BLOCK != damageResult.damage_type)
        {
            damageResult.damage_type = DAMAGE_TYPE_NORMAL;
        }
    }

    // 护甲
    if (skillDamage.physics_value > 0)
    {
        real fTemp = pow((real)skillDamage.physics_value, 1.0f/10.0f);
        fTemp = Crown::SDMax(1.0f, fTemp) * 2.0f + 1;
        fTemp = ((real)Defender->GetArmor()) / ((real)(Defender->GetArmor() + fTemp * skillDamage.physics_value));
        skillDamage.physics_value = int32(skillDamage.physics_value*(1.0f - fTemp));
    }
    
    // 抗性
    if (skillDamage.fire_value > 0)
    {
        skillDamage.fire_value = int32(((real)skillDamage.fire_value) * ((real)(100 - Defender->GetFireResistance())) / 100.0f);
    }
    if (skillDamage.ice_value > 0)
    {
        skillDamage.ice_value = int32(((real)skillDamage.ice_value) * ((real)(100 - Defender->GetIceResistance())) / 100.0f);
    }
    if (skillDamage.lightning_value > 0)
    {
        skillDamage.lightning_value = int32(((real)skillDamage.lightning_value) * ((real)(100 - Defender->GetLightningResistance())) / 100.0f);
    }

    // 感电状态伤害加50%
    if(Defender->GetBuffController()->IsShock())
    {
        skillDamage *= 1.5f;
    }

    // 物理伤害免疫
    sub_physics_hurt = Defender->GetPropertySetCalculater()->GetFinalProp().GetValue(PROP_SUB_PHYSICS_HURT);
    if(sub_physics_hurt > 0)
    {
        sub_physics_hurt = Crown::SDMax(Crown::SDMin(sub_physics_hurt, BASE_RATE_NUM), 0);
        skillDamage.physics_value = int32(skillDamage.physics_value * (1.0f - ToPercent(sub_physics_hurt)));
    }
    // 元素伤害免疫
    sub_magic_hurt = Defender->GetPropertySetCalculater()->GetFinalProp().GetValue(PROP_SUB_MAGIC_HURT);
    if(sub_magic_hurt > 0)
    {
        sub_magic_hurt = Crown::SDMax(Crown::SDMin(sub_magic_hurt, BASE_RATE_NUM), 0);
        skillDamage.fire_value = int32(skillDamage.fire_value * (1.0f - ToPercent(sub_magic_hurt)));
        skillDamage.ice_value = int32(skillDamage.ice_value * (1.0f - ToPercent(sub_magic_hurt)));
        skillDamage.lightning_value = int32(skillDamage.lightning_value * (1.0f - ToPercent(sub_magic_hurt)));
    }


    ////////////////////////////////////////////////////////////////////////////////
    // 毒伤害不参与减免
    damageResult.damageValue = skillDamage;
    damageResult.poison_damage = skillDamage.poison_value;
    damageResult.sum_damage = skillDamage.physics_value + skillDamage.fire_value + skillDamage.ice_value + skillDamage.lightning_value;

    // 最小伤害为1
    if (damageResult.sum_damage <= 0)
    {
        damageResult.sum_damage = 1;
    }

_END:
    DoDamage(damageResult, Attacker, Defender, pSkill);

    CnAssert(Attacker->GetScene() == Defender->GetScene());
    Defender->GetScene()->SendEvent(Defender->GetId(), BATTLE_EVENT_ONHIT, Attacker->GetId(), Defender->GetId(), 0);
    Attacker->GetScene()->SendEvent(Attacker->GetId(), BATTLE_EVENT_ATTACK, Defender->GetId(), Attacker->GetId(), 0);
    Attacker->GetScene()->SendEvent(0, BATTLE_EVENT_SCENE_ATTACK, Attacker->GetId(), Defender->GetId(), 0);

    // 反射伤害(攻击技能才反射伤害,毒伤害不反射)
    if(pSkillConfig->attack_type == SKILL_TYPE_ATTACK)
    {
        const PropertySet& defenderFianlProp = Defender->GetPropertySetCalculater()->GetFinalProp();
        int32 reflectDamageVal = defenderFianlProp.GetValue(PROP_REFLECT_DAMAGE);
        int32 reflectDamageMul = defenderFianlProp.GetValue(PROP_REFLECT_DAMAGE_MUL);
        if(reflectDamageVal > 0 || reflectDamageMul > 0)
        {
            DamageResult reflectDamageInfo;
            reflectDamageInfo.sum_damage = int32(damageResult.sum_damage * ToPercent(reflectDamageMul) + reflectDamageVal);

            DoDamage(reflectDamageInfo, Defender, Attacker, NULL);
        }
    }

    return damageResult.damage_type;
}

void BattleManager::DoDamage( const DamageResult& damage, BattleUnit* Attacker, BattleUnit* Defender, SkillObject* pSkill )
{
    CnAssert(!g_InBuffUpdate);  // buff做Update时,不允许调用
    if(!Defender)
    {
        CnAssert(Defender);     // 只有Defender不应为NULL
        return;
    }

    // Attacker 可能为 NULL, pSkill 可能为 NULL
    Defender->GetScene()->GetRegular()->Damage(damage, Attacker, Defender, pSkill);
}

// 允许Attacker为NULL
void BattleManager::DoHeal( int32 heal, BattleUnit* Attacker, BattleUnit* Defender )
{
    CnAssert(!g_InBuffUpdate);      // buff做Update时,不允许调用
    Defender->Heal(heal, Attacker);
}



