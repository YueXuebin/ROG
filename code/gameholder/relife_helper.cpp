#include "gameholder_pch.h"
#include "relife_helper.h"
#include "battle_unit.h"


ReHpHelper::ReHpHelper(BattleUnit* owner) : 
m_owner(owner),
m_AddVal(0.f)
{
    m_PeriodTimer = Crown::SDGetTickCount();
}

void ReHpHelper::Update()
{
    if(!m_owner || m_owner->IsDead())
        return;

    uint32 curr_time = Crown::SDGetTickCount();

    // 恢复周期
    int32 period = MILL_SECONDE/2;
    if((m_PeriodTimer + period) <= curr_time)
    {
        int32 difftime = curr_time - m_PeriodTimer;

        m_AddVal += ((real)difftime / MILL_SECONDE) * m_owner->GetHpRecovery();
        m_PeriodTimer = curr_time;

        if(fabs(m_AddVal) > 1)
        {
            int32 addval = (int32)m_AddVal;
            if(addval>0)
            {
                m_owner->AddHP(addval);
            }
            else
            {
                m_owner->SubHP(-addval);
            }
            m_AddVal -= addval;
        }
    }
}

