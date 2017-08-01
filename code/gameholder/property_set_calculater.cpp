#include "gameholder_pch.h"
#include "game_util.h"
#include "property_set_calculater.h"
#include "player.h"
#include "equip_manager.h"
#include "skill_info_manager.h"
#include "passive_skill_manager.h"
#include "horoscope_manager.h"
#include "wing_manager.h"
#include "sword_manager.h"
#include "formula_manager.h"
#include "soldier.h"

PropertySetCalculater::PropertySetCalculater(BattleUnit* unit) :
m_pBattleUnit(unit)
{
    m_BaseHp = 0;
}

PropertySetCalculater::~PropertySetCalculater()
{

}

Property PropertySetCalculater::CollectProperty(BattleUnit* pBattleUnit, const Property& srcProperty, PropertySet& srcPropSet, bool zeroSrc)
{
    Property desProperty;
    desProperty.type = srcProperty.type;

#ifdef DEBUG
    // 装备特有属性不会出现
    CnAssert(!srcPropSet.HasType(PROP_EQUIP_PHYSICS_DAMAGE));
    CnAssert(!srcPropSet.HasType(PROP_EQUIP_PHYSICS_DAMAGE_MUL));
    CnAssert(!srcPropSet.HasType(PROP_EQUIP_ARMOR));
    CnAssert(!srcPropSet.HasType(PROP_EQUIP_ARMOR_MUL));
    CnAssert(!srcPropSet.HasType(PROP_EQUIP_EVASION));
    CnAssert(!srcPropSet.HasType(PROP_EQUIP_EVASION_MUL));
    CnAssert(!srcPropSet.HasType(PROP_EQUIP_ENERGY_SHIELD));
    CnAssert(!srcPropSet.HasType(PROP_EQUIP_ENERGY_SHIELD_MUL));
    CnAssert(!srcPropSet.HasType(PROP_EQUIP_ATTACK_SPEED));
    CnAssert(!srcPropSet.HasType(PROP_EQUIP_ATTACK_BLOCK));
    CnAssert(!srcPropSet.HasType(PROP_EQUIP_MAGIC_BLOCK));
#endif


    //////////////////////////////////////////////////////////////////////////////////////
    // 通用属性汇总
    //////////////////////////////////////////////////////////////////////////////////////

    // 最大生命
    if(srcProperty.type == PROP_MAXHP)
    {
        int32 old_maxhp = srcProperty.value;

        int32 maxhp_mul = srcPropSet.GetValue(PROP_MAXHP_MUL);

        int32 new_maxhp = int32((old_maxhp) * (1.0f + ToPercent(maxhp_mul)));
        desProperty.value = new_maxhp;

        if(zeroSrc)
        {
            if(srcPropSet.HasType(PROP_MAXHP_MUL))
                srcPropSet.Set(PROP_MAXHP_MUL, 0);
        }
    }
    // 生命恢复
    else if(srcProperty.type == PROP_HP_REC_RATE)
    {
        int32 old_hp_rec = srcProperty.value;

        int32 hp_rec_mul = srcPropSet.GetValue(PROP_HP_REC_RATE_MUL);

        int32 new_hp_rec = int32((old_hp_rec) * (1.0f + ToPercent(hp_rec_mul)));
        desProperty.value = new_hp_rec;

        if(zeroSrc)
        {
            if(srcPropSet.HasType(PROP_HP_REC_RATE_MUL))
                srcPropSet.Set(PROP_HP_REC_RATE_MUL, 0);
        }
    }
    // 最大魔法
    else if(srcProperty.type == PROP_MAXMP)
    {
        int32 old_maxmp = srcProperty.value;

        int32 maxmp_mul = srcPropSet.GetValue(PROP_MAXMP_MUL);

        int32 new_maxmp = int32((old_maxmp) * (1.0f + ToPercent(maxmp_mul)));
        desProperty.value = new_maxmp;

        if(zeroSrc)
        {
            if(srcPropSet.HasType(PROP_MAXMP_MUL))
                srcPropSet.Set(PROP_MAXMP_MUL, 0);
        }
    }
    // 魔法恢复
    else if(srcProperty.type == PROP_MP_REC_RATE)
    {
        int32 old_mp_rec = srcProperty.value;

        int32 mp_rec_mul = srcPropSet.GetValue(PROP_MP_REC_RATE_MUL);

        int32 sub_mp_rec = srcPropSet.GetValue(PROP_MP_LOSS_RATE);          // 魔法流失

        int32 new_mp_rec = int32((old_mp_rec) * (1.0f + ToPercent(mp_rec_mul))) - sub_mp_rec;
        desProperty.value = new_mp_rec;

        if(zeroSrc)
        {
            if(srcPropSet.HasType(PROP_MP_REC_RATE_MUL))
                srcPropSet.Set(PROP_MP_REC_RATE_MUL, 0);
            if(srcPropSet.HasType(PROP_MP_LOSS_RATE))
                srcPropSet.Set(PROP_MP_LOSS_RATE, 0);
        }
    }
    // 最大能量盾
    else if(srcProperty.type == PROP_MAX_ENERGY_SHIELD)
    {
        int32 old_es = srcProperty.value;

        int32 es_mul = srcPropSet.GetValue(PROP_MAX_ENERGY_SHIELD_MUL);

        int32 new_es = int32((old_es) * (1.0f + ToPercent(es_mul)));
        desProperty.value = new_es;

        if(zeroSrc)
        {
            if(srcPropSet.HasType(PROP_MAX_ENERGY_SHIELD_MUL))
                srcPropSet.Set(PROP_MAX_ENERGY_SHIELD_MUL, 0);
        }
    }
    // 物理伤害
    else if(srcProperty.type == PROP_PHYSICS_DAMAGE)
    {
        int32 old_physics_damage = srcProperty.value;

        int32 physics_damage_mul = srcPropSet.GetValue(PROP_PHYSICS_DAMAGE_MUL);
        int32 equipAnimaiont_damage_mul = 0;
        if(pBattleUnit)
            equipAnimaiont_damage_mul = GetEquipAnimationDamageMul(pBattleUnit, srcPropSet);

        int32 new_physics_damage = int32( (old_physics_damage) * (1.0f + ToPercent(physics_damage_mul) + ToPercent(equipAnimaiont_damage_mul) ) );
        desProperty.value = new_physics_damage;

        if(zeroSrc)
        {
            if(srcPropSet.HasType(PROP_PHYSICS_DAMAGE_MUL))
                srcPropSet.Set(PROP_PHYSICS_DAMAGE_MUL, 0);
        }
    }
    // 火焰伤害
    else if(srcProperty.type == PROP_FIRE_DAMAGE)
    {
        int32 old_fire_damage = srcProperty.value;

        int32 fire_damage_mul = srcPropSet.GetValue(PROP_FIRE_DAMAGE_MUL);
        int32 magic_damage_mul = srcPropSet.GetValue(PROP_MAGIC_DAMAGE_MUL);

        int32 new_fire_damage = int32( (old_fire_damage) * (1.0f + ToPercent(fire_damage_mul) + ToPercent(magic_damage_mul)) );
        desProperty.value = new_fire_damage;

        if(zeroSrc)
        {
            if(srcPropSet.HasType(PROP_FIRE_DAMAGE_MUL))
                srcPropSet.Set(PROP_FIRE_DAMAGE_MUL, 0);
        }
    }
    // 冰霜伤害
    else if(srcProperty.type == PROP_ICE_DAMAGE)
    {
        int32 old_ice_damage = srcProperty.value;

        int32 ice_damage_mul = srcPropSet.GetValue(PROP_ICE_DAMAGE_MUL);
        int32 magic_damage_mul = srcPropSet.GetValue(PROP_MAGIC_DAMAGE_MUL);

        int32 new_ice_damage = int32( (old_ice_damage) * (1.0f + ToPercent(ice_damage_mul) + ToPercent(magic_damage_mul)) );
        desProperty.value = new_ice_damage;

        if(zeroSrc)
        {
            if(srcPropSet.HasType(PROP_ICE_DAMAGE_MUL))
                srcPropSet.Set(PROP_ICE_DAMAGE_MUL, 0);
        }
    }
    // 闪电伤害
    else if(srcProperty.type == PROP_THUNDER_DAMAGE)
    {
        int32 old_thunder_damage = srcProperty.value;

        int32 thunder_damage_mul = srcPropSet.GetValue(PROP_THUNDER_DAMAGE_MUL);
        int32 magic_damage_mul = srcPropSet.GetValue(PROP_MAGIC_DAMAGE_MUL);

        int32 new_thunder_damage = int32( (old_thunder_damage) * (1.0f + ToPercent(thunder_damage_mul) + ToPercent(magic_damage_mul)) );
        desProperty.value = new_thunder_damage;

        if(zeroSrc)
        {
            if(srcPropSet.HasType(PROP_THUNDER_DAMAGE_MUL))
                srcPropSet.Set(PROP_THUNDER_DAMAGE_MUL, 0);
        }
    }
    // 毒素伤害
    else if(srcProperty.type == PROP_POISON_DAMAGE)
    {
        int32 old_poison_damage = srcProperty.value;

        int32 poison_damage_mul = srcPropSet.GetValue(PROP_POISON_DAMAGE_MUL);

        int32 new_poison_damage = int32( (old_poison_damage) * (1.0f + ToPercent(poison_damage_mul)) );
        desProperty.value = new_poison_damage;

        if(zeroSrc)
        {
            if(srcPropSet.HasType(PROP_POISON_DAMAGE_MUL))
                srcPropSet.Set(PROP_POISON_DAMAGE_MUL, 0);
        }
    }
    // 攻击速度
    else if(srcProperty.type == PROP_ATTACK_PER_SEC)
    {
        int32 old_attack_speed = srcProperty.value;

        int32 attack_speed_mul = srcPropSet.GetValue(PROP_ATTACK_PER_SEC_MUL);

        int32 new_attack_speed = int32(old_attack_speed * (1.0f + ToPercent(attack_speed_mul)));
        desProperty.value = new_attack_speed;

        if(zeroSrc)
        {
            if(srcPropSet.HasType(PROP_ATTACK_PER_SEC_MUL))
                srcPropSet.Set(PROP_ATTACK_PER_SEC_MUL, 0);
        }
    }
    // 施法速度
    else if(srcProperty.type == PROP_CAST_PER_SEC)
    {
        int32 old_cast_speed = srcProperty.value;

        int32 cast_speed_mul = srcPropSet.GetValue(PROP_CAST_PER_SEC_MUL);

        int32 new_cast_speed = int32(old_cast_speed * (1.0f + ToPercent(cast_speed_mul)));
        desProperty.value = new_cast_speed;

        if(zeroSrc)
        {
            if(srcPropSet.HasType(PROP_CAST_PER_SEC_MUL))
                srcPropSet.Set(PROP_CAST_PER_SEC_MUL, 0);
        }
    }
    // 移动速度
    else if(srcProperty.type == PROP_MOVE_SPEED)
    {
        int32 old_move_speed = srcProperty.value;

        int32 move_speed_mul = srcPropSet.GetValue(PROP_MOVE_SPEED_MUL);

        int32 new_move_speed = int32(old_move_speed * (1.0f + ToPercent(move_speed_mul)));
        desProperty.value = new_move_speed;

        if(zeroSrc)
        {
            if(srcPropSet.HasType(PROP_MOVE_SPEED_MUL))
                srcPropSet.Set(PROP_MOVE_SPEED_MUL, 0);
        }
    }
    // 暴击
    else if(srcProperty.type == PROP_CRITICAL_ATTACK)
    {
        int32 old_crit = srcProperty.value;

        int32 crit_mul = srcPropSet.GetValue(PROP_CRITICAL_ATTACK_MUL);

        int32 new_crit = int32((old_crit) * (1.0f + ToPercent(crit_mul)));
        desProperty.value = new_crit;

        if(zeroSrc)
        {
            if(srcPropSet.HasType(PROP_CRITICAL_ATTACK_MUL))
                srcPropSet.Set(PROP_CRITICAL_ATTACK_MUL, 0);
        }
    }
    // 暴击伤害
    else if(srcProperty.type == PROP_CRITICAL_DAMAGE_ADDITION)
    {
        int32 old_crit_damage = srcProperty.value;

        int32 crit_damage_mul = srcPropSet.GetValue(PROP_CRITICAL_DAMAGE_ADDITION_MUL);

        int32 new_crit_damage = int32((old_crit_damage) * (1.0f + ToPercent(crit_damage_mul)));
        desProperty.value = new_crit_damage;

        if(zeroSrc)
        {
            if(srcPropSet.HasType(PROP_CRITICAL_DAMAGE_ADDITION_MUL))
                srcPropSet.Set(PROP_CRITICAL_DAMAGE_ADDITION_MUL, 0);
        }
    }
    // 护甲
    else if(srcProperty.type == PROP_ARMOR)
    {
        int32 old_armor = srcProperty.value;

        int32 armor_mul = srcPropSet.GetValue(PROP_ARMOR_MUL);

        int32 new_armor = int32((old_armor) * (1.0f + ToPercent(armor_mul)));
        desProperty.value = new_armor;
    
        if(zeroSrc)
        {
            if(srcPropSet.HasType(PROP_ARMOR_MUL))
                srcPropSet.Set(PROP_ARMOR_MUL, 0);
        }
    }
    // 闪避
    else if(srcProperty.type == PROP_DODGE)
    {
        int32 old_evsion = srcProperty.value;

        int32 evsion_mul = srcPropSet.GetValue(PROP_DODGE_MUL);

        int32 new_evsion = int32((old_evsion) * (1.0f + ToPercent(evsion_mul)));
        desProperty.value = new_evsion;

        if(zeroSrc)
        {
            if(srcPropSet.HasType(PROP_DODGE_MUL))
                srcPropSet.Set(PROP_DODGE_MUL, 0);
        }
    }
    // 命中
    else if(srcProperty.type == PROP_HIT_RATE)
    {
        int32 old_hit_rate = srcProperty.value;

        int32 hit_rate_mul = srcPropSet.GetValue(PROP_HIT_RATE_MUL);

        int32 new_hit_rate = int32((old_hit_rate) * (1.0f + ToPercent(hit_rate_mul)));
        desProperty.value = new_hit_rate;

        if(zeroSrc)
        {
            if(srcPropSet.HasType(PROP_HIT_RATE_MUL))
                srcPropSet.Set(PROP_HIT_RATE_MUL, 0);
        }
    }
    // 攻击格挡
    else if(srcProperty.type == PROP_BLOCK)
    {
        int32 old_block = srcProperty.value;

        int32 block_mul = srcPropSet.GetValue(PROP_BLOCK_MUL);

        int32 block_add = 0;
        if(pBattleUnit)
            block_add = GetEquipAnimationAttackBlock(pBattleUnit, srcPropSet);

        int32 new_block = int32((old_block + block_add) * (1.0f + ToPercent(block_mul)));
        desProperty.value = new_block;

        if(zeroSrc)
        {
            if(srcPropSet.HasType(PROP_BLOCK_MUL))
                srcPropSet.Set(PROP_BLOCK_MUL, 0);
        }
    }
    // 法术格挡
    else if(srcProperty.type == PROP_MAGIC_BLOCK)
    {
        int32 old_magic_block = srcProperty.value;

        int32 magic_block_mul = srcPropSet.GetValue(PROP_MAGIC_BLOCK_MUL);

        int32 new_magic_block = int32((old_magic_block) * (1.0f + ToPercent(magic_block_mul)));
        desProperty.value = new_magic_block;

        if(zeroSrc)
        {
            if(srcPropSet.HasType(PROP_MAGIC_BLOCK_MUL))
                srcPropSet.Set(PROP_MAGIC_BLOCK_MUL, 0);
        }
    }
    // 火抗
    else if(srcProperty.type == PROP_FIRE_DEFENCE)
    {
        int32 old_fire_defence = srcProperty.value;

        int32 magic_defence = srcPropSet.GetValue(PROP_MAGIC_DEFENCE);

        int32 fire_defence_mul = srcPropSet.GetValue(PROP_FIRE_DEFENCE_MUL);

        int32 new_fire_defence = int32((old_fire_defence + magic_defence) * (1.0f + ToPercent(fire_defence_mul)));
        desProperty.value = new_fire_defence;

        if(zeroSrc)
        {
            if(srcPropSet.HasType(PROP_FIRE_DEFENCE_MUL))
                srcPropSet.Set(PROP_FIRE_DEFENCE_MUL, 0);
        }
    }
    // 冰抗
    else if(srcProperty.type == PROP_ICE_DEFENCE)
    {
        int32 old_ice_defence = srcProperty.value;

        int32 magic_defence = srcPropSet.GetValue(PROP_MAGIC_DEFENCE);

        int32 ice_defence_mul = srcPropSet.GetValue(PROP_ICE_DEFENCE_MUL);

        int32 ice_defence_block = int32((old_ice_defence + magic_defence) * (1.0f + ToPercent(ice_defence_mul)));
        desProperty.value = ice_defence_block;

        if(zeroSrc)
        {
            if(srcPropSet.HasType(PROP_ICE_DEFENCE_MUL))
                srcPropSet.Set(PROP_ICE_DEFENCE_MUL, 0);
        }
    }
    // 雷抗
    else if(srcProperty.type == PROP_THUNDER_DEFENCE)
    {
        int32 old_thunder_defence = srcProperty.value;

        int32 magic_defence = srcPropSet.GetValue(PROP_MAGIC_DEFENCE);

        int32 thunder_defence_mul = srcPropSet.GetValue(PROP_THUNDER_DEFENCE_MUL);

        int32 thunder_defence_block = int32((old_thunder_defence + magic_defence) * (1.0f + ToPercent(thunder_defence_mul)));
        desProperty.value = thunder_defence_block;

        if(zeroSrc)
        {
            if(srcPropSet.HasType(PROP_THUNDER_DEFENCE_MUL))
                srcPropSet.Set(PROP_THUNDER_DEFENCE_MUL, 0);
        }
    }
    // 击杀生命恢复
    else if(srcProperty.type == PROP_KILL_HP_REC)
    {
        int32 old_kill_hp_rec = srcProperty.value;

        int32 kill_hp_rec_mul = srcPropSet.GetValue(PROP_KILL_HP_REC_MUL);

        int32 kill_hp_rec_block = int32((old_kill_hp_rec) * (1.0f + ToPercent(kill_hp_rec_mul)));
        desProperty.value = kill_hp_rec_block;

        if(zeroSrc)
        {
            if(srcPropSet.HasType(PROP_KILL_HP_REC_MUL))
                srcPropSet.Set(PROP_KILL_HP_REC_MUL, 0);
        }
    }
    // 击杀魔法恢复
    else if(srcProperty.type == PROP_KILL_MP_REC)
    {
        int32 old_kill_mp_rec = srcProperty.value;

        int32 kill_mp_rec_mul = srcPropSet.GetValue(PROP_KILL_MP_REC_MUL);

        int32 kill_mp_rec_block = int32((old_kill_mp_rec) * (1.0f + ToPercent(kill_mp_rec_mul)));
        desProperty.value = kill_mp_rec_block;

        if(zeroSrc)
        {
            if(srcPropSet.HasType(PROP_KILL_MP_REC_MUL))
                srcPropSet.Set(PROP_KILL_MP_REC_MUL, 0);
        }
    }
    else
        desProperty.value = srcProperty.value;


    return desProperty;
}

