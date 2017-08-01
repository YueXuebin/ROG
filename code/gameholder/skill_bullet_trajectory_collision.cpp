#include "gameholder_pch.h"
#include "skill_object.h"
#include "battle_unit.h"
#include "scene.h"
#include "skill_utils.h"
#include "skill_define.h"
#include "skill_bullet_trajectory_collision.h"
#include "skill_logic_bullet.h"
#include "game_util.h"



SkillBulletTrajectory_Collision::SkillBulletTrajectory_Collision()
{
    m_PassTime = 0;
    m_CollisionCounter = 0;
    memset(m_CollisionIDs, 0, sizeof(m_CollisionIDs));
    memset(m_CollisionTimes, 0 ,sizeof(m_CollisionTimes));
}

SkillBulletTrajectory_Collision::~SkillBulletTrajectory_Collision()
{

}

void SkillBulletTrajectory_Collision::Init(SkillLogicBullet* pBullet)
{
    m_Bullet = pBullet;

    // 碰撞型
    m_BulletData = &m_Bullet->m_SkillData->m_BulletData;
    CnAssert(m_BulletData->m_BulletMode == SKILL_BULLET_MODE_COLLISION);


    CoVec2 CastPos = SkillUtils::GetCasterPosition(m_Bullet->m_Skill, m_Bullet->m_BulletTarget);

    // 计算终点位置
    const SHOOT_TARGET& target = m_Bullet->m_BulletTarget;
    CoVec2 TargetPostion(target.target_x, target.target_y);

    // 计算弹道时间
    m_ShootTime = m_BulletData->m_ShootTime;
    // 碰撞型弹道速度
    m_ShootSpeed.Set((real)TargetPostion.x - CastPos.x, (real)TargetPostion.y - CastPos.y);
    m_ShootSpeed.Normalize();

    // 弹道初始位置(前移10个距离,便于计算方向)
    m_BulletStartPosition = CastPos + m_ShootSpeed*10;
    m_BulletPosition = m_BulletStartPosition;

    // 弹道偏角
    int32 offsetAngle = m_BulletData->m_OffsetAngle + m_Bullet->m_BulletArgs.m_StartAngle;   // 设置偏角+弹道偏角修正
    if(offsetAngle != 0)
    {
        int32 orginAngle = RADIAN_TO_ANGLE(m_ShootSpeed.GetAngle());
        m_ShootSpeed.SetAngle(1.0f, ANGLE_TO_RADIAN(orginAngle + offsetAngle));
    }
    m_ShootSpeed *= (real)Crown::SDMax(m_BulletData->m_ShootSpeed * ToPercent(m_Bullet->m_SkillArgs->m_BulletSpeed), 0.01f);

    // 子弹方向
    if(m_ShootSpeed.LengthSquared() == 0)
        m_BulletAngle = m_Bullet->GetSkill()->GetOwner()->GetAngle();            // 起始结束位置相同，则取角色方向
    else
        m_BulletAngle = RADIAN_TO_ANGLE(m_ShootSpeed.GetAngle());

    // 碰撞次数
    m_CollisionCounter = m_BulletData->m_CollisionCount;
    if(m_CollisionCounter <= 0) 
        m_CollisionCounter = 999999;        // 无限碰撞
}

void SkillBulletTrajectory_Collision::Update()
{
    UpdateCollision();

    m_PassTime += g_ServerUpdateTime;
}

