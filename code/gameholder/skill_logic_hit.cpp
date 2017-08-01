#include "gameholder_pch.h"
#include "skill_data.h"
#include "skill_define.h"
#include "skill_object.h"
#include "battle_unit.h"
#include "scene.h"
#include "skill_utils.h"
#include "skill_logic_hit.h"


SkillLogicHit::SkillLogicHit(const SHOOT_TARGET& hitTarget, const HitArgs& args) :
m_HitTarget(hitTarget),
m_HitArgs(args)
{
    m_DebugShow = false;

    m_PassTime = 0;

    m_HitRateTimer = 0;

    m_HitDir.SetAngle(1.0f, (real)args.m_HitAngle);
}

SkillLogicHit::~SkillLogicHit()
{

}

void SkillLogicHit::Init(SkillObject* pSkill)
{
    SkillLogicBase::Init(pSkill);
    m_HitData = &pSkill->GetSkillData()->m_HitData;

    m_DebugShow = true;
}

void SkillLogicHit::Update()
{
    if(m_HitRateTimer <= 0)
    {
        // 触发一次
        HitOnce();

        if(m_HitData->m_HitRate>0)
        {
            m_HitRateTimer += m_HitData->m_HitRate;
            m_DebugShow = true;
        }
        else
            m_HitRateTimer += 99999999;     // HitRate为0，只触发一次
    }

    m_HitRateTimer -= g_ServerUpdateTime;

    if(m_PassTime >= m_HitData->m_HitTime)
    {   // 命中逻辑结束
        End();
        return;
    }

    m_PassTime += g_ServerUpdateTime;

}

void SkillLogicHit::CalcHitPosition()
{
    BattleUnit* Owner = m_Skill->GetOwner();
    BattleUnit* pTarget = NULL;

    if(m_HitTarget.target_id > 0)
    {   // 对目标进行击中逻辑，需要重新计算位置
        pTarget = CnDynamicCast(BattleUnit, Owner->GetScene()->GetSceneObject(m_HitTarget.target_id));
    }

    if(pTarget)
    {
        m_HitPosition = pTarget->GetPosition();
    }
    else
    {
        m_HitPosition.x = real(m_HitTarget.target_x);
        m_HitPosition.y = real(m_HitTarget.target_y);
    }
}

void SkillLogicHit::HitOnce()
{
    BattleUnit* Owner = m_Skill->GetOwner();

    // 弹道结束,击中目标判断
    const int32 listLen = 50;           // 最多50个目标
    BattleUnit* pTargetList[listLen];    
    int32 targetCount = 0;

    CalcHitPosition();      // 计算击中的位置和方向

    // 范围检查
    if(m_HitData->m_HitRange <= 0)        // 无范围
    {   
        if(m_HitTarget.target_id > 0)
        {
            // 技能击中对象
            BattleUnit* pOne = CnDynamicCast(BattleUnit, Owner->GetScene()->GetSceneObject(m_HitTarget.target_id));
            if(pOne)
            {
                pTargetList[0] = pOne;
                targetCount = 1;
            }
        }
        else
        {
            // 位置技能,无范围,则无命中目标

        }
    }
    else    // 有范围
    {   
        CoVec2 dir = m_HitDir;

        if(m_HitData->m_IsRect)
        {   // 矩形范围
            dir.Normalize();
            dir.SetAngle((real)m_HitData->m_HitRange/2, ANGLE_TO_RADIAN(m_HitArgs.m_HitAngle));
            CoVec2 center = m_HitPosition + dir;
            targetCount = Owner->GetScene()->FindBattleUnitInRect(center, m_HitData->m_HitRange/2, m_HitData->m_HitAngle/2, m_HitArgs.m_HitAngle, pTargetList, listLen);
        }
        else
        {   // 扇形范围
            int32 angleSeperation = m_HitData->m_HitAngle/2;
            if(angleSeperation <= 0) angleSeperation = 0;
            if(angleSeperation > 180) angleSeperation = 180;

            targetCount = Owner->GetScene()->FindBattleUnitInFan(m_HitPosition, m_HitData->m_HitRange, m_HitArgs.m_HitAngle, angleSeperation, pTargetList, listLen);
        }
    }

    // 对每个击中的目标，进行处理
    for(int32 ti=0; ti<targetCount; ti++)
    {
        BattleUnit* pTarget = pTargetList[ti];
        if(pTarget->IsDead())
            continue;           // 死亡目标不再受任何影响

        if(!SkillUtils::CanAttack(m_Skill, Owner, pTarget))        // 是否可以对目标产生效果
            continue;

        HitOne(pTarget);        // 击中处理
    }
}

