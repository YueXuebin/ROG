#include "gameholder_pch.h"
#include "buff_func_bleed.h"
#include "buff_define.h"
#include "battle_unit.h"
#include "state_controller.h"
#include "state_define.h"
#include "buff_controller.h"
#include "buff.h"


BuffFuncBleed::BuffFuncBleed() :
BuffFuncBase(BUFF_FUNC_BLEED)
{
    m_SecondTimer = 0;
}

BuffFuncBleed::~BuffFuncBleed()
{

}

void BuffFuncBleed::Start()
{
    m_SecondTimer = 0;
}

void BuffFuncBleed::Update(uint32 delta_time)
{
    m_SecondTimer -= delta_time;
    if(m_SecondTimer <= 0)
    {
        m_SecondTimer += BUFF_UPDATE_TIME;
        uint32 damageClass = GetArg1();

        int32 damagePerSceond = int32(GetBuffVal() * ((real)BUFF_UPDATE_TIME/1000));     // 每秒平均伤害

        uint32 state = GetOwner()->GetStateController()->GetState();

        DamageValue damageValue;
        if(state == STATE_RUN)
        {   // 移动时60%伤害
            damageValue.physics_value = int32(damagePerSceond * 0.6f);
        }
        else
        {   // 10%伤害
            damageValue.physics_value = int32(damagePerSceond * 0.1f);
        }

        damageValue.physics_value = Crown::SDMax(damageValue.physics_value, 1);

        if(damageValue.sum()>0)
        {
            GetOwnerBuffController()->AddBuffUpdateDamage(damageValue, m_pBuff->GetCasterID());
        }
    }
}
