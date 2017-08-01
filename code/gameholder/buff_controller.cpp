#include "gameholder_pch.h"
#include "buff_controller.h"
#include "buff.h"
#include "buff_manager.h"
#include "buff_define.h"
#include "battle_unit.h"
#include "buff_data.h"
#include "buff_func_base.h"
#include "scene.h"
#include "game_util.h"
#include "battle_manager.h"
#include "property_set_calculater.h"


bool g_InBuffUpdate = false;            // 检查buff的update

BuffController::BuffController( BattleUnit* owner ) :
m_Owner(owner)
{
    m_BuffPropertyDirty = false;

    m_DamageCasterId = 0;
    m_HealCasterId = 0;

    m_BuffUpdateDamage.Clear();
    m_BuffUpdateHeal = 0;
}

BuffController::~BuffController()
{
    for(BuffList::iterator iter = m_Buffs.begin(); iter != m_Buffs.end(); iter++)
    {
        CnDelete *iter;
    }
    m_Buffs.clear();
}

void BuffController::Update(uint32 delta_time)
{
    if (!m_Owner->GetScene())
    {
        return;
    }

    // 更新buff,并销毁过期的buff
    for(BuffList::iterator buffIt = m_Buffs.begin(); buffIt != m_Buffs.end(); )
    {
        CnAssert(!m_Owner->GetCantBuff());      // 不受buff影响就不会有buff

        Buff* pBuff = (*buffIt);

        if(pBuff->GetRemainTime() <= 0)
        {
            BUFF_NTF ntf;
            ntf.object_id = m_Owner->GetId();
            ntf.change = BUFF_DEL;
            ntf.buff_id = pBuff->GetId();
            ntf.buff_val1 = pBuff->GetBuffVal1();
            ntf.buff_val2 = pBuff->GetBuffVal2();
            ntf.buff_time = 0;
            ntf.remain_time = 0;
            m_Owner->GetScene()->NotifyMessage(m_Owner, MSG_BUFF_NTF, &ntf);

            CnDelete *buffIt;
            *buffIt = NULL;
            buffIt = m_Buffs.erase(buffIt);
        }
        else
        {
            ++buffIt;
        }
    }

    // 先做光环的Update
    for(uint32 i=0; i<m_Buffs.size(); ++i)
    {
        Buff* pBuff = m_Buffs[i];
        if(pBuff->IsAura())
            pBuff->Update(delta_time);
    }

    // buff的update中,绝对不允许执行会影响buff列表的功能
    CnAssert(!g_InBuffUpdate);
    g_InBuffUpdate = true;
    for(uint32 i=0; i<m_Buffs.size(); ++i)
    {
        Buff* pBuff = m_Buffs[i];
        if(!pBuff->IsAura())
            pBuff->Update(delta_time);
    }
    CnAssert(g_InBuffUpdate);
    g_InBuffUpdate = false;

    // 更新buff影响的属性值
    if(GetPropertyDirty())
    {
        m_Owner->CalcProperty();
    }

    // 执行该Update，buff造成的伤害和治疗效果
    m_BuffUpdateDamage.sum_damage = m_BuffUpdateDamage.damageValue.physics_value+m_BuffUpdateDamage.damageValue.fire_value+m_BuffUpdateDamage.damageValue.ice_value+m_BuffUpdateDamage.damageValue.lightning_value;
    m_BuffUpdateDamage.poison_damage = m_BuffUpdateDamage.damageValue.poison_value;
    if(m_BuffUpdateDamage.sum_damage != 0 || m_BuffUpdateDamage.poison_damage != 0)
    {
        BattleUnit* damageCaster = m_Owner->GetScene()->GetBattleUnit(m_DamageCasterId);
        m_BuffUpdateDamage.damage_type = DAMAGE_TYPE_BUFF;
        BattleManager::Instance()->DoDamage(m_BuffUpdateDamage, damageCaster, m_Owner, NULL);
        m_BuffUpdateDamage.Clear();
        m_DamageCasterId = 0;
    }

    if(m_BuffUpdateHeal > 0)
    {
        BattleUnit* healCaster = m_Owner->GetScene()->GetBattleUnit(m_HealCasterId);
        BattleManager::Instance()->DoHeal(m_BuffUpdateHeal, healCaster, m_Owner);
        m_BuffUpdateHeal = 0;
        m_HealCasterId = 0;
    }
    
}