// 属性汇总算法
void PropertySetCalculater::CollectPropertySet(PropertySet& srcPropSet, BattleUnit* pBattleUnit)
{
    for(PropertyList::iterator iter = srcPropSet.m_list.begin(); iter != srcPropSet.m_list.end(); iter++)
    {
        Property& src_prop = *iter;
        Property des_prop = CollectProperty(pBattleUnit, src_prop, srcPropSet, true);
        src_prop = des_prop;
    }

    // 消除综合类属性
    if(true)
    {
        if(srcPropSet.HasType(PROP_MAGIC_DAMAGE_MUL))
            srcPropSet.Set(PROP_MAGIC_DAMAGE_MUL, 0);
        if(srcPropSet.HasType(PROP_MAGIC_DEFENCE))
            srcPropSet.Set(PROP_MAGIC_DEFENCE, 0);
        if(srcPropSet.HasType(PROP_ONE_HAND_DAMAGE_MUL))
            srcPropSet.Set(PROP_ONE_HAND_DAMAGE_MUL, 0);
        if(srcPropSet.HasType(PROP_TWO_HAND_DAMAGE_MUL))
            srcPropSet.Set(PROP_TWO_HAND_DAMAGE_MUL, 0);
        if(srcPropSet.HasType(PROP_DUAL_HAND_DAMAGE_MUL))
            srcPropSet.Set(PROP_DUAL_HAND_DAMAGE_MUL, 0);
        if(srcPropSet.HasType(PROP_BOW_DAMAGE_MUL))
            srcPropSet.Set(PROP_BOW_DAMAGE_MUL, 0);
        if(srcPropSet.HasType(PROP_SHIELD_ATTACK_BLOCK))
            srcPropSet.Set(PROP_SHIELD_ATTACK_BLOCK, 0);
        if(srcPropSet.HasType(PROP_DUAL_ATTACK_BLOCK))
            srcPropSet.Set(PROP_DUAL_ATTACK_BLOCK, 0);
    }
}

