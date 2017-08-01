#include "gameholder_pch.h"
#include "ai_action_skill_to.h"
#include "battle_unit.h"
#include "ai_controller.h"
#include "scene.h"
#include "action_controller.h"
#include "skill_config_manager.h"
#include "skill_info_manager.h"
#include "soldier.h"

AIActionSkillTo::AIActionSkillTo() :
AIActionBase(AI_ACTION_SKILL_TO)
{

}

AIActionSkillTo::~AIActionSkillTo()
{

}

void AIActionSkillTo::Do()
{
    BattleUnit* pOwner = m_pController->GetOwner();
    
    uint32 targetId = m_pController->GetTargetUnitId();

    BattleUnit* pTarget = pOwner->GetScene()->GetBattleUnit(targetId);
    if (!pTarget)
    {
        return;
    }

    if (!pOwner->CanAttack(pTarget))
    {
        return;
    }

    uint32 skillId = m_pController->GetSelectSkill();
    if(skillId == 0)
    {
        return;
    }

    SceneObject* targetObject = m_pController->GetOwner()->GetScene()->GetSceneObject(targetId);
    CoVec2 skillToPos = pOwner->GetPosition();
    if(targetObject)
    {
        skillToPos = targetObject->GetPosition();
    }

    SHOOT_TARGET target;
    target.caster_id = pOwner->GetId();
    target.target_id = targetId;
    target.target_x = (int32)skillToPos.x;
    target.target_y = (int32)skillToPos.y;

    switch(pOwner->GetObjectType())
    {
    case OBJ_MONSTER:
        {
            DoMonster(target);
        }
        break;
    case OBJ_PET:
        {
            DoPet(target);
        }
        break;
    }
}

void AIActionSkillTo::DoMonster(const SHOOT_TARGET& target)
{
    BattleUnit* pOwner = m_pController->GetOwner();

    uint32 skillId = m_pController->GetSelectSkill();

    SkillArgumentData args;
    const SKILL_ROOT_SKILL_STRUCT* pSkillConfigData = SkillConfigManager::Instance()->GetSkillConfigData(skillId);
    if (pSkillConfigData)
    {
        args.m_BulletChain = pSkillConfigData->bullet_chain;
        args.m_BulletChainID = pSkillConfigData->bullet_chain_id;
        args.m_BulletMulti = pSkillConfigData->bullet_multi;
        args.m_BulletMultiAngle = pSkillConfigData->bullet_multi_angle;
        args.m_BulletRepeat = pSkillConfigData->bullet_repeat;
        args.m_BulletRepeatInterval = pSkillConfigData->bullet_repeat_interval;
        args.m_CastMulti = pSkillConfigData->cast_multi;

        if(pSkillConfigData->prop1_id>0)
        {
            args.m_SkillProps.Add(pSkillConfigData->prop1_id, pSkillConfigData->prop1_base);       // 怪物技能没有成长 
        }
        if(pSkillConfigData->prop2_id>0)
        {
            args.m_SkillProps.Add(pSkillConfigData->prop2_id, pSkillConfigData->prop2_base);
        }
    }

    SkillInfoManager::makeSkillPropSetByBattleUnit(&args.m_SkillProps, pOwner, pSkillConfigData);      // 怪物技能受怪物自身影响

    pOwner->GetActionController()->UseSkill(skillId, target, args, 0, 0);
}

void AIActionSkillTo::DoPet(const SHOOT_TARGET& target)
{
    BattleUnit* pOwner = m_pController->GetOwner();

    uint32 targetId = m_pController->GetTargetUnitId();

    uint32 nSlot = m_pController->GetSelectSkill(false);

    if (!pOwner->IsKindOf(&Soldier::ms_RTTI))
    {
        return;
    }

    Soldier* pSoldier = CnDynamicCast(Soldier, pOwner);

    if (2 == nSlot)
    {
        pSoldier->UseDefaultSkill(target);
        return;
    }

    SkillInfo* pSkillInfo = pSoldier->GetSkillInfoMgr()->GetSkillInfoBySlot(nSlot);
    if (!pSkillInfo)
    {
        CnAssert(false);
        return;
    }

    USE_SKILL_REQ sendMsg;
    sendMsg.skill_id = pSkillInfo->GetSkillId();
    sendMsg.skill_slot = nSlot;
    sendMsg.skill_args = *pSkillInfo->GetSkillArgs();
    sendMsg.skill_args.skill_mode = SKILL_MODE_SELF;
    sendMsg.target = target;
    pSoldier->GetSkillInfoMgr()->OnUseBaseSkill(&sendMsg);
}
