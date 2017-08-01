#include "gameholder_pch.h"
#include "buff_func_damage_scale.h"
#include "buff_data.h"
#include "buff_define.h"
#include "buff.h"
#include "battle_unit.h"
#include "buff_controller.h"


//
//BuffFuncDamageScale::BuffFuncDamageScale() :
//BuffFuncBase(BUFF_FUNC_DAMAGE_SCALE)
//{
//    m_SecondTimer = 0;
//}
//
//BuffFuncDamageScale::~BuffFuncDamageScale()
//{
//
//}
//
//void BuffFuncDamageScale::Start()
//{
//    m_SecondTimer = GetArg2();
//}
//
//void BuffFuncDamageScale::Update(uint32 delta_time)
//{
//    m_SecondTimer -= delta_time;
//    if(m_SecondTimer <= 0)
//    {
//        m_SecondTimer += int32(GetArg2() * ((real)GetArg2()/1000));
//
//        BattleUnit* pOwner = m_pBuff->GetOwner();
//
//        int32 maxhp = pOwner->GetMaxHp();
//        int32 changehp = int32( (real)m_pBuff->GetBuffVal()/10000 * maxhp );
//        
//        if(changehp < 0)
//        {
//            DAMAGE_INFO damageInfo;
//            damageInfo.physics_damage = -changehp;
//            damageInfo.type = 0;
//
//            GetOwnerBuffController()->AddBuffUpdateDamage(damageInfo, m_pBuff->GetCasterID());
//        }
//        else if(changehp > 0)
//        {
//            GetOwnerBuffController()->AddBuffUpdateHeal(changehp, m_pBuff->GetCasterID());
//        }
//    }
//}
