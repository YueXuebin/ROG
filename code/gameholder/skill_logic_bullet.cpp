#include "gameholder_pch.h"
#include "skill_object.h"
#include "battle_unit.h"
#include "skill_define.h"
#include "scene.h"
#include "action_controller.h"
#include "ai_controller.h"
#include "pet_controller.h"
#include "pet.h"
#include "skill_utils.h"
#include "skill_logic_bullet.h"
#include "skill_controller.h"
#include "skill_config_manager.h"
#include "event_define.h"



SkillLogicBullet::SkillLogicBullet(const SHOOT_TARGET& bulletTarget, const BulletArgs& args) :
m_BulletTarget(bulletTarget),
m_BulletArgs(args),
m_BulletIndex(0)
{
    m_BulletData = NULL;
}

SkillLogicBullet::~SkillLogicBullet()
{
    m_BulletData = NULL;
}

void SkillLogicBullet::Init(SkillObject* pSkill, uint32 bulletIndex)
{
    SkillLogicBase::Init(pSkill);

    m_BulletIndex = bulletIndex;

    m_BulletData = &pSkill->GetSkillData()->m_BulletData;

    BattleUnit* Owner = m_Skill->GetOwner();
    CnAssert(Owner);

    if(m_BulletData->m_BulletMode == SKILL_BULLET_MODE_DIRECT)
        m_TrajectoryDirect.Init(this);
    else if(m_BulletData->m_BulletMode == SKILL_BULLET_MODE_COLLISION)
        m_TrajectoryCollision.Init(this);
    else
        CnAssert(false);

}

void SkillLogicBullet::Update()
{
    if(m_BulletData->m_BulletMode == SKILL_BULLET_MODE_DIRECT)
    {
        m_TrajectoryDirect.Update();
    }
    else if(m_BulletData->m_BulletMode == SKILL_BULLET_MODE_COLLISION)
    {
        m_TrajectoryCollision.Update();
    }
    else
        CnAssert(false);

}

// 弹道结束
void SkillLogicBullet::OnBulletEnd()
{
    End();

    if(m_BulletData->m_BulletMode == SKILL_BULLET_MODE_COLLISION)
    {   // 碰撞型技能，才通知子弹结束
        m_Skill->NotifyBulletEnd(this);
    }
}
// 弹道命中
void SkillLogicBullet::OnBulletHit(const SHOOT_TARGET& hitTarget, const HitArgs& args)
{
    // 触发命中逻辑
    SkillLogicHit* pHitLogic = m_Skill->CreateLogicHit(hitTarget, args);


    // 触发弹射逻辑
    if(m_BulletArgs.m_Chain > 0)
    {
        OnBulletChain(hitTarget);
    }

    // 投射命中事件
    if(m_Skill->GetSkillArgument().m_SkillMode == SKILL_MODE_SELF)
    {
        const SKILL_ROOT_SKILL_STRUCT* skillConfig = SkillConfigManager::Instance()->GetSkillConfigData(m_Skill->GetSkillId());
        if((skillConfig->feature & SKILL_FEATURE_PROJECTILE)
            && ((m_Skill->m_TriggerFlags & SKILL_TRIGGER_PROJECTILE_EVENT) == 0)
            )
        {
            m_Skill->GetOwner()->GetScene()->SendEvent(m_Skill->GetOwner()->GetId(), BATTLE_EVENT_SELF_SKILL_PROJECTILE, hitTarget.target_id, 0, 0);
            m_Skill->m_TriggerFlags |= SKILL_TRIGGER_PROJECTILE_EVENT;
        }
    }
}
// 触发技能弹射
void SkillLogicBullet::OnBulletChain(const SHOOT_TARGET& hitTarget)
{
    // 计算弹射目标
    SHOOT_TARGET chainTarget;
    chainTarget.caster_id = hitTarget.target_id;

    BattleUnit* chainBattleUnit = FindChainTarget(hitTarget);       // 查找弹射弹道的目标
    if(chainBattleUnit)
    {
        chainTarget.target_id = chainBattleUnit->GetId();
        chainTarget.target_x = (int32)chainBattleUnit->GetPosition().x;
        chainTarget.target_y = (int32)chainBattleUnit->GetPosition().y;
    }
    else if(m_BulletData->m_BulletMode == SKILL_BULLET_MODE_COLLISION)
    {   // 如果没有弹射对象，且弹道为碰撞型，则朝随机方向弹射
        chainTarget.target_id = 0;
        chainTarget.target_x = hitTarget.target_x + Crown::SDRandom(100) - 200;
        chainTarget.target_y = hitTarget.target_y + Crown::SDRandom(100) - 200;
    }
    else
        return;     // 没有弹射目标

    // 弹射子弹参数
    BulletArgs chainArgs;
    chainArgs.m_StartAngle = 0;
    chainArgs.m_Chain = m_BulletArgs.m_Chain - 1;

    SkillArgumentData chainArgData = m_Skill->GetSkillArgument();
    chainArgData.m_SkillMode = SKILL_MODE_PASSIVE;
    chainArgData.m_BulletChain--;
    chainArgData.m_CastMulti = 0;
    chainArgData.m_BulletMulti = 0;
    chainArgData.m_BulletMultiAngle = 0;
    chainArgData.m_BulletRepeat = 0;
    chainArgData.m_BulletRepeatInterval = 0;
    
    m_Skill->GetOwner()->GetSkillController()->UseSkillByPassive(chainArgData.m_BulletChainID, chainTarget, chainArgData, m_Skill->GetSkillSlot());

}