void BuffController::CalcProperty()
{
    if (!GetPropertyDirty())
    {
        return;
    }

    bool hide = false;

    m_BuffPropertyDirty = false;

    // 所有属性归零
    for(BuffPropertyMap::iterator piter = m_PropertyMap.begin(); piter != m_PropertyMap.end(); piter++)
    {
        CnAssert(piter->first);
        piter->second = 0;
    }

    // 计算Buff影响属性
    for(BuffList::iterator iter = m_Buffs.begin(); iter != m_Buffs.end(); iter++)
    {
        Buff* pBuff = *iter;
        
        const std::vector<BuffFuncBase*>& buffFuncs = pBuff->GetBuffFuncs();
        for(std::vector<BuffFuncBase*>::const_iterator iter = buffFuncs.begin(); iter != buffFuncs.end(); iter++)
        {
            const BuffFuncBase* pBuffFunc = *iter;

            const BuffFuncBase::PropertyMap& prop_map = pBuffFunc->GetPropertyMap();
            BuffFuncBase::PropertyMap::const_iterator prop_iter;
            for (prop_iter = prop_map.begin(); prop_iter != prop_map.end(); ++prop_iter)
            {
                uint32 propertyId = prop_iter->first;
                int32 porpertyValue = prop_iter->second;
                if(propertyId == 0)
                    continue;

                if(m_PropertyMap.find(propertyId) == m_PropertyMap.end())
                {
                    m_PropertyMap[propertyId] = porpertyValue;
                }
                else
                {
                    m_PropertyMap[propertyId] += porpertyValue;
                }
            }
        }
    }

    PropertySet& propSet = m_Owner->GetPropertySetCalculater()->GetBuffProp();
    propSet.Clear();

    // 更新buff影响的角色属性
    for(BuffPropertyMap::iterator piter = m_PropertyMap.begin(); piter != m_PropertyMap.end(); piter++)
    {
        uint32 propertyId = piter->first;
        int32 porpertyValue = piter->second;

        CnAssert(propertyId);
        propSet.Add(propertyId, porpertyValue);
    }


    m_Owner->SetHide(hide);
}

void BuffController::AddBuff(uint32 buff_id, uint32 buff_time, BattleUnit* pCaster, int32 buff_val1, int32 buff_val2)
{
    CnAssert(!g_InBuffUpdate);

    const BuffData* pBuffData = BuffManager::Instance()->GetBuffData(buff_id);
    if(!pBuffData)
    {
        CnAssert(false);
        return;
    }

    AddBuffByData(pBuffData, buff_time, pCaster, buff_val1, buff_val2);
}

void BuffController::DelBuff(uint32 buff_id)
{
    CnAssert(!g_InBuffUpdate);

    BuffList::iterator buffIt = m_Buffs.begin();
    for(; buffIt != m_Buffs.end(); ++buffIt)
    {
        if((*buffIt)->GetId() == buff_id)
        {
            break;
        }
    }

    if(buffIt == m_Buffs.end())
    {
        return;
    }

    int32 buffVal1 = (*buffIt)->GetBuffVal1();
    int32 buffVal2 = (*buffIt)->GetBuffVal2();

    CnDelete *buffIt;
    m_Buffs.erase(buffIt);

    // 去除buff通知
    BUFF_NTF ntf;
    ntf.object_id = m_Owner->GetId();
    ntf.change = BUFF_DEL;
    ntf.buff_id = buff_id;
    ntf.buff_val1 = buffVal1;
    ntf.buff_val2 = buffVal2;
    if(m_Owner->GetScene())
        m_Owner->GetScene()->NotifyMessage(m_Owner, MSG_BUFF_NTF, &ntf);
}

