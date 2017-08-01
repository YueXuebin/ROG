#include "gameholder_pch.h"
#include "skill_manager.h"
#include "skill_object.h"
#include "battle_unit.h"
#include "skill_define.h"
#include "scene.h"
#include "action_controller.h"
#include "creature.h"
#include "state_define.h"
#include "battle_manager.h"
#include "player.h"
#include "skill_node_pool.h"
#include "game_util.h"
#include "state_controller.h"
#include "skill_info_manager.h"
#include "skill_utils.h"


SkillObject::SkillObject( const SkillData* pSkillData, BattleUnit* owner, const SkillArgumentData& args ) :
m_Owner(owner),
m_dead(false),
m_pSkillData(pSkillData),
m_SkillArgs(args),
m_RunOnceUpdate(false),
m_NodeIdCounter(1),
m_BulletIndexCounter(1),
m_CalcShootData(NULL)
{
    SkillManager::Instance()->IncSkillCount();

    m_CastTimeSum = 0;
    m_TriggerFlags = 0;

    m_SkillSlot = 0;

    // 根据技能参数，构建时间节点
    BuildCastLogic();
}

SkillObject::~SkillObject()
{
    // 释放所有的技能逻辑节点
    for(uint32 i=0; i<m_AllLogics.size(); i++)
    {
        DestorySkillNode(m_AllLogics[i]);
    }

    SkillManager::Instance()->DecSkillCount();
}

// 构建cast逻辑节点
void SkillObject::BuildCastLogic()
{
    m_CastTimeSum = 0;
    for(int32 i=0; i<(m_SkillArgs.m_CastMulti+1); i++)
    {
        SkillLogicCast* pCast = CreateLogicCast();

        if(i==0)
            pCast->m_IsFirst = true;
        else if(i == m_SkillArgs.m_CastMulti)
            pCast->m_IsLast = true;
        
        pCast->m_StartTime = m_CastTimeSum;

        m_CastTimeSum += pCast->GetTime();
    }
}

SkillLogicCast* SkillObject::CreateLogicCast()
{
    SkillLogicCast* pCast = SkillManager::Instance()->GetSkillNodePool()->CreateLogicCastNode();
    pCast->Init(this);
    pCast->Active();
    m_AllLogics.push_back(pCast);
    return pCast;
}

SkillLogicLaunch* SkillObject::CreateLogicLaunch()
{
    SkillLogicLaunch* pLaunch = SkillManager::Instance()->GetSkillNodePool()->CreateLogicLaunchNode();
    pLaunch->Init(this);
    pLaunch->Active();
    m_AllLogics.push_back(pLaunch);

    // 通知客户端，触发发射逻辑
    SKILL_LAUNCH_NTF ntf;
    ntf.owner_id = GetOwner()->GetId();
    ntf.skill_id = GetSkillId();
    ntf.skill_obj_id = GetSkillObjectID();

    GetOwner()->GetScene()->NotifyMessage(GetOwner(), MSG_SKILL_LAUNCH_NTF, &ntf);
    return pLaunch;
}

SkillLogicBullet* SkillObject::CreateLogicBullet(const SHOOT_TARGET& bulletTarget, const BulletArgs& args)
{
    SkillLogicBullet* pBullet = SkillManager::Instance()->GetSkillNodePool()->CreateLogicBulletNode(bulletTarget, args);
    m_BulletIndexCounter++;
    pBullet->Init(this, m_BulletIndexCounter);
    pBullet->Active();
    m_AllLogics.push_back(pBullet);
    return pBullet;
}

SkillLogicHit* SkillObject::CreateLogicHit(const SHOOT_TARGET& hitTarget, const HitArgs& args)
{
    SkillLogicHit* pHit= SkillManager::Instance()->GetSkillNodePool()->CreateLogicHitNode(hitTarget, args);
    pHit->Init(this);
    pHit->Active();
    m_AllLogics.push_back(pHit);

    // 通知客户端，触发命中逻辑
    SKILL_HIT_NTF ntf;
    ntf.owner_id = GetOwner()->GetId();
    ntf.skill_id = GetSkillId();
    ntf.skill_obj_id = GetSkillObjectID();
    ntf.hit_direction = args.m_HitAngle;
    ntf.hit_target = hitTarget;
    GetOwner()->GetScene()->NotifyMessage(GetOwner(), MSG_SKILL_HIT_NTF, &ntf);

    return pHit;
}

