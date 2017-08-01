#include "gameholder_pch.h"
#include "buff_func_base.h"
#include "battle_unit.h"
#include "buff.h"
#include "buff_controller.h"
#include "player.h"

BuffFuncBase::BuffFuncBase(uint32 funcId) :
m_FuncId(funcId),
m_BuffValIndex(0)
{
    m_property_map.clear();
}

BuffFuncBase::~BuffFuncBase()
{

}

BattleUnit* BuffFuncBase::GetOwner()
{
    return m_pBuff->GetOwner();
}

BuffController* BuffFuncBase::GetOwnerBuffController()
{
    return m_pBuff->GetOwner()->GetBuffController();
}

int32 BuffFuncBase::GetProperty(uint32 property_id)
{
    if(m_property_map.find(property_id) == m_property_map.end())
        return 0;
    return m_property_map[property_id];
}

void BuffFuncBase::SetProperty( uint32 property_id, int32 value )
{
    m_property_map[property_id] = value;

    GetOwner()->GetBuffController()->SetPropertyDirty();       // buff影响的属性值发生改变
}

void BuffFuncBase::End()
{
    if (!m_property_map.empty())
    {
        // 如果该buff影响角色属性，消失时要通知属性改变
        GetOwner()->GetBuffController()->SetPropertyDirty();
    }
}

void BuffFuncBase::AddProperty(uint32 property_id, int32 val)
{
    if(GetOwner()->GetObjectType() == OBJ_PLAYER)
    {
        Player* player = (Player*)GetOwner();
        switch(property_id)
        {
        case PROP_GAMEPOINT:
            player->AddGamePoint(val, REWARD_REASON_BUFF);
            break;
        case PROP_SOULPOINT:
            player->AddSoulPoint(val, REWARD_REASON_BUFF);
            break;
        case PROP_EXP:
            player->AddExp(val);
            break;
        default:
            CnAssert(false);            // 需要时可以添加
            break;
        }
    }
}

void BuffFuncBase::SubProperty(uint32 property_id, int32 val)
{
    // 以后有减少属性可以添加
    CnError("buff func property id: %d is not exist!\n", property_id);
}

void BuffFuncBase::AddDamage(const DamageValue& damage)
{
    GetOwner()->GetBuffController()->AddBuffUpdateDamage(damage, m_pBuff->GetCasterID());
}

void BuffFuncBase::AddHeal(int32 heal)
{
    CnAssert(heal >= 0);
    GetOwner()->GetBuffController()->AddBuffUpdateHeal(heal, m_pBuff->GetCasterID());
}

int32 BuffFuncBase::GetBuffVal()
{
    if(m_BuffValIndex == 0)
    {
        return m_pBuff->GetBuffVal1();
    }
    else if(m_BuffValIndex == 1)
    {
        return m_pBuff->GetBuffVal2();
    }
    else
    {
        CnAssert(false);
        return m_pBuff->GetBuffVal2();
    }
}
