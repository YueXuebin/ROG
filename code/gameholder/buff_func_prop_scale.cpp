#include "gameholder_pch.h"
#include "buff_func_prop_scale.h"
#include "buff.h"
#include "creature.h"
#include "buff_define.h"
#include "buff_data.h"
#include "property_set_calculater.h"


BuffFuncPropScale::BuffFuncPropScale() :
BuffFuncBase(BUFF_FUNC_PROP_SCALE)
{
    m_FirstUpdate = true;
}

BuffFuncPropScale::~BuffFuncPropScale()
{

}

void BuffFuncPropScale::Start()
{

}

void BuffFuncPropScale::Update(uint32 delta_time)
{
    if(!m_FirstUpdate)
        return;
    m_FirstUpdate = false;

    BattleUnit* pOwner = m_pBuff->GetOwner();
    Creature* creature = CnDynamicCast(Creature, pOwner);
    if(!creature)
        return;

    int32 propType = GetArg1();
    if(propType == PROP_HP) propType = PROP_MAXHP;
    if(propType == PROP_MP) propType = PROP_MAXMP;

    int32 baseVal = creature->GetPropertySetCalculater()->GetBaseProp().GetValue(propType);

    int32 scaleVal = GetBuffVal() * (1 + m_pBuff->GetOverlapNum());

    int32 changeVal = int32( (real)scaleVal/10000 * baseVal );

    if(GetProperty(propType) != changeVal)
    {
        SetProperty(propType, changeVal);
    }
}
