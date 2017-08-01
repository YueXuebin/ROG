/*
 *	战斗系统管理器
 */
#ifndef battle_manager_h__
#define battle_manager_h__

#include "battle_define.h"

class BattleUnit;
class SkillObject;
class Player;
class FollowPetManager;


class BattleManager
{
public:
    BattleManager();
    ~BattleManager();

    DECLARE_SINGLETON(BattleManager)

    bool Init();

    // 技能攻击计算
    uint32 Attack(BattleUnit* Attacker, BattleUnit* Defender, SkillObject* pSkill);

    // 造成治疗
    void DoHeal(int32 heal, BattleUnit* Attacker, BattleUnit* Defender);

    // 直接造成伤害(对外仅场景技能、buff用)
    void DoDamage(const DamageResult& damage, BattleUnit* Attacker, BattleUnit* Defender, SkillObject* pSkill);

private:

private:

};

#endif // battle_manager_h__
