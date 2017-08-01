#ifndef battle_define_h__
#define battle_define_h__

/*
 *	战斗计算相关定义
 */

#define DAMAGE_CLASS_PHYSICS    1
#define DAMAGE_CLASS_FIRE       2
#define DAMAGE_CLASS_ICE        3
#define DAMAGE_CLASS_LIGHTNING  4
#define DAMAGE_CLASS_POISON     5


typedef struct _DamageValue
{
    int32	physics_value;          // 物理伤害
    int32	fire_value;             // 火焰伤害
    int32	ice_value;              // 冰霜伤害
    int32	lightning_value;          // 闪电伤害
    int32   poison_value;           // 毒素伤害

    _DamageValue() : physics_value(0), fire_value(0), ice_value(0), lightning_value(0), poison_value(0) {}

    void operator *=(real scale) 
    { 
        physics_value   = int32(physics_value*scale);
        fire_value      = int32(fire_value*scale);
        ice_value       = int32(ice_value*scale);
        lightning_value = int32(lightning_value*scale);
        poison_value    = int32(poison_value*scale);
    }

    void Increase(const _DamageValue& val)
    {
        physics_value   += val.physics_value;
        fire_value      += val.fire_value;
        ice_value       += val.ice_value;
        lightning_value += val.lightning_value;
        poison_value    += val.poison_value;
    }

    int32 sum() {return physics_value+fire_value+ice_value+lightning_value+poison_value;}
    void Clear() {physics_value=0; fire_value=0; ice_value=0; lightning_value=0; poison_value=0;}
} DamageValue;

typedef struct _DamageResult
{
    DamageValue damageValue;
    uint32  damage_type;        // 伤害类型
    int32   sum_damage;         // 总伤害(去掉毒伤害)
    int32   poison_damage;      // 毒伤害

    _DamageResult() : damage_type(0), sum_damage(0), poison_damage(0) {}

    void Clear()
    {
        damageValue.Clear();
        damage_type = 0;
        sum_damage = 0;
        poison_damage = 0;
    }

} DamageResult;


#endif // battle_define_h__