int32 PropertySetCalculater::GetEquipAnimationDamageMul(BattleUnit* pBattleUnit, PropertySet& srcPropSet)
{
    if(!pBattleUnit->IsKindOf(&Player::ms_RTTI))
        return 0;

    Player* player = CnDynamicCast(Player, pBattleUnit);

    int32 mul = 0;
    int32 equipAnimation = player->GetEquipMgr()->GetEquipAnimation();
    if(equipAnimation == EQUIP_ANIMATION_SC)
    {   // 双持
        mul = srcPropSet.GetValue(PROP_DUAL_HAND_DAMAGE_MUL);
    }
    else if(equipAnimation == EQUIP_ANIMATION_SJ
        || equipAnimation == EQUIP_ANIMATION_SZ)
    {   // 双手武器
        mul = srcPropSet.GetValue(PROP_TWO_HAND_DAMAGE_MUL);
    }
    else if(equipAnimation == EQUIP_ANIMATION_GJ)
    {   // 弓箭武器
        mul = srcPropSet.GetValue(PROP_BOW_DAMAGE_MUL);
    }
    else
    {   // 单手武器
        mul = srcPropSet.GetValue(PROP_ONE_HAND_DAMAGE_MUL);
    }

    return mul;
}

int32 PropertySetCalculater::GetEquipAnimationAttackBlock(BattleUnit* pBattleUnit, PropertySet& srcPropSet)
{
    if(!pBattleUnit->IsKindOf(&Player::ms_RTTI))
        return 0;

    Player* player = CnDynamicCast(Player, pBattleUnit);

    int32 add = 0;

    ItemEquip* pLHandEquip = player->GetEquipMgr()->GetEquipGrid()[EQUIP_PART_HAND_L];
    if(pLHandEquip && pLHandEquip->GetSubType() == EQUIP_SUB_TYPE_14)
    {   // 盾牌精通
        add += srcPropSet.GetValue(PROP_SHIELD_ATTACK_BLOCK);
    }
   
    int32 equipAnimation = player->GetEquipMgr()->GetEquipAnimation();
    if(equipAnimation == EQUIP_ANIMATION_SC)
    {   // 双持格挡
        add += srcPropSet.GetValue(PROP_DUAL_ATTACK_BLOCK);
    }

    return add;
}

