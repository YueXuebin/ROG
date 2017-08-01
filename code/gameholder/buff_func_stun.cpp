#include "gameholder_pch.h"
#include "buff_define.h"
#include "buff_func_stun.h"
#include "state_define.h"
#include "buff.h"
#include "battle_unit.h"
#include "state_controller.h"

BuffFuncStun::BuffFuncStun() :
BuffFuncBase(BUFF_FUNC_STUN)
{

}

BuffFuncStun::~BuffFuncStun()
{

}

void BuffFuncStun::Start()
{
    EnterStun(m_pBuff->GetLifeTime());
}

void BuffFuncStun::ResetLifeTime(int32 time)
{
    EnterStun(time);
}

void BuffFuncStun::EnterStun(int32 time)
{
    if(m_pBuff->GetOwner()->GetState() == STATE_STUN)
    {
        m_pBuff->GetOwner()->GetStateController()->SetStunTime(time);
    }
    else
    {   // 进入眩晕状态
        if(m_pBuff->GetOwner()->CanChangeTo(STATE_STUN))
        {
            m_pBuff->GetOwner()->GetStateController()->SetStunTime(time);
            m_pBuff->GetOwner()->ChangeState(STATE_STUN);       // 应该只有该buff功能会触发进入眩晕状态
        }
    }
}
