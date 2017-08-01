#include "gameholder_pch.h"
#include "reenergyshield_helper.h"
#include "battle_unit.h"




ReEnergyShieldHelper::ReEnergyShieldHelper(BattleUnit* owner) :
m_owner(owner),
m_AddVal(0.f)
{
    m_PeriodTimer = Crown::SDGetTickCount();
    m_DamageTimer = 0;
}

void ReEnergyShieldHelper::OnDamage()
{
    m_DamageTimer = MILL_SECONDE * 3;           // 受到伤害后,n秒开始能量盾回复
}

void ReEnergyShieldHelper::Update()
{
    if(!m_owner || m_owner->IsDead())
        return;

    uint32 curr_time = Crown::SDGetTickCount();

    if(m_DamageTimer > 0) m_DamageTimer -= g_ServerUpdateTime;

    bool restore = true;

    if( (m_owner->GetEnergyShield() >= m_owner->GetMaxEnergyShield())
        || m_DamageTimer>0 )
    {
        restore = false;
    }

    if(!restore)
    {
        m_PeriodTimer = curr_time;
        return;
    }

    // 恢复计算周期
    int32 period = MILL_SECONDE/2;
    real fullTime = 5.f;              // 5秒恢复满能量盾
    if((m_PeriodTimer + period) <= curr_time)
    {
        int32 difftime = curr_time - m_PeriodTimer;

        m_AddVal += ((real)difftime / period) * (1.0f/5.0f * ((real)period/MILL_SECONDE) ) * m_owner->GetMaxEnergyShield();

        m_PeriodTimer = curr_time;

        if(m_AddVal > 1)
        {
            int32 addval = (int32)m_AddVal;
            m_owner->AddEnergyShield(addval);
            m_AddVal -= addval;
        }
    }
}