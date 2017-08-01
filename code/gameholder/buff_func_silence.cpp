#include "gameholder_pch.h"
#include "buff_func_silence.h"
#include "state_define.h"
#include "buff.h"
#include "battle_unit.h"
#include "skill_controller.h"

//BuffFuncSilence::BuffFuncSilence() :
//BuffFuncBase(BUFF_FUNC_SILENCE)
//{
//
//}
//
//BuffFuncSilence::~BuffFuncSilence()
//{
//
//}
//
//void BuffFuncSilence::Start()
//{
//    BattleUnit* pOwner = m_pBuff->GetOwner();
//
//    int32 silenceTime = pOwner->GetSkillController()->GetSilence();
//    // 前一沉默时间长于新沉默时间 
//    if(silenceTime >= m_pBuff->GetLifeTime())
//    {
//        return;
//    }
//    else
//    {
//        pOwner->GetSkillController()->SetSilence(m_pBuff->GetLifeTime());
//    }
//}
