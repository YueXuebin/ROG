#include "gameholder_pch.h"
#include "push_mover.h"
#include "creature.h"
#include "action_controller.h"
#include "box2d_define.h"



PushMover::PushMover(Creature* creature) :
m_Creature(creature)
{
    m_IsPushing = false;

    m_StartX = m_DesX = 0;
    m_StartY = m_DesY = 0;
    m_PushSpeed.SetZero();
}

PushMover::~PushMover()
{

}

void PushMover::PushTo(int32 x, int32 y, int32 time, bool isPassive)
{
    if(m_IsPushing)
    {
        m_IsPushing = false;

        m_Creature->SetPhysicsBullet(false);
        m_Creature->SetPhysicsMask(m_OldMask);
    }

    CnAssert(!m_IsPushing);
    m_IsPushing = true;

    m_IsPassivePush = isPassive;
    m_PushTime = time;
    m_PushUseTime = 0;

    m_StartX = m_Creature->GetX();
    m_StartY = m_Creature->GetY();
    m_DesX = (int32)x;
    m_DesY = (int32)y;

    m_PushLine.Set(real(m_DesX - m_StartX), real(m_DesY - m_StartY));

    // 计算移动的距离和速度
    m_PushSpeed = m_PushLine * (1000.0f / time);

    m_Creature->SetPhysicsVelocity(m_PushSpeed);
    m_OldMask = m_Creature->GetPhysicsMask();
    m_Creature->SetPhysicsMask(m_OldMask & ~PHY_CATEGORY_CREATURE);     // 不与生物碰撞
    m_Creature->SetPhysicsBullet(true);
}

void PushMover::Stop()
{
    CnAssert(m_IsPushing);
    m_IsPushing = false;

    m_Creature->SetPhysicsBullet(false);
    m_Creature->SetPhysicsMask(m_OldMask);

    m_Creature->GetActionController()->OnMoveEnd(MOVE_TYPE_PUSH);
}

void PushMover::UpdatePush()
{
    m_PushUseTime += g_ServerUpdateTime;

    real k = ProjectOnLine(m_Creature->GetX(), m_Creature->GetY());
    if(k >= 1.0f)
    {   // 到达目的地
        m_Creature->SetPosition(m_DesX, m_DesY);
        Stop();
        return;
    }

    if(m_PushUseTime >= m_PushTime)
    {   // 推移时间
        m_Creature->SetPosition(m_DesX, m_DesY);
        Stop();
        return;
    }
}

real PushMover::ProjectOnLine(int32 x, int32 y)
{
    CoVec2 dir(m_PushLine);
    dir.Normalize();

    CoVec2 p31(x - m_StartX, y - m_StartY);

    real len = Vec2Dot(p31, dir);

    return len / m_PushLine.Length();
}