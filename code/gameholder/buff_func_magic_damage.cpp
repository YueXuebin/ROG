#include "gameholder_pch.h"
#include "buff_func_magic_damage.h"
#include "creature.h"
#include "buff.h"
#include "buff_data.h"
#include "battle_manager.h"
#include "buff_controller.h"


//BuffFuncMagicDamage::BuffFuncMagicDamage() : 
//BuffFuncBase(BUFF_FUNC_MAGIC_DAMAGE)
//{
//    m_SecondTimer = 0;
//}
//
//BuffFuncMagicDamage::~BuffFuncMagicDamage()
//{
//
//}
//
//void BuffFuncMagicDamage::Start()
//{
//    m_SkillRuneType = GetArg1();     // arg1伤害类型
//    m_SecondTimer = GetArg2();       // arg2伤害周期
//}
//
//void BuffFuncMagicDamage::Update(uint32 delta_time)
//{
//    if(!m_pBuff->GetOwner())
//    {
//        CnAssert(false);
//        return;
//    }
//
//    m_SecondTimer -= delta_time;
//    if(m_SecondTimer <= 0)
//    {
//        m_SecondTimer += GetArg2();
//
//        int32 damagePerTimer = int32(m_pBuff->GetBuffVal() * ((real)GetArg2()/1000));     // 每秒平均伤害
//
//        int32 buffDamagePerTimer = BattleManager::Instance()->BuffAttack(damagePerTimer, m_SkillRuneType, m_pBuff->GetOwner());
//
//        DAMAGE_INFO damageInfo;
//        damageInfo.magic_damage = buffDamagePerTimer;
//        damageInfo.type = 0;
//
//        GetOwnerBuffController()->AddBuffUpdateDamage(damageInfo, m_pBuff->GetCasterID());
//    }
//}