SkillLogicStrike* SkillObject::CreateLogicStrike(const SHOOT_TARGET& strikeTarget, const StrikeArgs& args)
{
    SkillLogicStrike* pStrike = SkillManager::Instance()->GetSkillNodePool()->CreateLogicStrikeNode(strikeTarget, args);
    pStrike->Init(this);
    pStrike->Active();
    m_AllLogics.push_back(pStrike);

    // 通知客户端，触发打击逻辑
    SKILL_STRIKE_NTF ntf;
    ntf.owner_id = GetOwner()->GetId();
    ntf.target_id = strikeTarget.target_id;
    ntf.skill_id = GetSkillId();
    ntf.skill_obj_id = GetSkillObjectID();
    ntf.strike_direction = args.m_StrikeAngle;
    GetOwner()->GetScene()->NotifyMessage(GetOwner(), MSG_SKILL_STRIKE_NTF, &ntf);

    return pStrike;
}

void SkillObject::NotifyBulletEnd(SkillLogicBullet* bullet)
{
    // 通知客户端，子弹结束
    SKILL_BULLET_NTF ntf;
    ntf.owner_id = GetOwner()->GetId();
    ntf.skill_id = GetSkillId();
    ntf.skill_obj_id = GetSkillObjectID();
    ntf.bullet_obj_id = bullet->GetBulletIndex();
    GetOwner()->GetScene()->NotifyMessage(GetOwner(), MSG_SKILL_BULLET_END_NTF, &ntf);
}

void SkillObject::BreakOff()
{
    // 打断cast逻辑
    for(uint32 i=0; i<m_AllLogics.size(); i++)
    {
        SkillLogicBase* pLogic = m_AllLogics[i];
        if(pLogic->GetLogicType() != SKILL_LOGIC_CAST)
            continue;
        
        SkillLogicCast* pCastLogic = (SkillLogicCast*)pLogic;
        pCastLogic->BreakOff();

    }

}
//　是否可以进入施法判断
bool SkillObject::CanCast()
{
    BattleUnit* owner = GetOwner();

    if(m_SkillArgs.m_SkillMode == SKILL_MODE_SELF)
    {   // 自身施法，受状态机影响
        if(!owner->CanChangeTo(STATE_ATTACK))
            return false;
        else
            return true;
    }
    //else if(m_SkillArgs.m_SkillMode == SKILL_MODE_SCENE)
    //{
    //    return true;
    //}
    //else if(m_SkillArgs.m_SkillMode == SKILL_MODE_FOLLOW_PET)
    //{
    //    return true;
    //}
    else
        CnAssert(false);

    return false;
}

void SkillObject::EnterCast()
{
    BattleUnit* owner = GetOwner();
    if(m_SkillArgs.m_SkillMode == SKILL_MODE_SELF)
    {
        if(owner->IsKindOf(&Player::ms_RTTI))
        {   // 玩家的施法时间修正
            int32 castTime = Crown::SDMax(int32(m_CastTimeSum - g_ServerUpdateTime), 0);       // 施法时间(扣除当前帧)
            if(castTime > g_ServerUpdateTime)       // 施法状态大于一帧时间，才进入状态。避免极高攻速时释放技能失败
            {   // 进入施法状态
                owner->GetStateController()->SetCanCastBreak(m_pSkillData->m_CastData.m_CanBreak);
                owner->GetStateController()->SetAttackTime(castTime);
                owner->ChangeState(STATE_ATTACK);
            }
        }
        else
        {
            if(m_CastTimeSum > 0)
            {
                owner->GetStateController()->SetCanCastBreak(m_pSkillData->m_CastData.m_CanBreak);
                owner->GetStateController()->SetAttackTime(m_CastTimeSum);
                owner->ChangeState(STATE_ATTACK);
            }
        }
    }
}

void SkillObject::LeaveCast()
{

}

void SkillObject::DestorySkillNode(SkillLogicBase* pSkillLogic)
{
    SkillNodePool* pool = SkillManager::Instance()->GetSkillNodePool();
    pool->DestorySkillNode(pSkillLogic);
}