BattleUnit* SkillLogicBullet::FindChainTarget(const SHOOT_TARGET& hitTarget)
{
    // 搜索目标范围内最近的可攻击对象
    CoVec2 targetPos = SkillUtils::GetTargetPosition(m_Skill, hitTarget);

    BattleUnit* resTarget = NULL;
    real nearDistance = 999999;
    BattleUnit* uints[10];
    int32 targetCount = m_Skill->GetOwner()->GetScene()->FindBattleUnitInCircle(targetPos, m_Skill->GetSkillData()->m_TargetData.m_Range, uints, 10);
    for(int32 ti=0; ti<targetCount; ti++)
    {
        BattleUnit* pTarget = uints[ti];
        if(pTarget->IsDead())           // 死亡目标不再受任何影响
            continue;

        if(pTarget->GetId() == hitTarget.target_id) // 当前目标不能被弹射
            continue;

        if(m_Skill->GetSkillArgument().HasTargetId(pTarget->GetId()))  // 是否已经攻击过
            continue;

        if(!SkillUtils::CanAttack(m_Skill, m_Skill->GetOwner(), pTarget))              // 是否可以对目标产生效果
            continue;

        real dis = (targetPos - pTarget->GetPosition()).Length();
        if(nearDistance < dis)
            continue;

        nearDistance = dis;
        resTarget = pTarget;
    }
    return resTarget;
}

const CoVec2& SkillLogicBullet::GetPosition()
{
    if(m_BulletData->m_BulletMode == SKILL_BULLET_MODE_DIRECT)
    {
        return m_TrajectoryDirect.GetPosition();
    }
    else if(m_BulletData->m_BulletMode == SKILL_BULLET_MODE_COLLISION)
    {
        return m_TrajectoryCollision.GetPosition();
    }
    else
    {
        CnAssert(false);
        static CoVec2 ret;
        return ret;
    }
}

int32 SkillLogicBullet::GetAngle()
{
    if(m_BulletData->m_BulletMode == SKILL_BULLET_MODE_DIRECT)
    {
        return m_TrajectoryDirect.GetAngle();
    }
    else if(m_BulletData->m_BulletMode == SKILL_BULLET_MODE_COLLISION)
    {
        return m_TrajectoryCollision.GetAngle();
    }
    else
        CnAssert(false);
    return 0;
}

void SkillLogicBullet::HitOnce(const SHOOT_TARGET& target)
{
    //// 召唤处理
    //Summon();
    //// 嘲讽处理
    //Taunt();

    //// 击中处理
    //SkillHitNode* pHidNode = m_Skill->CreateHitNode(this, target, m_ShootIndex, m_ShootLevel);
    //m_Skill->ActiveNode(pHidNode->GetNodeId());
}

