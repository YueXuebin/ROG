#include "gameholder_pch.h"
#include "buff_func_prop_change.h"
#include "creature.h"
#include "buff.h"
#include "buff_data.h"
#include "buff_define.h"


BuffFuncPropChange::BuffFuncPropChange() : 
BuffFuncBase(BUFF_FUNC_PROP_CHANGE)
{
    m_FirstUpdate = true;
}

BuffFuncPropChange::~BuffFuncPropChange()
{

}

void BuffFuncPropChange::Start()
{

}

void BuffFuncPropChange::Update(uint32 delta_time)
{
    if(!m_FirstUpdate)
        return;
    m_FirstUpdate = false;

    BattleUnit* pOwner = m_pBuff->GetOwner();
    Creature* creature = CnDynamicCast(Creature, pOwner);
    if(!creature)
        return;

    int32 changeVal = GetBuffVal() * (1 + m_pBuff->GetOverlapNum());
    int32 propType = GetArg1();

    if(GetProperty(propType) != changeVal)
    {
        SetProperty(propType, changeVal);
    }
}

