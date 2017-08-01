#include "gameholder_pch.h"
#include "buff.h"
#include "scene.h"
#include "battle_unit.h"
#include "game_define.h"
#include "buff_data.h"
#include "buff_func_base.h"
#include "buff_func_factory.h"
#include "buff_define.h"
#include "skill_config_manager.h"

int32 Buff::GetId()
{
    return m_myBuffData->m_BuffInfo.buff_id;
}

int32 Buff::GetType()
{
    return m_myBuffData->m_BuffInfo.buff_type;
}

Buff::Buff(const BuffData* pData, uint32 CasterId, int32 buff_val1, int32 buff_val2)
{
    m_myBuffData = CnNew BuffData(*pData);      // BuffData复制
    CnAssert(m_myBuffData->m_BuffInfo.overlap_num == 0);
    m_CasterId = CasterId;
    m_myBuffData->m_BuffInfo.buff_val1 = buff_val1;
    m_myBuffData->m_BuffInfo.buff_val2 = buff_val2;
    m_BuffFuncs.clear();
}

Buff::~Buff()
{
    for(uint32 i=0; i<m_BuffFuncs.size(); i++)
    {
        BuffFuncBase* pFunc = m_BuffFuncs[i];
        pFunc->End();
        CnDelete pFunc;
    }
    m_BuffFuncs.clear();

    CnDelete m_myBuffData;
}

bool Buff::Init( BattleUnit* pOwner )
{
    m_Owner = pOwner;
    // 根据角色属性,修改buff参数
    // ...


    // 创建Buff功能
    for(std::vector<BUFF_FUNC>::iterator iter = m_myBuffData->m_BuffInfo.buff_func.begin(); iter != m_myBuffData->m_BuffInfo.buff_func.end(); iter++)
    {
        const BUFF_FUNC& buffFunc = *iter;
        BuffFuncBase* pBuffFunc = CreateBuffFunc(buffFunc.buff_func_type);
        if(!pBuffFunc)
        {
            CnWarn("buff func type %d not exist\n", buffFunc.buff_func_type);
            continue;
        }

        pBuffFunc->SetBuff(this);
        pBuffFunc->SetArgs(buffFunc.arg1, buffFunc.arg2, buffFunc.buff_val_index);
        pBuffFunc->Start();

        m_BuffFuncs.push_back(pBuffFunc);
    }

    return true;
}

void Buff::Update(uint32 delta_time)
{
    m_myBuffData->m_BuffInfo.buff_remain_time -= delta_time;

//    CnDbg("Buff buffID: %u, funcType:%u\n", m_myBuffData->m_BuffId, m_myBuffData->m_FuncType);

    if(!m_myBuffData)
    {
        CnAssert(false);
        return;
    }

    for(std::vector<BuffFuncBase*>::iterator iter = m_BuffFuncs.begin(); iter != m_BuffFuncs.end(); iter++)
    {
        BuffFuncBase* pBuffFunc = *iter;
        pBuffFunc->Update(delta_time);       // buff功能
    }
}

void Buff::SetLifeTime(int32 lifeTime) 
{
    m_myBuffData->m_BuffInfo.buff_remain_time = m_myBuffData->m_BuffInfo.buff_time = lifeTime;
}

void Buff::SetRemainTime(int32 lifeTime) 
{
    m_myBuffData->m_BuffInfo.buff_remain_time = lifeTime;
}

int32 Buff::GetLifeTime() 
{
    return m_myBuffData->m_BuffInfo.buff_time; 
}

int32 Buff::GetRemainTime()
{
    return m_myBuffData->m_BuffInfo.buff_remain_time;
}

void Buff::ResetLiftTime(int32 lifeTime)
{
    SetLifeTime(lifeTime);
    for(std::vector<BuffFuncBase*>::iterator iter = m_BuffFuncs.begin(); iter != m_BuffFuncs.end(); iter++)
    {
        BuffFuncBase* pBuffFunc = *iter;
        pBuffFunc->ResetLifeTime(lifeTime);
    }
}

int32 Buff::GetBuffVal1()
{
    return m_myBuffData->m_BuffInfo.buff_val1;
}