//// 对目标范围内的敌人,产生嘲讽
//void SkillLogicBullet::Taunt()
//{
//    BattleUnit* Owner = m_Skill->GetOwner();
//    if(!m_ShootData->m_hasTaunt)
//        return;
//
//    BattleUnit* uint[20];
//    CoVec2 targetPos = SkillUtils::GetTargetPosition(m_Skill, m_ShootTarget);
//    int32 ret = Owner->GetScene()->FindBattleUnitInCircle(targetPos, m_ShootData->m_HitRange, uint, 20);
//    for (int32 i = 0; i < ret; i++)
//    {
//        BattleUnit* pTarget = uint[i];
//
//        if(pTarget->GetCantTaunt())             // 不能被嘲讽
//            continue;
//
//        if(!SkillUtils::CanAttack(m_Skill, Owner, pTarget))          // 是否可以对目标产生效果
//            continue;
//
//        pTarget->GetAIController()->SetTargetUnitId(Owner->GetId());
//    }
//}
//
//// 召唤
//void SkillLogicBullet::Summon()
//{
//    BattleUnit* Owner = m_Skill->GetOwner();
//    CnAssert(Owner);
//    AttackSkillData* pSkillData = (AttackSkillData*)m_Skill->GetSkillData();
//
//    if(!m_ShootData->m_hasSummon)
//        return;
//
//    if(Owner->GetObjectType() == OBJ_PLAYER || Owner->GetObjectType() == OBJ_OFFLINE_PLAYER)
//        Owner->GetPetController()->KillAllPet();      // 玩家只能拥有一个召唤怪
//
//    for(int32 i=0; i<(int32)m_ShootData->m_SummonMonsterNumber; i++)
//    {
//        CoVec2 targetPos;
//
//        if(pSkillData->m_TargetType == SKILL_TARGET_OBJECT || pSkillData->m_TargetType == SKILL_TARGET_POS)
//        {
//            targetPos = SkillUtils::GetTargetPosition(m_Skill, m_ShootTarget);
//        }
//        else if(pSkillData->m_TargetType == SKILL_TARGET_SELF)
//        {
//            targetPos = Owner->GetPosition();
//        }
//        else
//            CnAssert(false);
//
//        // 计算最远的一个可用位置
//        CoVec2 dis(targetPos.x - Owner->GetX(), targetPos.y - Owner->GetY());
//        CoVec2 desPos = Owner->GetScene()->GetCoWorld()->RayTraceFind(Owner->GetPosition(), int32(dis.GetAngle() * 180.f / M_PI), dis.Length(), (real)PHYSICAL_RADIUS);
//        CoVec2 finalPos = desPos;
//        // 召唤半径随机位置
//        int32 testnum = 0;
//        do 
//        {
//            real ra = Crown::SDRandomFloat((real)M_PI*2);
//            int32 rx = int32( Crown::SDRandom(m_ShootData->m_SummonRadius) * cos(ra) );
//            int32 ry = int32( Crown::SDRandom(m_ShootData->m_SummonRadius) * sin(ra) );
//            finalPos.x = desPos.x + rx;
//            finalPos.y = desPos.y + ry;
//
//            testnum++;
//            
//            if( Owner->GetScene()->TestCircleWithTriangles(desPos, PHYSICAL_RADIUS) )
//                finalPos = desPos;
//            else
//                testnum = 3;
//        }
//        while ( testnum < 3 );
//
//        BattlePet* pBattlePet = Owner->SummonPet(m_ShootData->m_SummonMonsterId, pSkillData->m_SkillId, (int32)finalPos.x, (int32)finalPos.y, (m_ShootData->m_SummonType == 0));
//        if(pBattlePet)
//        {
//            pBattlePet->SetDeadByMaster(m_ShootData->m_SummonMonsterDeadByMaster);
//
//            if(m_ShootData->m_SummonAIName.length() > 0)
//                pBattlePet->GetAIController()->LoadAI(m_ShootData->m_SummonAIName);
//        }
//        else
//            CnWarn("summon monster %d not exist!\n", m_ShootData->m_SummonMonsterId);
//    }
//}
