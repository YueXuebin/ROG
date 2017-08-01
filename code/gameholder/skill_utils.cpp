#include "gameholder_pch.h"
#include "skill_utils.h"
#include "battle_unit.h"
#include "scene.h"
#include "skill_object.h"
#include "skill_config_manager.h"
#include "game_util.h"


CoVec2 SkillUtils::GetCasterPosition(SkillObject* pSkill, const SHOOT_TARGET& target)
{
    BattleUnit* pCaster = pSkill->GetOwner()->GetScene()->GetBattleUnit(target.caster_id);
    if(pCaster)
        return pCaster->GetPosition();
    else
        return CoVec2(0, 0);
}

CoVec2 SkillUtils::GetTargetPosition(SkillObject* pSkill, const SHOOT_TARGET& target)
{
    BattleUnit* pTarget = pSkill->GetOwner()->GetScene()->GetBattleUnit(target.target_id);
    if(pTarget)
        return pTarget->GetPosition();
    else
        return CoVec2(target.target_x, target.target_y);
}

int32 SkillUtils::GetPlayerSkillMap(uint32 skill_id, uint32 player_level)
{
    // 获得技能耗兰
    return 0;
}

// 是否可以作用于目标对象
bool SkillUtils::CanAttack(SkillObject* pSkill, BattleUnit* owner, BattleUnit* target)
{
    if(!target->GetAttackable())
        return false;

    const SkillData* pSkillData = pSkill->GetSkillData();

    uint32 skillID = pSkill->GetSkillId();

    const SKILL_ROOT_SKILL_STRUCT* pSkillConfig = SkillConfigManager::Instance()->GetSkillConfigData(skillID);
    if(!pSkillConfig)
        return false;

    if(owner == target)     // 目标是技能所有者
    {
        if(pSkillConfig->to_me)
            return true;
        else
            return false;
    }
    else
    {
        if(pSkillConfig->to_friend && !owner->IsEnemy(target))
        {
            return true;
        }
        if(pSkillConfig->to_enemy&& owner->IsEnemy(target) && target->GetAttackable())
        {
            return true;
        }
    }
    return false;
}