void Buff::SetBuffVal1(int32 val)
{
    m_myBuffData->m_BuffInfo.buff_val1 = val;
}

int32 Buff::GetBuffVal2()
{
    return m_myBuffData->m_BuffInfo.buff_val2;
}

void Buff::SetBuffVal2(int32 val)
{
    m_myBuffData->m_BuffInfo.buff_val2 = val;
}

void Buff::OnUpdateValue()
{
    for(std::vector<BuffFuncBase*>::iterator iter = m_BuffFuncs.begin(); iter != m_BuffFuncs.end(); iter++)
    {
        BuffFuncBase* pBuffFunc = *iter;
        pBuffFunc->OnUpdateValue();
    }
}

BattleUnit* Buff::GetCaster()
{
    return GetOwner()->GetScene()->GetBattleUnit(m_CasterId);
}

bool Buff::CanDead()
{
    for(std::vector<BuffFuncBase*>::iterator iter = m_BuffFuncs.begin(); iter != m_BuffFuncs.end(); iter++)
    {
        BuffFuncBase* pBuffFunc = *iter;
        if(!pBuffFunc->CanDead())
            return false;
    }
    return true;
}

bool Buff::IsImmortal()
{
    for(std::vector<BuffFuncBase*>::iterator iter = m_BuffFuncs.begin(); iter != m_BuffFuncs.end(); iter++)
    {
        BuffFuncBase* pBuffFunc = *iter;
        if(pBuffFunc->GetFuncId() == BUFF_FUNC_IMMORTAL)
            return true;
    }
    return false;
}

bool Buff::IsShock()
{
    for(std::vector<BuffFuncBase*>::iterator iter = m_BuffFuncs.begin(); iter != m_BuffFuncs.end(); iter++)
    {
        BuffFuncBase* pBuffFunc = *iter;
        if(pBuffFunc->GetFuncId() == BUFF_FUNC_SHOCK)
            return true;
    }
    return false;
}

bool Buff::IsExpMultiple(int32* pMultiple)
{
    for(std::vector<BuffFuncBase*>::iterator iter = m_BuffFuncs.begin(); iter != m_BuffFuncs.end(); iter++)
    {
        BuffFuncBase* pBuffFunc = *iter;
        if(pBuffFunc->GetFuncId() == BUFF_FUNC_EXP_MUL)
        {
            if (pMultiple)
            {
                *pMultiple = pBuffFunc->GetBuffVal();
            }
            return true;
        }
    }
    return false;
}


bool Buff::IsDropQuality(int32* pMultiple)
{
    for(std::vector<BuffFuncBase*>::iterator iter = m_BuffFuncs.begin(); iter != m_BuffFuncs.end(); iter++)
    {
        BuffFuncBase* pBuffFunc = *iter;
        if(pBuffFunc->GetFuncId() == BUFF_FUNC_DROP_QUALITY)
        {
            if (pMultiple)
            {
                *pMultiple = pBuffFunc->GetBuffVal();
            }
            return true;
        }
    }
    return false;
}

DAMAGE_INFO Buff::FilterDamage(DAMAGE_INFO damage)
{
    for(std::vector<BuffFuncBase*>::iterator iter = m_BuffFuncs.begin(); iter != m_BuffFuncs.end(); iter++)
    {
        BuffFuncBase* pBuffFunc = *iter;
        damage = pBuffFunc->FilterDamage(damage);
    }
    return damage;
}

void Buff::SetEndTime( uint32 val )
{
    m_myBuffData->m_BuffInfo.end_time = val;
}

void Buff::AddOverlapNum()
{
    int32 maxOverlapNum = 0;
    const BUFF_ROOT_BUFF_STRUCT* buffConfig = SkillConfigManager::Instance()->GetBuffConfigData(GetId());
    if(buffConfig)
    {
        maxOverlapNum = buffConfig->overlap_num;
    }

    m_myBuffData->m_BuffInfo.overlap_num++;
    if(m_myBuffData->m_BuffInfo.overlap_num > maxOverlapNum)
        m_myBuffData->m_BuffInfo.overlap_num = maxOverlapNum;
}

int32 Buff::GetOverlapNum()
{
    return m_myBuffData->m_BuffInfo.overlap_num;
}