void BuffController::AddBuffByData(const BuffData* pBuffData, uint32 buff_time, BattleUnit* pCaster, int32 buff_val1, int32 buff_val2)
{
    CnAssert(!g_InBuffUpdate);

    uint32 buff_id = pBuffData->m_BuffInfo.buff_id;
    Buff* pExistBuff = GetBuff(buff_id);

    if(pBuffData->m_BuffInfo.buff_overlap == BUFF_NO_OVERLAP)
    {   // 不可叠加, 同ID不同实例buff


    }
    else if(pExistBuff)
    {
        if(pBuffData->m_BuffInfo.buff_overlap == BUFF_CAN_UPDATE)
        {   // 不可叠加，则更新时间
            if(abs(pExistBuff->GetBuffVal1()) < abs(buff_val1))
            {
                pExistBuff->SetBuffVal1(buff_val1);     // 更新用最大的buff值
            }
            if(abs(pExistBuff->GetBuffVal2()) < abs(buff_val2))
            {
                pExistBuff->SetBuffVal2(buff_val2);
            }

            pExistBuff->OnUpdateValue();

            if(pExistBuff->GetRemainTime() < (int32)buff_time)
            {
                pExistBuff->ResetLiftTime(buff_time);
                if(pBuffData->m_BuffInfo.buff_type == BUFF_TYPE_NORMAL)         // 功能性buff需要持久化保存
                    pExistBuff->SetEndTime((uint32)Crown::SDNow().GetTimeValue() + buff_time / 1000);

                BUFF_NTF ntf;
                ntf.object_id = m_Owner->GetId();
                ntf.change = BUFF_UPDATE;
                ntf.buff_id = buff_id;
                ntf.buff_val1 = pExistBuff->GetBuffVal1();
                ntf.buff_val2 = pExistBuff->GetBuffVal2();
                ntf.buff_time = buff_time;
                ntf.remain_time = buff_time;
                ntf.overlap_num = pExistBuff->GetOverlapNum();
                m_Owner->GetScene()->NotifyMessage(m_Owner, MSG_BUFF_NTF, &ntf);
            }
        }
        else if(pBuffData->m_BuffInfo.buff_overlap == BUFF_CAN_OVERLAP)
        {   // 可叠加(叠加层数)
            if(abs(pExistBuff->GetBuffVal1()) < abs(buff_val1))
            {
                pExistBuff->SetBuffVal1(buff_val1);     // 更新用最大的buff值
            }
            if(abs(pExistBuff->GetBuffVal2()) < abs(buff_val2))
            {
                pExistBuff->SetBuffVal2(buff_val2);
            }

            if(pExistBuff->GetRemainTime() < (int32)buff_time)
            {
                pExistBuff->ResetLiftTime(buff_time);
                if(pBuffData->m_BuffInfo.buff_type == BUFF_TYPE_NORMAL)
                    pExistBuff->SetEndTime((uint32)Crown::SDNow().GetTimeValue() + buff_time / 1000);
            }

            pExistBuff->AddOverlapNum();
            pExistBuff->OnUpdateValue();

            // 添加buff通知
            BUFF_NTF ntf;
            ntf.object_id = m_Owner->GetId();
            ntf.change = BUFF_UPDATE;
            ntf.buff_id = buff_id;
            ntf.buff_val1 = pExistBuff->GetBuffVal1();
            ntf.buff_val2 = pExistBuff->GetBuffVal2();
            ntf.buff_time = pExistBuff->GetLifeTime();
            ntf.remain_time = pExistBuff->GetLifeTime();
            ntf.overlap_num = pExistBuff->GetOverlapNum();
            m_Owner->GetScene()->NotifyMessage(m_Owner, MSG_BUFF_NTF, &ntf);
        }
        else
        {
            CnAssert(false);
        }

        return;
    }

    Buff* pBuff = BuffManager::Instance()->CreateBuff(pBuffData, buff_val1, buff_val2, pCaster?pCaster->GetId():0);
    pBuff->SetLifeTime(buff_time);

    if(pBuffData->m_BuffInfo.buff_type == BUFF_TYPE_NORMAL)
        pBuff->SetEndTime((uint32)Crown::SDNow().GetTimeValue() + buff_time / 1000);

    if(!pBuff->Init(m_Owner))
    {
        CnDelete pBuff;
        return;
    }

    m_Buffs.push_back(pBuff);

    // 添加buff通知
    BUFF_NTF ntf;
    ntf.object_id = m_Owner->GetId();
    ntf.change = BUFF_ADD;
    ntf.buff_id = buff_id;
    ntf.buff_val1 = pBuff->GetBuffVal1();
    ntf.buff_val2 = pBuff->GetBuffVal2();
    ntf.buff_time = buff_time;
    ntf.remain_time = buff_time;
    ntf.overlap_num = pBuff->GetOverlapNum();
    m_Owner->GetScene()->NotifyMessage(m_Owner, MSG_BUFF_NTF, &ntf);
}

