#include "gameholder_pch.h"
#include "buff_func_absorb_damage.h"
#include "buff.h"

//
//BuffFuncAbsorbDamage::BuffFuncAbsorbDamage() :
//BuffFuncBase(BUFF_FUNC_ABSORB_DAMAGE)
//{
//
//}
//
//BuffFuncAbsorbDamage::~BuffFuncAbsorbDamage()
//{
//
//}
//
//void BuffFuncAbsorbDamage::Start()
//{
//    m_RemainValue = m_AbsorbValue = m_pBuff->GetBuffVal1();
//}
//
//DAMAGE_INFO BuffFuncAbsorbDamage::FilterDamage( DAMAGE_INFO damage )
//{
//    int32 allDamage = damage.physics_damage + damage.magic_damage;
//    DAMAGE_INFO resultDamage;
//
//    if(m_RemainValue >= allDamage)
//    {
//        m_RemainValue -= allDamage;
//    }
//    else
//    {
//        m_RemainValue = 0;
//        
//        resultDamage.magic_damage -= m_RemainValue;     // 先吸收法术伤害
//        if(resultDamage.magic_damage < 0)               // 再吸收物理伤害
//        {
//            resultDamage.physics_damage += resultDamage.magic_damage;
//            resultDamage.magic_damage = 0;
//            resultDamage.physics_damage = Crown::SDMax(resultDamage.physics_damage, 0);
//        }
//    }
//
//    if(m_RemainValue <= 0)
//    {   // Buff结束
//        m_pBuff->SetLifeTime(0);
//    }
//
//    return resultDamage;
//}