void PropertySetCalculater::CalcFourAttr()
{
    // 力量
    int32 strength = m_BasePropSet.GetValue(PROP_STRENGTH);
    m_FourAttr.Set(PROP_PHYSICS_DAMAGE_MUL, strength * 50);         // 力量=0.5%物理伤害
    m_FourAttr.Set(PROP_ARMOR, strength * 5);                       // 力量=5护甲

    // 敏捷
    int32 agility = m_BasePropSet.GetValue(PROP_AGILITY);
    m_FourAttr.Set(PROP_HIT_RATE, agility * 5);                     // 1敏捷=5命中
    m_FourAttr.Set(PROP_DODGE, agility * 5);                        // 1敏捷=5闪避
    m_FourAttr.Set(PROP_CRITICAL_ATTACK, agility * 5);              // 1敏捷=0.05%暴击率
    m_FourAttr.Set(PROP_CRITICAL_DAMAGE_ADDITION, agility * 50);    // 1敏捷=0.5%暴击伤害

    // 体质
    int32 constitution = m_BasePropSet.GetValue(PROP_CONSTITUTION);
    m_FourAttr.Set(PROP_MAXHP, constitution * 5);                   // 1体质=5生命
    m_FourAttr.Set(PROP_HP_REC_RATE, int32(constitution * 0.1f));   // 1体质=0.1生命恢复

    // 智力
    int32 intelligence = m_BasePropSet.GetValue(PROP_SPIRIT);
    m_FourAttr.Set(PROP_MAGIC_DAMAGE_MUL, intelligence * 20);       // 智力=0.2%元素伤害
    m_FourAttr.Set(PROP_MAX_ENERGY_SHIELD, intelligence * 2);       // 智力=2能量盾
    m_FourAttr.Set(PROP_MAXMP, intelligence * 2);                   // 智力=2魔法值
    m_FourAttr.Set(PROP_MP_REC_RATE, int32(intelligence * 0.1f));   // 智力=0.1魔法恢复

}