Buff* BuffController::GetBuff(uint32 buffId)
{
    for(int32 i=0; i<(int32)m_Buffs.size(); i++)
    {
        if(m_Buffs[i]->GetId() == buffId)
        {
            return m_Buffs[i];
        }
    }
    return NULL;
}

Buff* BuffController::GetBuff(uint32 buffId, int32 buff_val1, int32 buff_val2)
{
    for(int32 i=0; i<(int32)m_Buffs.size(); i++)
    {
        Buff* buff = m_Buffs[i];
        if(buff->GetId() == buffId && buff->GetBuffVal1() == buff_val1 && buff->GetBuffVal2() == buff_val2)
        {
            return buff;
        }
    }
    return NULL;
}

void BuffController::ClearAllBuff()
{
    CnAssert(!g_InBuffUpdate);

    std::vector<uint32> buffList;

    for(BuffList::iterator it = m_Buffs.begin(); it != m_Buffs.end(); ++it)
    {
        Buff* pBuff = GetBuff((*it)->GetId());
        if(pBuff && pBuff->GetBuffData()->m_BuffInfo.buff_type != BUFF_TYPE_NORMAL)
        {
            buffList.push_back((*it)->GetId());
        }
    }

    for(std::vector<uint32>::iterator it = buffList.begin(); it != buffList.end(); ++it)
    {
        DelBuff(*it);
    }

}

void BuffController::FilterDamage( DAMAGE_INFO& damage )
{
    for(int32 i=0; i<(int32)m_Buffs.size(); i++)
    {
        damage = m_Buffs[i]->FilterDamage(damage);
    }
}

bool BuffController::CanDead()
{
    bool canDead = true;
    for(int32 i=0; i<(int32)m_Buffs.size(); i++)
    {
        if(!m_Buffs[i]->CanDead())
            canDead = false;
    }
    return canDead;
}

bool BuffController::IsImmortal()
{
    for(int32 i=0; i<(int32)m_Buffs.size(); i++)
    {
        if(m_Buffs[i]->IsImmortal())
            return true;
    }
    return false;
}

bool BuffController::IsShock()
{
    for(int32 i=0; i<(int32)m_Buffs.size(); i++)
    {
        if(m_Buffs[i]->IsShock())
            return true;
    }
    return false;
}

bool BuffController::IsExpMultiple(int32* pMultiple)
{
    for(int32 i=0; i<(int32)m_Buffs.size(); i++)
    {
        if(m_Buffs[i]->IsExpMultiple(pMultiple))
            return true;
    }
    return false;
}

