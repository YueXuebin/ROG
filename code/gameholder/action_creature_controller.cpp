#include "gameholder_pch.h"
#include "action_creature_controller.h"
#include "creature.h"
#include "state_define.h"
#include "scene.h"
#include "game_define.h"
#include "skill_define.h"
#include "aoi_manager.h"
#include "path_mover.h"
#include "push_mover.h"
#include "performance_analysis.h"
#include "state_controller.h"


ActionCreatureController::ActionCreatureController(Creature* creature) :
ActionController(creature),
m_Creature(creature)
{
    m_PathMover = CnNew PathMover(m_Creature);
    m_PushMover = CnNew PushMover(m_Creature);

    m_isMoving = false;
    m_IsPushing = false;

    m_SendAngle = 0;
    m_SendPosition.SetZero();
    m_SendVelocity.SetZero();

    m_SendTimer = 0;

}

ActionCreatureController::~ActionCreatureController()
{
    CnDelete m_PathMover;
    m_PathMover = NULL;

    CnDelete m_PushMover;
    m_PushMover = NULL;
}

const std::vector<CoVec2>& ActionCreatureController::GetPath()
{
    return m_PathMover->GetPath();
}

bool ActionCreatureController::Bounce(int32 height, int32 time)
{
    if(m_Creature->GetCantMove())
        return true;

    m_Creature->Bounce(height, time, false);
    return true;
}

bool ActionCreatureController::AngleTo( int32 angle )
{
    if(m_Creature->GetCantAngle())
        return true;

    if(!m_Creature->GetStateController()->CanChangePostion())
        return false;

    m_Creature->SetAngle(angle);
    return true;
}

void ActionCreatureController::AngleToObject(SceneObject* pObject)
{
    CoVec2 dir(pObject->GetX() - m_Creature->GetX(), pObject->GetY() - m_Creature->GetY());
    AngleTo(RADIAN_TO_ANGLE(dir.GetAngle()));
}

void ActionCreatureController::Stop()
{
    if(m_Creature->GetCantMove())
        return;

    if(!m_Creature->CanChangeTo(STATE_IDLE))
        return;
    m_Creature->ChangeState(STATE_IDLE);

    m_PathMover->Stop();
    
    return;
}

bool ActionCreatureController::JumpTo( int32 x, int32 y)
{
    if(m_Creature->GetCantMove())
        return false;

    m_Creature->SetPosition(x, y);

    // jump通知
    JUMP_NTF ntf;
    ntf.object_id = m_Creature->GetId();
    ntf.x = x;
    ntf.y = y;
    ntf.time = 0;
    ntf.is_stop = false;
    ntf.is_passive = true;
    m_Creature->GetScene()->NotifyMessage(m_Creature, MSG_JUMP_NTF, &ntf, false);

    return true;
}

// 推动角色到某位置
bool ActionCreatureController::PushTo(int32 x, int32 y, int32 time, bool isPassive)
{
    CnAssert(time > 0);     // time==0,为JumpTo

    if(m_Creature->GetCantMove())
        return false;

    m_IsPushing = true;
    m_PushMover->PushTo(x, y, time, isPassive);

    // jump通知
    JUMP_NTF ntf;
    ntf.object_id = m_Creature->GetId();
    ntf.x = x;
    ntf.y = y;
    ntf.time = time;
    ntf.is_stop = false;
    ntf.is_passive = isPassive;
    m_Creature->GetScene()->NotifyMessage(m_Creature, MSG_JUMP_NTF, &ntf, false);

    // 如果是被动推移，则认为角色进入眩晕状态
    if(false)       // 击退不晕
    {
        if(m_Creature->GetState() != STATE_STUN)
        {
            if(m_Creature->CanChangeTo(STATE_STUN))
            {
                m_Creature->ChangeState(STATE_STUN);
                m_Creature->GetStateController()->SetStunTime(time);
            }
        }
        else
        {
            if(m_Creature->GetStateController()->GetRemainTime() < time)
                m_Creature->GetStateController()->SetStunTime(time);
        }
    }

    return true;
}

void ActionCreatureController::WalkTo( int32 x, int32 y )
{
    if(m_Creature->GetCantMove())
        return;

    if(!m_Creature->CanChangeTo(STATE_WALK))
        return;

    if(m_Creature->GetX() == x && m_Creature->GetY() == y)  // 位置相同，无需移动
        return;

    m_Creature->ChangeState(STATE_WALK);

    m_PathMover->MoveTo(x, y);                    // 计算要走的下一个目标点
    m_isMoving = true;
}

