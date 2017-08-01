#include "gameholder_pch.h"
#include "buff_func_damage.h"
#include "buff_data.h"
#include "buff_define.h"
#include "game_define.h"
#include "buff.h"
#include "battle_unit.h"
#include "buff_controller.h"


BuffFuncDamage::BuffFuncDamage() :
BuffFuncBase(BUFF_FUNC_DAMAGE)
{
    m_SecondTimer = 0;
}

BuffFuncDamage::~BuffFuncDamage()
{

}

void BuffFuncDamage::Start()
{
    m_SecondTimer = 0;
}

void BuffFuncDamage::Update(uint32 delta_time)
{
    m_SecondTimer -= delta_time;
    if(m_SecondTimer <= 0)
    {
        m_SecondTimer += BUFF_UPDATE_TIME;
        uint32 damageClass = GetArg1();

        int32 damagePerSceond = int32(GetBuffVal() * ((real)BUFF_UPDATE_TIME/1000));     // Ã¿ÃëÆ½¾ùÉËº¦

        DamageValue damageValue;
        switch(damageClass)
        {
        case DAMAGE_CLASS_PHYSICS:
            damageValue.physics_value = damagePerSceond;
            break;
        case DAMAGE_CLASS_FIRE:
            damageValue.fire_value = damagePerSceond;
            break;
        case DAMAGE_CLASS_ICE:
            damageValue.ice_value = damagePerSceond;
            break;
        case DAMAGE_CLASS_LIGHTNING:
            damageValue.lightning_value = damagePerSceond;
            break;
        case DAMAGE_CLASS_POISON:
            damageValue.poison_value = damagePerSceond;
            break;
        default:
            CnAssert(false);
            damageValue.poison_value = damagePerSceond;
            break;
        }

        if(damageValue.sum()>0)
        {
            GetOwnerBuffController()->AddBuffUpdateDamage(damageValue, m_pBuff->GetCasterID());
        }
    }
}



