/*
 *	技能相关的功能函数
 */
#ifndef skill_utils_h__
#define skill_utils_h__

class SkillObject;
class BattleUnit;


namespace SkillUtils
{
    // 获得caster的位置
    CoVec2 GetCasterPosition(SkillObject* pSkill, const SHOOT_TARGET& target);
    // 获得target的位置
    CoVec2 GetTargetPosition(SkillObject* pSkill, const SHOOT_TARGET& target);

    // 获得玩家的技能魔法消耗
    int32 GetPlayerSkillMap(uint32 skill_id, uint32 player_level);

    // 判断技能是否可以作用于对象
    bool CanAttack(SkillObject* pSkill, BattleUnit* owner, BattleUnit* target);

}


#endif // skill_utils_h__