void ActionCreatureController::RunTo( int32 x, int32 y )
{
    if(m_Creature->GetCantMove())
        return;

    if(!m_Creature->CanChangeTo(STATE_RUN))
        return;

    if(m_Creature->GetX() == x && m_Creature->GetY() == y)  // 位置相同，无需移动
        return;

    m_Creature->ChangeState(STATE_RUN);

    m_PathMover->MoveTo(x, y);                        // 计算要走的下一个目标点
    m_isMoving = true;
}

void ActionCreatureController::OnStateChange( uint32 state )
{
    if(m_isMoving)
    {
        if(state != STATE_WALK || state != STATE_RUN)
        {
            m_PathMover->Stop();        // 非移动状态，移动停止
        }
    }

    if(m_IsPushing && !m_PushMover->IsPassivePush())     // 主动推移会被打断
    {
        if(IsServerState(state))
        {
            m_PushMover->Stop();
        }
    }

}

bool ActionCreatureController::UseSkill(uint32 skillId, const SHOOT_TARGET& target, const SkillArgumentData& args, uint32 skillObjID, uint32 skillSlot)
{
    if(!m_Creature->CanUseSkill())
        return false;

    // 面向目标
    SceneObject* pTargetObj = m_Creature->GetScene()->GetSceneObject(target.target_id);
    if(pTargetObj)
        AngleToObject(pTargetObj);

    bool skillOk = m_Creature->UseSkill(skillId, target, args, skillObjID, skillSlot);

    return skillOk;
}

void ActionCreatureController::Update()
{
    if (!m_Creature->GetScene())
    {
        return;
    }

    PERFORM_HELPER;
    if(m_IsPushing)
    {   // push状态(无碰撞)
        UpdatePush();
    }
    else if(m_isMoving)
    {   // 移动状态(有碰撞)
        UpdateMove();
    }

    SendMoveUpdate();
}

void ActionCreatureController::UpdatePush()
{
    m_PushMover->UpdatePush();
}

void ActionCreatureController::UpdateMove()
{
    m_PathMover->UpdateMove();
}

void ActionCreatureController::OnMoveEnd(uint32 moveType)
{
    if(moveType == MOVE_TYPE_MOVE)
    {   // 移动结束
        CnAssert(m_isMoving);
        m_isMoving = false;
        
        if(m_Creature->CanChangeTo(STATE_IDLE))
            m_Creature->ChangeState(STATE_IDLE);
    }
    else if(moveType == MOVE_TYPE_PUSH)
    {   // 推移结束
        CnAssert(m_IsPushing);
        m_IsPushing = false;
        // 发送推移结束通知
        JUMP_NTF ntf;
        ntf.object_id = m_Creature->GetId();
        ntf.x = m_Creature->GetX();
        ntf.y = m_Creature->GetY();
        ntf.is_stop = true;
        m_Creature->GetScene()->NotifyMessage(m_Creature, MSG_JUMP_NTF, &ntf, false);
    }

    m_Creature->SetPhysicsVelocity(CoVec2(0, 0));
}

void ActionCreatureController::SendMoveUpdate()
{
    bool needSend = false;

    // 判断是否为移动状态
    bool isMoving = !m_Creature->GetPhysicsVelocity().IsZero();

    CoVec2 position = m_Creature->GetPosition();
    CoVec2 velocity = m_Creature->GetFinalPhysicsVelocity();

    if(isMoving)
    {
        m_SendTimer -= g_ServerUpdateTime;
        if(m_SendTimer <= 0)
            needSend = true;
    }
    else
    {
        m_SendTimer = 0;
        // 判断位置是否相同
        if(Vec2DistanceSquared(m_SendPosition, position) > 10)
            needSend = true;
    }

    // 判断速度是否相同
    if(Vec2DistanceSquared(m_SendVelocity, velocity) > 10)
        needSend = true;
    
    // 判断方向是否相同
    if(fabs((real)m_SendAngle - m_Creature->GetAngle()) > 5.0f)
        needSend = true;

    if(!needSend)
        return;

    // 发送移动通知包
    m_SendPosition = position;
    m_SendVelocity = velocity;
    m_SendAngle = m_Creature->GetAngle();
    m_SendTimer = MOVE_FORSEE_TIME;      // 匀速运动通知时间间隔
    m_Creature->GetScene()->NotifyMove(m_Creature->GetId(), m_SendAngle, position, velocity, m_isMoving);

}

bool ActionCreatureController::Touch()
{
    // 采集状态转换
    if(!m_Creature->CanChangeTo(STATE_TOUCH))
    {
        return false;
    }

    m_Creature->ChangeState(STATE_TOUCH);
    return true;
}