bool BuffController::IsDropQuality(int32* pMultiple)
{
    for(int32 i=0; i<(int32)m_Buffs.size(); i++)
    {
        if(m_Buffs[i]->IsDropQuality(pMultiple))
            return true;
    }
    return false;
}

void BuffController::FillBuffState(STATE_INFO& info) const
{
    info.buff_list.clear();

    for(int32 i=0; i<(int32)m_Buffs.size(); i++)
    {
        Buff* pBuff = m_Buffs[i];
        info.buff_list.push_back( pBuff->GetBuffData()->m_BuffInfo );
    }
}

void BuffController::LoadBuffState(const STATE_INFO& info)
{
    for(std::vector<BUFF_INFO>::const_iterator buffIter = info.buff_list.begin(); buffIter != info.buff_list.end(); buffIter++)
    {
        const BUFF_INFO& buffInfo = *buffIter;

        const BuffData* pBuffData = BuffManager::Instance()->GetBuffData(buffInfo.buff_id);
        if(!pBuffData)
        {
            CnAssert(false);
            return;
        }

        // 构建Buff数据
        BuffData tmpBuffData(*pBuffData);
        tmpBuffData.m_BuffInfo = buffInfo;

        // 创建Buff对象
        Buff* pBuff = BuffManager::Instance()->CreateBuff(&tmpBuffData, buffInfo.buff_val1, buffInfo.buff_val2, 0);
        if(!pBuff->Init(m_Owner))
        {
            CnDelete pBuff;
            return;
        }

        int32 liftTime = 0;
        if(pBuff->GetBuffData()->m_BuffInfo.buff_type == BUFF_TYPE_NORMAL)
        {
            liftTime = pBuff->GetBuffData()->m_BuffInfo.end_time - (uint32)Crown::SDNow().GetTimeValue();
            if(liftTime < 0)
                liftTime = 0;

            liftTime = liftTime * 1000;
            pBuff->SetRemainTime(liftTime);
        }

        m_Buffs.push_back(pBuff);
    }
}

void BuffController::AddBuffUpdateDamage(const DamageValue& damage, uint32 damageCasterId)
{
    m_BuffUpdateDamage.damageValue.Increase(damage);
    m_DamageCasterId = damageCasterId;
}

void BuffController::AddBuffUpdateHeal(int32 heal, uint32 healCasterId)
{
    CnAssert(heal >= 0);
    m_BuffUpdateHeal += heal;
    m_HealCasterId = healCasterId;
}

void BuffController::FirstEnterGameInitBuff()
{
    for(BuffList::iterator it = m_Buffs.begin(); it != m_Buffs.end(); ++it)
    {
        Buff* pBuff = GetBuff((*it)->GetId());
        if(pBuff && pBuff->GetBuffData()->m_BuffInfo.buff_type == BUFF_TYPE_NORMAL)
        {
            // 添加buff通知
            BUFF_NTF ntf;
            ntf.object_id = m_Owner->GetId();
            ntf.change = BUFF_UPDATE;
            ntf.buff_id = pBuff->GetBuffData()->m_BuffInfo.buff_id;
            ntf.buff_val1 = pBuff->GetBuffData()->m_BuffInfo.buff_val1;
            ntf.buff_val2 = pBuff->GetBuffData()->m_BuffInfo.buff_val2;
            ntf.buff_time = pBuff->GetBuffData()->m_BuffInfo.buff_time;
            ntf.remain_time = pBuff->GetBuffData()->m_BuffInfo.buff_remain_time;
            m_Owner->GetScene()->NotifyMessage(m_Owner, MSG_BUFF_NTF, &ntf);
        }
    }
}

void BuffController::GetBuffIDList(std::vector<uint32>* buffIDList)
{
    buffIDList->clear();
    for(BuffList::iterator it = m_Buffs.begin(); it != m_Buffs.end(); ++it)
    {
        uint32 buffID = (*it)->GetId();
        buffIDList->push_back(buffID);
    }
}