/*
 *  属性计算
 *	(等级+装备+天赋+四围+翅膀)*(buff效果+地图效果)
 *  Buff和Map属性在汇总后的值上进行计算
 */
void PropertySetCalculater::CalcFinalProp()
{
    if(m_Equip.GetValue(PROP_ATTACK_PER_SEC)>0)
        m_Level.Set(PROP_ATTACK_PER_SEC, 0);            // 装备上有攻速,则不再计算角色本身的攻速


    //*********************************************************
    // Step 1: 计算角色基础属性
    //*********************************************************
    m_BasePropSet.Clear();
    m_BasePropSet.Add(m_Level);
    m_BasePropSet.Add(m_Equip);
    m_BasePropSet.Add(m_Talent);
    m_BasePropSet.Add(m_Wing);

    CalcFourAttr();

    m_BasePropSet.Add(m_FourAttr);

    //*********************************************************
    // Step 2: 地图和buff影响
    //*********************************************************
    m_BuffAndMap.Clear();
    m_BuffAndMap.Add(m_Buff);
    m_BuffAndMap.Add(m_Map);

    //*********************************************************
    // Step 3: 最终角色属性
    //*********************************************************
    m_FinalPropSet.Clear();

    m_FinalPropSet.Add(m_BasePropSet);
    CollectPropertySet(m_FinalPropSet, m_pBattleUnit);

    m_FinalPropSet.Add(m_BuffAndMap);
    CollectPropertySet(m_FinalPropSet, m_pBattleUnit);
    

    //*********************************************************
    // Step 4: 取基础属性到角色身上
    //*********************************************************
    const std::vector<uint32>& basePropertyIdList = GetBasePropertyIdList();
    for(uint32 i=0; i<basePropertyIdList.size(); i++)
    {
        Property prop;
        prop.type = basePropertyIdList[i];
        prop.value = m_FinalPropSet.GetValue(prop.type);
        m_pBattleUnit->SetProp(prop.type, prop.value);
    }

    const std::vector<uint32>& advancePropertyIdList = GetAdvancePropertyIdList();
    for(uint32 i=0; i<advancePropertyIdList.size(); i++)
    {
        Property prop;
        prop.type = advancePropertyIdList[i];
        prop.value = m_FinalPropSet.GetValue(prop.type);
        m_pBattleUnit->SetProp(prop.type, prop.value);
    }

    //*********************************************************
    // 设置四围
    //*********************************************************
    Player* player = CnDynamicCast(Player, m_pBattleUnit);
    if(player)
    {
        int32 strength = m_BasePropSet.GetValue(PROP_STRENGTH);             // 力量
        int32 agility = m_BasePropSet.GetValue(PROP_AGILITY);               // 敏捷
        int32 constitution = m_BasePropSet.GetValue(PROP_CONSTITUTION);     // 体质
        int32 intelligence = m_BasePropSet.GetValue(PROP_SPIRIT);           // 智力

        player->SetStrength(strength);
        player->SetAgility(agility);
        player->SetConstitution(constitution);
        player->SetSpirit(intelligence);
    }

    Soldier* pSoldier = CnDynamicCast(Soldier, m_pBattleUnit);
    if (pSoldier)
    {
        int32 strength = m_BasePropSet.GetValue(PROP_STRENGTH);             // 力量
        int32 agility = m_BasePropSet.GetValue(PROP_AGILITY);               // 敏捷
        int32 constitution = m_BasePropSet.GetValue(PROP_CONSTITUTION);     // 体质
        int32 intelligence = m_BasePropSet.GetValue(PROP_SPIRIT);           // 智力

        pSoldier->SetStrength(strength);
        pSoldier->SetAgility(agility);
        pSoldier->SetConstitution(constitution);
        pSoldier->SetSpirit(intelligence);
    }

    //*********************************************************
    // Step 5: 计算对技能的影响
    //*********************************************************
    player = CnDynamicCast(Player, m_pBattleUnit);
    if(player)
    {
        player->GetSkillInfoMgr()->OnPropertyChange();
        return;
    }

    pSoldier = CnDynamicCast(Soldier, m_pBattleUnit);
    if (pSoldier)
    {
        pSoldier->GetSkillInfoMgr()->OnPropertyChange();
    }
}
