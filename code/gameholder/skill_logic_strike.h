/*
 *	击中处理流程
 */
#ifndef skill_hit_proc_h__
#define skill_hit_proc_h__

#include "skill_logic_base.h"



class SkillLogicStrike : public SkillLogicBase
{
public:
    SkillLogicStrike(const SHOOT_TARGET& strikeTarget, const StrikeArgs& args);
    virtual ~SkillLogicStrike();
    virtual uint32  GetLogicType() {return SKILL_LOGIC_STRIKE;}

    virtual void Init(SkillObject* pSkill);

    virtual void Update();
    virtual void OnStateChange(uint32 state) {}

private:
    void CauseBuff(BattleUnit* pTarget);
    void CauseMove(BattleUnit* Target);
    void MoveEnemy(BattleUnit* Owner, BattleUnit* Target, int32 PushMode, int32 PushTime, int32 PushDistance, int32 BounceTime, int32 BounceHeight);

private:
    SHOOT_TARGET    m_StrikeTarget;
    StrikeArgs      m_StrikeArgs;
};

#endif // skill_hit_proc_h__
