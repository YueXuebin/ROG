#include "gameholder_pch.h"
#include "buff.h"
#include "battle_unit.h"
#include "buff_func_heal.h"
#include "buff_define.h"


BuffFuncHeal::BuffFuncHeal() : 
BuffFuncBase(BUFF_FUNC_HEAL)
{
    m_SecondTimer = 0;
}

BuffFuncHeal::~BuffFuncHeal()
{

}

void BuffFuncHeal::Start()
{
    m_SecondTimer = 0;
}

void BuffFuncHeal::Update(uint32 delta_time)
{
    if(!m_pBuff->GetOwner())
    {
        CnAssert(false);
        return;
    }

    m_SecondTimer -= delta_time;
    if(m_SecondTimer <= 0)
    {
        m_SecondTimer += BUFF_UPDATE_TIME;

        int32 HealPerTimer = int32(GetBuffVal() * ((real)BUFF_UPDATE_TIME/1000)); 
        
        GetOwner()->Heal(HealPerTimer, NULL);
    }
}