void SkillBulletTrajectory_Collision::UpdateCollision()
{
    if(m_CollisionCounter <= 0 || m_PassTime >= m_ShootTime)
    {   // 弹道结束
        if(m_CollisionCounter > 0 && m_BulletData->m_CollisionEndHit)
        {   // 子弹移动到尽头后是否要触发一次命中逻辑
            SHOOT_TARGET hitTarget;
            hitTarget.caster_id = m_Bullet->m_BulletTarget.caster_id;
            hitTarget.target_id = 0;
            hitTarget.target_x = int32(m_BulletPosition.x);
            hitTarget.target_y = int32(m_BulletPosition.y);

            HitArgs hitArgs;
            hitArgs.m_HitAngle = m_BulletAngle;

            m_Bullet->OnBulletHit(hitTarget, hitArgs);
        }
        m_Bullet->OnBulletEnd();
        return;
    }

    // 更新弹道位置
    m_BulletPosition.x += m_ShootSpeed.x * (real)g_ServerUpdateTime / 1000;
    m_BulletPosition.y += m_ShootSpeed.y * (real)g_ServerUpdateTime / 1000;

    BattleUnit* Owner = m_Bullet->m_Skill->GetOwner();
    // 判断是否飞到世界外面了
    if(Owner->GetScene()->TestCircleWithBound(m_BulletPosition, m_BulletData->m_CollisionRadius))
    {
        m_CollisionCounter = 0;       // 强制碰撞结束
        return;
    }
    
    // 碰撞周期更新
    for(int32 i=0; i<SHOOT_MAX_COLLISION; i++)
    {
        if(m_CollisionTimes[i] <= 0)
        {
            CnAssert(m_CollisionIDs[i] == 0);
            continue;
        }

        m_CollisionTimes[i] -= g_ServerUpdateTime;
        if(m_CollisionTimes[i] <= 0)
        {
            CnAssert(m_CollisionIDs[i] != 0);
            m_CollisionIDs[i] = 0;
            m_CollisionTimes[i] = 0;
        }
    }

    if(m_BulletData->m_CollisionRadius <= 0)
        return;     // 碰撞半径为0,则不做碰撞

    // 和生物对象的碰撞检测
    const int32 findNum = 30;       // 最多检测N个
    BattleUnit* pTargetList[findNum];
    Owner->GetScene()->FindBattleUnitInCircle(m_BulletPosition, m_BulletData->m_CollisionRadius, pTargetList, findNum);

    // 处理未碰撞对象
    for(int32 i=0; i<findNum; i++)
    {
        if(m_CollisionCounter <= 0)
            break;

        BattleUnit* pTarget = pTargetList[i];
        if(!pTarget)
            break;

        if(pTarget->IsDead())
            continue;           // 死亡目标不再受任何影响

        if(pTarget->GetId() == m_Bullet->m_BulletTarget.caster_id)
            continue;           // 施法者不受碰撞

        if(!SkillUtils::CanAttack(m_Bullet->m_Skill, Owner, pTarget))        // 是否可以对目标产生效果
            continue;

        if(!CheckCollisionID(pTarget->GetId()))
        {
            if(FillCollisionID(pTarget->GetId()))
            {
                // 穿刺判断
                bool bSubCollision = true;
                uint32 piercePercent = m_Bullet->m_SkillArgs->m_SkillProps.GetValue(PROP_SKILL_PIERCE);
                if(piercePercent> 0)
                {
                    if(piercePercent >= Crown::SDRandom(BASE_RATE_NUM))
                        bSubCollision = false;
                }

                if(bSubCollision)
                    m_CollisionCounter--;

                // 碰撞到某生物
                SHOOT_TARGET hitTarget;
                hitTarget.caster_id = m_Bullet->m_BulletTarget.caster_id;
                hitTarget.target_id = pTarget->GetId();
                hitTarget.target_x = pTarget->GetX();
                hitTarget.target_y = pTarget->GetY();

                HitArgs hitArgs;
                hitArgs.m_HitAngle = m_BulletAngle;

                m_Bullet->OnBulletHit(hitTarget, hitArgs);
            }
            else
                break;
        }
    }

}

bool SkillBulletTrajectory_Collision::CheckCollisionID(uint32 object_id)
{
    for(int32 i=0; i<SHOOT_MAX_COLLISION; i++)
    {
        if(m_CollisionIDs[i] == object_id)
            return true;
    }
    return false;
}

bool SkillBulletTrajectory_Collision::FillCollisionID(uint32 object_id)
{
    for(int32 i=0; i<SHOOT_MAX_COLLISION; i++)
    {
        if(m_CollisionIDs[i] == 0)
        {
            m_CollisionIDs[i] = object_id;
            m_CollisionTimes[i] = ( m_BulletData->m_CollisionTime <= 0 ? 600000 : m_BulletData->m_CollisionTime );
            return true;
        }
    }
    return false;
}


