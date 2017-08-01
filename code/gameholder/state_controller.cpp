#include "gameholder_pch.h"
#include "performance_analysis.h"
#include "state_controller.h"
#include "battle_unit.h"
#include "state_define.h"
#include "state_idle.h"
#include "state_walk.h"
#include "state_run.h"
#include "state_attack.h"
#include "state_die.h"
#include "state_stun.h"
#include "state_singing.h"
#include "state_movie.h"
#include "scene.h"
#include "state_dance.h"
#include "state_touch.h"


StateController::StateController(BattleUnit* owner) :
m_Owner(owner)
{
    m_States[STATE_IDLE]        = CnNew StateIdle(owner);
    m_States[STATE_WALK]        = CnNew StateWalk(owner);
    m_States[STATE_RUN]         = CnNew StateRun(owner);
    m_States[STATE_STUN]        = CnNew StateStun(owner);
    m_States[STATE_DIE]         = CnNew StateDie(owner);
    m_States[STATE_ATTACK]      = CnNew StateAttack(owner);
    m_States[STATE_MOVIE]       = CnNew StateMovie(owner);
    m_States[STATE_DANCE]       = CnNew StateDance(owner);
    m_States[STATE_TOUCH]       = CnNew StateTouch(owner);

    m_CurState = m_States[STATE_IDLE];      // 初始为idle状态
}

StateController::~StateController()
{
    for(StateMap::iterator iter=m_States.begin(); iter != m_States.end(); iter++)
    {
        CnDelete iter->second;
    }
}

void StateController::FillStateInfo(STATE_INFO& info) const
{
    info.is_dead_state = (GetState() == STATE_DIE)?1:0;
}

void StateController::LoadStateInfo(const STATE_INFO& info)
{
    if(info.is_dead_state)
    {
        m_CurState = m_States[STATE_DIE];       // 跳副本时为死亡状态
    }
}

void StateController::SetStateToState( uint32 state, uint32 next_state )
{
    if(m_States.find(state) == m_States.end() || m_States.find(next_state) == m_States.end())
    {
        CnAssert(false);
        return;
    }

    m_States[state]->SetNextState(next_state);
}

void StateController::SetCanCastBreak(bool canBreak)
{
    ((StateAttack*)m_States[STATE_ATTACK])->SetCanBreak(canBreak);
}

bool StateController::GetCanCastBreak()
{
    return ((StateAttack*)m_States[STATE_ATTACK])->GetCanBreak();
}

void StateController::SetMovieName(const std::string& movieName)
{
    ((StateMovie*)m_States[STATE_MOVIE])->SetMovieName(movieName);
}

const std::string& StateController::GetMovieName()
{
    return ((StateMovie*)m_States[STATE_MOVIE])->GetMovieName();
}

void StateController::SetMovieAttackAble(bool attackable)
{
    ((StateMovie*)m_States[STATE_MOVIE])->SetAttackable(attackable);
}

uint32  StateController::GetState() const
{
    return m_CurState->GetType();
}

bool StateController::CanChangeTo(uint32 state)
{
    if(m_States.find(state) == m_States.end())
        return false;

    return m_CurState->CanChangeTo(state);
}

void StateController::ChangeState( uint32 state )
{
    CnAssert(m_States.find(state) != m_States.end());
    CnAssert(CanChangeTo(state));       // 逻辑检错

    _ChangeState(state);
}

void StateController::Resurgence()
{
    CnAssert(m_CurState->GetType() == STATE_DIE);

    _ChangeState(STATE_IDLE);
}

void StateController::_ChangeState(uint32 state)
{
    uint32 oldState = m_CurState->GetType();

    if(m_CurState->GetType() == state)
        return;

    m_CurState->onLeaveState();
    m_CurState = m_States[state];
    m_CurState->onEnterState();

    // 状态改变回调
    m_Owner->OnStateChange(oldState, state);
    // 状态改变通知
    STATE_NTF ntf;
    ntf.object_id = m_Owner->GetId();
    ntf.state = state;
    if(state == STATE_MOVIE)
    {
        ntf.state_arg = GetMovieName();
    }

    if(m_Owner->GetScene())
    {
        m_Owner->GetScene()->NotifyMessage(m_Owner, MSG_STATE_NTF, &ntf);
    }
}

void StateController::Update()
{
    if (!m_Owner->GetScene())
    {
        return;
    }

    PERFORM_HELPER;
    m_CurState->Update();       // ouyang: update first for add time before judge end

    if(m_CurState->isEnd())
    {   // 切换状态
        if(m_CurState->GetNextState() != STATE_NONE)
        {   // 有指定下一个状态
            uint32 NextStateId = m_CurState->GetNextState();
            m_CurState->SetNextState(STATE_NONE);
            _ChangeState(NextStateId);
        }
        else
        {   // 无指定下一个状态进入IDLE
            _ChangeState(STATE_IDLE);
        }
    }
}

void StateController::SetAttackTime( int32 time )
{
    ((StateAttack*)m_States[STATE_ATTACK])->SetEndTime(time);
}

void StateController::SetMovieTime(int32 time)
{
    ((StateMovie*)m_States[STATE_MOVIE])->SetEndTime(time);
}

void StateController::SetStunTime(int32 time)
{
    ((StateStun*)m_States[STATE_STUN])->SetEndTime(time);
}

void StateController::SetDeadTime(int32 time)
{
    ((StateDie*)m_States[STATE_DIE])->SetDeadTime(time);
}

void StateController::SetSkill( uint32 skill_id, uint32 object_id, int32 x, int32 y )
{

}

bool StateController::CanChangePostion()
{
    uint32 type = m_CurState->GetType();
    return (type == STATE_IDLE) || (type == STATE_WALK) || (type == STATE_RUN) || (type == STATE_ATTACK && GetCanCastBreak());      // 可打断cast才能够移动
}

bool StateController::CanUseSkill()
{
    // 眩晕，死亡，被击 状态不可以使用技能
    uint32 type = m_CurState->GetType();
    if(m_Owner->GetObjectType() == OBJ_PLAYER)
        return (type == STATE_IDLE) || (type == STATE_WALK) || (type == STATE_RUN) || (type == STATE_TOUCH) || (type == STATE_ATTACK && GetCanCastBreak());    // 玩家可以技能中断技能
    else
        return (type == STATE_IDLE) || (type == STATE_WALK) || (type == STATE_RUN);
}

bool StateController::GetAttackable()
{
    uint32 type = m_CurState->GetType();
    if(type == STATE_MOVIE)
    {
        return ((StateMovie*)m_States[STATE_MOVIE])->GetAttackable();
    }
    else
        return true;
}

int32 StateController::GetRemainTime()
{
    return m_CurState->GetRemainTime();
}


