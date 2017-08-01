#include "gameholder_pch.h"
#include "skill_utils.h"
#include "skill_define.h"
#include "skill_bullet_trajectory_direct.h"
#include "skill_logic_bullet.h"
#include "battle_unit.h"
#include "game_util.h"


SkillBulletTrajectory_Direct::SkillBulletTrajectory_Direct()
{
    m_PassTime = 0;
}

SkillBulletTrajectory_Direct::~SkillBulletTrajectory_Direct()
{

}

void SkillBulletTrajectory_Direct::Init(SkillLogicBullet* pBullet)
{
    m_Bullet = pBullet;
    m_BulletData = &m_Bullet->m_SkillData->m_BulletData;
    CnAssert(m_BulletData->m_BulletMode == SKILL_BULLET_MODE_DIRECT);

    const SHOOT_TARGET& target = m_Bullet->m_BulletTarget;
    m_BulletEndPostion.Set((real)target.target_x, (real)target.target_y);

    BattleUnit* Owner = m_Bullet->m_Skill->GetOwner();
    // 子弹起始位置
    m_BulletStartPosition = Owner->GetPosition();
    m_BulletPosition = m_BulletStartPosition;

    // 根据距离/速度，计算弹道时间
    if(m_BulletData->m_ShootTime > 0)
    {
        m_ShootTime = m_BulletData->m_ShootTime;
    }
    else if(m_BulletData->m_ShootSpeed > 0)
    {
        int32 dis = (int32)sqrt((real)(m_BulletStartPosition.x - m_BulletEndPostion.x)*(m_BulletStartPosition.x - m_BulletEndPostion.x) + (m_BulletStartPosition.y - m_BulletEndPostion.y)*(m_BulletStartPosition.y - m_BulletEndPostion.y));
        m_ShootTime = (int32)( (real)dis / (m_BulletData->m_ShootSpeed * ToPercent(m_Bullet->m_SkillArgs->m_BulletSpeed)) * 1000 );
    }
    else
    {
        m_ShootTime = 0;        // 瞬发
        m_BulletPosition = m_BulletEndPostion;
    }

    // 弹道方向
    CoVec2 dir(m_BulletEndPostion.x - m_BulletStartPosition.x, m_BulletEndPostion.y - m_BulletStartPosition.y);
    if(dir.LengthSquared() == 0)
        m_BulletAngle = m_Bullet->GetSkill()->GetOwner()->GetAngle();            // 起始结束位置相同，则取角色方向
    else
        m_BulletAngle = RADIAN_TO_ANGLE(dir.GetAngle());

}

void SkillBulletTrajectory_Direct::Update()
{
    if(m_PassTime >= m_ShootTime)
    {
        ShootEnd();
        return;
    }
    
    UpdatePosition();

    m_PassTime += g_ServerUpdateTime;
}

void SkillBulletTrajectory_Direct::UpdatePosition()
{
    // 计算子弹位置
    if(m_BulletData->m_BulletTargetData.m_TargetType == SKILL_TARGET_OBJECT || m_BulletData->m_BulletTargetData.m_TargetType == SKILL_TARGET_SELF)        // 子弹目标为角色
    {
        m_BulletEndPostion = SkillUtils::GetTargetPosition(m_Bullet->m_Skill, m_Bullet->m_BulletTarget);      // 对目标需要重新计算位置
    }
    else if(m_BulletData->m_BulletTargetData.m_TargetType == SKILL_TARGET_POS)      // 子弹目标为位置
    {
        m_BulletEndPostion.x = real(m_Bullet->m_BulletTarget.target_x);
        m_BulletEndPostion.y = real(m_Bullet->m_BulletTarget.target_y);
    }
    else
        CnAssert(false);

    real s = (real)g_ServerUpdateTime / (m_ShootTime - m_PassTime);
    s = Crown::SDMin(s, 1.0f);

    CoVec2 dir = m_BulletEndPostion - m_BulletPosition;
    dir *= s;
    m_BulletPosition = m_BulletPosition + dir;
}

void SkillBulletTrajectory_Direct::ShootEnd()
{
    SHOOT_TARGET hitTarget;
    hitTarget.caster_id = m_Bullet->m_BulletTarget.caster_id;

    if(m_BulletData->m_BulletTargetData.m_TargetType == SKILL_TARGET_OBJECT)
    {
        hitTarget.target_id = m_Bullet->m_BulletTarget.target_id;
    }
    else if(m_BulletData->m_BulletTargetData.m_TargetType == SKILL_TARGET_POS)
    {
        hitTarget.target_id = 0;
    }
    else if(m_BulletData->m_BulletTargetData.m_TargetType == SKILL_TARGET_SELF)
    {
        hitTarget.target_id = m_Bullet->m_BulletTarget.target_id;
    }
    else
        CnAssert(false);

    hitTarget.target_x = int32(m_BulletEndPostion.x);
    hitTarget.target_y = int32(m_BulletEndPostion.y);

    HitArgs hitArgs;
    hitArgs.m_HitAngle = m_BulletAngle;

    m_Bullet->OnBulletHit(hitTarget, hitArgs);

    m_Bullet->OnBulletEnd();
}

