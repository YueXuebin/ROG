#include "gameholder_pch.h"
#include "step_mover.h"
#include "creature.h"
#include "state_define.h"
#include "scene.h"
#include "action_controller.h"
#include "path_mover.h"



StepMover::StepMover(Creature* creature) :
m_Creature(creature)
{

}

StepMover::~StepMover()
{

}

void StepMover::Init(PathMover* pathMover)
{
    m_PathMover = pathMover;

    m_IsMoving = false;

    m_StartX = m_DesX = m_Creature->GetX();
    m_StartY = m_DesY = m_Creature->GetY();

}

real StepMover::RemainDistance()
{
    real k = ProjectOnLine(m_Creature->GetX(), m_Creature->GetY());
    if(k < 1.0f)
    {
        return (1.0f - k) * m_Line.Length();
    }
    else
        return 0.0f;
}

real StepMover::ProjectOnLine(int32 x, int32 y)
{
    CoVec2 dir(m_Line);
    dir.Normalize();

    CoVec2 p31(x - m_StartX, y - m_StartY);
    
    real len = Vec2Dot(p31, dir);

    return len / m_Line.Length();
}

void StepMover::UpdateMove()
{
    if(!m_IsMoving)
        return;

    // 判断当前位置是否已经移动完成
    

    // 判断结束(非盘绕状态下判断)
    if(m_TwineAngle == 0)
    {
        real k = ProjectOnLine(m_Creature->GetX(), m_Creature->GetY());
        if(k>=1)
        {
            // 到达目的地
            m_IsMoving = false;
            m_DesX = m_Creature->GetX();
            m_DesY = m_Creature->GetY();
            m_PathMover->OnStepMoveEnd();
            return;
        }
    }
    else    // (盘绕状态下判断)
    {

    }
    
   
    
    // 计算移动速度
    real moveSpeed = (real)m_Creature->GetMoveSpeed();
    switch(m_Creature->GetState())
    {
    case STATE_RUN:
        break;
    case STATE_WALK:
        {
            moveSpeed *= 0.4f;
        }
        break;
    default:
        CnAssert(false);
        break;
    }
    
    // 目标点方向
    CoVec2 desDir((real)(m_DesX-m_Creature->GetX()), (real)(m_DesY-m_Creature->GetY()));
    desDir.Normalize();

    // 真实速度
    CoVec2 finalVelocity = m_Creature->GetFinalPhysicsVelocity();
    real finalSpeed = finalVelocity.Length();

    // 真实速度小于n%时,认为被阻挡,进入或增加盘绕
    if(finalSpeed < moveSpeed*0.5f)
    {
        m_TwineCounter++;
        if(m_TwineCounter >= 5)
        {
            // 计算真实速度和目标方向的夹角
            real th = Vec2Dot(desDir, finalVelocity) / finalSpeed;
            real ar = acos(th);
            int32 a = RADIAN_TO_ANGLE(ar);

            if(Vec2Cross(desDir, finalVelocity) < 0)
                a = -a;

            if(m_TwineAngle > 0)
            {
                m_TwineAngle += DELTA_ANGLE;
                if(m_TwineAngle > MAX_TWINE_ANGLE)          
                    m_TwineAngle = -m_TwineAngle;
            }
            else if(m_TwineAngle < 0)
            {
                m_TwineAngle -= DELTA_ANGLE;
                if(m_TwineAngle < -MAX_TWINE_ANGLE)
                    m_TwineAngle = -m_TwineAngle;
            }
            else
            {
                // 进入盘绕
                real dis = Vec2Distance(m_Creature->GetPosition(), CoVec2(m_DesX, m_DesY));     // 计算离目标点距离
                if(dis >= m_TwineLeaveDistance)      // 如果已经无法再离目标更近,则结束
                {   
                    m_IsMoving = false;
                    m_DesX = m_Creature->GetX();
                    m_DesY = m_Creature->GetY();
                    m_PathMover->OnStepMoveEnd();
                    return;
                }
                else
                {
                    m_TwineLeaveDistance = dis;
                }

                if(a > 0)
                    m_TwineAngle = DELTA_ANGLE;
                else
                    m_TwineAngle = -DELTA_ANGLE;
            }
            
            m_TwineCounter = 0;
        }
    }
    else
    {
        if(m_TwineAngle != 0)
        {
            m_TwineCounter++;
            if(m_TwineCounter >= 10)
            {   
                if(m_TwineAngle > 0)
                    m_TwineAngle -= DELTA_ANGLE;
                else if(m_TwineAngle < 0)
                    m_TwineAngle += DELTA_ANGLE;

                if(m_TwineAngle == 0)
                {
                    // 结束盘绕
                    real dis = m_TwineLeaveDistance;        // 记录盘绕最近距离
                    MoveTo(m_DesX, m_DesY);         // 重新朝目标点移动
                    m_TwineLeaveDistance = dis;
                }
            }
        }
    }

    // 计算移动方向
    if(m_TwineAngle == 0)
    {   // 朝目标点移动
        m_TwineStartAngle = RADIAN_TO_ANGLE(desDir.GetAngle());
        m_Creature->SetAngle(m_TwineStartAngle);
        m_Creature->SetPhysicsVelocity(desDir * moveSpeed);
    }
    else
    {   // 盘绕移动
        int32 moveAngle = m_TwineStartAngle;
        moveAngle += m_TwineAngle;
        desDir.SetAngle(moveSpeed, ANGLE_TO_RADIAN(moveAngle));
        
        m_Creature->SetAngle(moveAngle);
        m_Creature->SetPhysicsVelocity(desDir);
    }

}

void StepMover::MoveTo(int32 x, int32 y)
{
    m_StartX = m_Creature->GetX();
    m_StartY = m_Creature->GetY();
    m_DesX = x;
    m_DesY = y;

    m_Line.Set(real(m_DesX - m_StartX), real(m_DesY - m_StartY));

    m_IsMoving = true;

    m_TwineAngle = 0;
    m_TwineCounter = 0;
    m_TwineLeaveDistance = 999999;
}

void StepMover::Stop()
{
    if(!m_IsMoving)
        return;

    m_StartX = m_DesX = m_Creature->GetX();
    m_StartY = m_DesY = m_Creature->GetY();
    
    m_Line.SetZero();
    
    m_IsMoving = false;

    m_PathMover->OnStepMoveEnd();
}