void SkillLogicHit::HitOne(BattleUnit* target)
{
    BattleUnit* Owner = m_Skill->GetOwner();
    // 触发打击逻辑
    SHOOT_TARGET strikeTarget;
    strikeTarget.caster_id = Owner->GetId();
    strikeTarget.target_id = target->GetId();
    strikeTarget.target_x = target->GetX();
    strikeTarget.target_y = target->GetY();

    StrikeArgs args;
    args.m_StrikeAngle = m_HitArgs.m_HitAngle;

    m_Skill->CreateLogicStrike(strikeTarget, args);


    //m_Skill->RecordTargetId(target->GetId());

    //BattleUnit* Owner = m_Skill->GetOwner();
    //SkillHit::Hit(Owner, target, m_Skill, this);
}

///////////////////////////////////////////////////////////////////////////////////////
// 子弹道
void SkillLogicHit::UpdateSubShoot()
{
    //int32 preHitTime = m_PrePassTime;
    //int32 hitTime = m_PassTime;

    //for(uint32 i=0; i<GetShootData()->m_NextShoot.size(); i++)
    //{
    //    const CastShootData& castShootData = GetShootData()->m_NextShoot[i];
    //    if(hitTime > castShootData.m_DelayTime && castShootData.m_DelayTime >= preHitTime)
    //    {
    //        CastSubShoot(castShootData.m_ShoodId);
    //    }
    //}
}

void SkillLogicHit::CastRemainShoot()
{
    //// 触发剩下的再生弹道
    //int32 preHitTime = m_PrePassTime;
    //int32 hitTime = m_PassTime;

    //for(uint32 i=0; i<GetShootData()->m_NextShoot.size(); i++)
    //{
    //    const CastShootData& castShootData = GetShootData()->m_NextShoot[i];
    //    if(castShootData.m_DelayTime >= hitTime)
    //    {
    //        CastSubShoot(castShootData.m_ShoodId);
    //    }
    //}
}

// 触发再生弹道
void SkillLogicHit::CastSubShoot(uint32 shootId)
{
    //BattleUnit* Owner = m_Skill->GetOwner();

    //// 搜索目标范围内最近的可攻击角色
    //real nearDistance = 999999;
    //BattleUnit* resTarget = NULL;
    //BattleUnit* uints[5];

    //CoVec2 targetPos = SkillUtils::GetTargetPosition(m_Skill, m_HitTarget);
    //int32 targetCount = m_Skill->GetOwner()->GetScene()->FindBattleUnitInCircle(targetPos, m_Skill->GetSkillData()->m_NextRange, uints, 5);
    //for(int32 ti=0; ti<targetCount; ti++)
    //{
    //    BattleUnit* pTarget = uints[ti];
    //    if(pTarget->IsDead())
    //        continue;           // 死亡目标不再受任何影响

    //    if(m_Skill->HasTargetId(pTarget->GetId()))  // 是否已经攻击过
    //        continue;

    //    if(HasNextTargetId(pTarget->GetId()))       // 是否已经弹射过
    //        continue;

    //    if(!SkillUtils::CanAttack(m_Skill, Owner, pTarget))              // 是否可以对目标产生效果
    //        continue;

    //    real dis = (targetPos - pTarget->GetPosition()).Length();
    //    if(nearDistance < dis)
    //        continue;

    //    nearDistance = dis;
    //    resTarget = pTarget;
    //}

    //if(resTarget)
    //{
    //    AddNextTargetId(resTarget->GetId());

    //    SHOOT_TARGET shootTarget;
    //    shootTarget.caster_id = m_HitTarget.target_id;
    //    shootTarget.target_id = resTarget->GetId();

    //    SkillShootNode* pShootNode = m_Skill->CreateShootNode(shootId, shootTarget, m_ShootLevel+1);
    //    m_Skill->ActiveNode(pShootNode->GetNodeId());
    //}
}

void SkillLogicHit::AddNextTargetId(uint32 object_id)
{
    if(HasNextTargetId(object_id))
        return;

    CnAssert(m_NextTargetIDS[m_NextTargetCount] == 0);
    m_NextTargetIDS[m_NextTargetCount] = object_id;
    m_NextTargetCount++;
}

bool SkillLogicHit::HasNextTargetId(uint32 object_id)
{
    for(int32 i=0; i<m_NextTargetCount; i++)
    {
        if(m_NextTargetIDS[i] == object_id)
            return true;
    }
    return false;
}

