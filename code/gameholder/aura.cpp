#include "gameholder_pch.h"
#include "aura.h"
#include "battle_unit.h"
#include "scene.h"
#include "buff_data.h"
#include "buff_controller.h"
#include "game_define.h"

const int32 SEARCH_PERIOD = 1000;

Aura::Aura( const BuffData* pData, uint32 CasterId, int32 buffVal1, int32 buffVal2 ) :
Buff(pData, CasterId, buffVal1, buffVal2),
m_SecondTimer(0)
{
    CnAssert(m_myBuffData->m_BuffInfo.is_aura);
}

Aura::~Aura()
{
}

void Aura::Update(uint32 delta_time)
{
    Buff::Update(delta_time);

    m_SecondTimer -= delta_time;
    if(m_SecondTimer <= 0)
    {
        UpdateSearch();
        m_SecondTimer += SEARCH_PERIOD;      // 每一秒做一次范围对象的搜索
    }
}

void Aura::UpdateSearch()
{
    if (!m_Owner->GetScene())
    {
        return;
    }

    // 搜索半径范围内的对象
    BattleUnit*  uints[MAX_AURA_MEMBER];
    int32 searchCount = m_Owner->GetScene()->FindBattleUnitInCircle(m_Owner->GetPosition(), m_myBuffData->m_BuffInfo.aura_radius, uints, MAX_AURA_MEMBER);

    // 给范围内的对象上一个持续时间正好为一次搜索周期的buff
    for(int32 i=0;i<searchCount;i++)
    {
        BattleUnit* target = uints[i];
        CnAssert(target);
        if(!m_Owner->CanBuff(target))
            continue;

        bool can = false;

        if(m_myBuffData->m_BuffInfo.aura_self && (m_Owner == target))
            can = true;
        else if(m_myBuffData->m_BuffInfo.aura_enemy && m_Owner->IsEnemy(target))
            can = true;
        else if(m_myBuffData->m_BuffInfo.aura_friend && !m_Owner->IsEnemy(target))
            can = true;

        if(can && m_Owner->CanBuff(target))
           target->AddBuff(m_myBuffData->m_BuffInfo.aura_buff_id, SEARCH_PERIOD*2, m_Owner, GetBuffVal1(), GetBuffVal2());
    }
}