bool SkillObject::Init(const SHOOT_TARGET& target, const SkillArgumentData& args, uint32 skill_slot)
{
    m_SkillSlot = skill_slot;

    // 记录技能目标参数
    m_Target = target;
    if(m_Target.caster_id == 0)
        m_Target.caster_id = m_Owner->GetId();

    m_SkillArgs.AddTargetId(m_Target.caster_id);            // 施法者不作为弹射目标

    // 计算使用技能的方向
    CoVec2 dir(target.target_x - m_Owner->GetX(), target.target_y - m_Owner->GetY());

    // 是否可以使用该技能
    if(!MakeTarget())
        return false;

    if(args.m_SkillMode != SKILL_MODE_SELF)
        return true;

    // 使用技能的消耗
    if(!OwnerConsume())
        return false;

    // 使用技能时,面向目标
    if(!m_Owner->GetCantAngle())
    {
        m_Owner->SetAngle(RADIAN_TO_ANGLE(dir.GetAngle()));
    }

    return true;
}

void SkillObject::Update()
{
    if(m_dead)
        return;

    bool hasOneActive = false;

    uint32 LogicIndex = 0;
    while(LogicIndex < m_AllLogics.size())
    {
        SkillLogicBase* pLogic = m_AllLogics[LogicIndex];
        if(pLogic->IsActive())
        {
            pLogic->Update();
            hasOneActive = true;
        }
        LogicIndex++;
    }

    if(!hasOneActive)
    {
        m_dead = true;      // 无可执行的逻辑节点了,技能结束
    }

    if(!m_RunOnceUpdate)
        m_RunOnceUpdate = true;
}

void SkillObject::OnStateChange( uint32 state )
{
    if(!m_RunOnceUpdate)
        return;         // 如果技能还没有做过一次Update，表示技能逻辑还没有执行起来，因此不对状态改变做响应

    uint32 LogicIndex = 0;
    while(LogicIndex < m_AllLogics.size())
    {
        SkillLogicBase* pLogic = m_AllLogics[LogicIndex];
        pLogic->OnStateChange(state);
        LogicIndex++;
    }
}

// 是否可以使用技能(检测并构建技能目标)
bool SkillObject::MakeTarget()
{
    if(m_pSkillData->m_TargetData.m_TargetType == SKILL_TARGET_SELF)
    {   // 目标是自己
        m_Target.target_id = m_Owner->GetId();
        m_Target.target_x = m_Owner->GetX();
        m_Target.target_y = m_Owner->GetY();
        return true;
    }
    else if(m_pSkillData->m_TargetData.m_TargetType == SKILL_TARGET_POS)
    {   // 目标是位置
        SceneObject* pTarget = m_Owner->GetScene()->GetSceneObject(m_Target.target_id);
        if(pTarget)
        {
            m_Target.target_x = pTarget->GetX();
            m_Target.target_y = pTarget->GetY();
        }
        else
        {
            m_Target.target_id = 0;
            m_Target.target_x = m_Target.target_x;
            m_Target.target_y = m_Target.target_y;
        }
        return true;
    }
    else if(m_pSkillData->m_TargetData.m_TargetType == SKILL_TARGET_OBJECT)
    {   // 目标是对象
        SceneObject* pTarget = m_Owner->GetScene()->GetSceneObject(m_Target.target_id);
        if(pTarget)
        {
            m_Target.target_id = pTarget->GetId();
            m_Target.target_x = pTarget->GetX();
            m_Target.target_y = pTarget->GetY();
        }
        else
        {
            m_Target.target_id = 0;
            m_Target.target_x = m_Target.target_x;
            m_Target.target_y = m_Target.target_y;
        }
        return true;
    }

    return false;
}
// 使用技能的消耗
bool SkillObject::OwnerConsume()
{
    if(m_Owner->GetObjectType() == OBJ_PLAYER)
    {
        if( ((Player*)m_Owner)->IsGMNoCD() )
            return true;
    }

    return true;
}

void SkillObject::End()
{
    // 结束所有逻辑节点
    for(uint32 i=0; i<m_AllLogics.size(); i++)
    {
        m_AllLogics[i]->End();
    }
}


