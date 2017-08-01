#include "gameholder_pch.h"
#include "skill_logic_launch.h"
#include "skill_logic_bullet.h"
#include "skill_utils.h"



SkillLogicLaunch::SkillLogicLaunch()
{
    m_LaunchData = NULL;

    m_BulletIntervalTimer = 0;
    m_BulletRepeatCounter = 0;
}

SkillLogicLaunch::~SkillLogicLaunch()
{
    m_LaunchData = NULL;
}

void SkillLogicLaunch::Init(SkillObject* pSkill)
{
    SkillLogicBase::Init(pSkill);

    m_LaunchData = &m_SkillData->m_LaunchData;

    m_BulletRepeatCounter = m_SkillArgs->m_BulletRepeat+1;

    m_BulletIntervalTimer = 0;
}

void SkillLogicLaunch::Update()
{
    if(m_BulletIntervalTimer <= 0)
    {   // 发射下一个弹道
        for(int i=0; i<(m_SkillArgs->m_BulletMulti+1); i++)
        {
            // 子弹目标
            SHOOT_TARGET bulletTarget = CalcBulletTarget();
            if(bulletTarget.target_id)
                m_Skill->GetSkillArgument().AddTargetId(bulletTarget.target_id);                    // 记录子弹攻击的目标
            // 子弹参数
            BulletArgs bulletArgs;
            int32 startAngle = -(m_SkillArgs->m_BulletMulti)*m_SkillArgs->m_BulletMultiAngle/2;        // 计算弹道偏角
            bulletArgs.m_StartAngle = startAngle + i*m_SkillArgs->m_BulletMultiAngle;
            bulletArgs.m_Chain = m_SkillArgs->m_BulletChain;                                            // 计算弹道弹射次数

            // 触发弹道
            SkillLogicBullet* pBullet = m_Skill->CreateLogicBullet(bulletTarget, bulletArgs);
        }

        m_BulletIntervalTimer += m_SkillArgs->m_BulletRepeatInterval;
        m_BulletRepeatCounter--;
    }

    m_BulletIntervalTimer -= g_ServerUpdateTime;

    if(m_BulletRepeatCounter <= 0)          // 子弹发射完毕，发射逻辑就结束了
    {
        End();
    }
}
// 计算弹道的目标
SHOOT_TARGET SkillLogicLaunch::CalcBulletTarget()
{
    const SkillBulletData& bulletData = m_SkillData->m_BulletData;

    const SHOOT_TARGET& skillTarget = m_Skill->GetTarget();
    
    SHOOT_TARGET bulletTarget;
    CoVec2 TargetPostion;
    
    bulletTarget.caster_id = skillTarget.caster_id;
    
    // 技能对目标
    if(m_SkillData->m_TargetData.m_TargetType == SKILL_TARGET_OBJECT)
    {
        // 弹道对目标
        if(bulletData.m_BulletTargetData.m_TargetType == SKILL_TARGET_OBJECT)
        {
            TargetPostion = SkillUtils::GetTargetPosition(m_Skill, skillTarget);      // 对目标需要重新计算位置
            bulletTarget.target_id = skillTarget.target_id;
            bulletTarget.target_x = int32(TargetPostion.x);
            bulletTarget.target_y = int32(TargetPostion.y);
        }
        // 弹道对位置
        else if(bulletData.m_BulletTargetData.m_TargetType == SKILL_TARGET_POS)
        {
            TargetPostion = SkillUtils::GetTargetPosition(m_Skill, skillTarget);      // 对目标需要重新计算位置
            bulletTarget.target_id = 0;
            bulletTarget.target_x = int32(TargetPostion.x);
            bulletTarget.target_y = int32(TargetPostion.y);
        }
        // 弹道对自己
        else if(bulletData.m_BulletTargetData.m_TargetType == SKILL_TARGET_SELF)
        {
            TargetPostion = SkillUtils::GetCasterPosition(m_Skill, skillTarget);
            bulletTarget.target_id = skillTarget.caster_id;
            bulletTarget.target_x = int32(TargetPostion.x);
            bulletTarget.target_y = int32(TargetPostion.y);
        }
        else
            CnAssert(false);
    }
    // 技能对位置
    else if(m_SkillData->m_TargetData.m_TargetType == SKILL_TARGET_POS)
    {
        // 弹道对目标,弹道对位置
        if(bulletData.m_BulletTargetData.m_TargetType == SKILL_TARGET_OBJECT || bulletData.m_BulletTargetData.m_TargetType == SKILL_TARGET_POS)
        {
            bulletTarget.target_id = 0;
            bulletTarget.target_x = int32(skillTarget.target_x);
            bulletTarget.target_y = int32(skillTarget.target_y);
        }
        // 弹道对自己
        else if(bulletData.m_BulletTargetData.m_TargetType == SKILL_TARGET_SELF)
        {
            TargetPostion = SkillUtils::GetCasterPosition(m_Skill, skillTarget);
            bulletTarget.target_id = 0;
            bulletTarget.target_x = int32(TargetPostion.x);
            bulletTarget.target_y = int32(TargetPostion.y);
        }
        else
            CnAssert(false);
    }
    // 技能对自己
    else if(m_SkillData->m_TargetData.m_TargetType == SKILL_TARGET_SELF)
    {
        TargetPostion = SkillUtils::GetCasterPosition(m_Skill, skillTarget);      // 对目标需要重新计算位置
        // 弹道对目标,弹道对自己
        if(bulletData.m_BulletTargetData.m_TargetType == SKILL_TARGET_OBJECT || bulletData.m_BulletTargetData.m_TargetType == SKILL_TARGET_SELF)
        {
            bulletTarget.target_id = skillTarget.caster_id;
            bulletTarget.target_x = int32(TargetPostion.x);
            bulletTarget.target_y = int32(TargetPostion.y);
        }
        // 弹道对位置
        else if(bulletData.m_BulletTargetData.m_TargetType == SKILL_TARGET_POS)
        {
            bulletTarget.target_id = 0;
            bulletTarget.target_x = int32(TargetPostion.x);
            bulletTarget.target_y = int32(TargetPostion.y);
        }
    }

    return bulletTarget;
}