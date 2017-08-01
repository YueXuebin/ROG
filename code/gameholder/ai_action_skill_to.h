#ifndef ai_action_skill_to_h__
#define ai_action_skill_to_h__

#include "ai_action_base.h"

/*
 *	AI行为：使用技能
 */
class AIActionSkillTo : public AIActionBase
{
public:
    AIActionSkillTo();
    virtual ~AIActionSkillTo();

    virtual void Do();

private:
    void DoMonster(const SHOOT_TARGET& target);
    void DoPet(const SHOOT_TARGET& target);
};

#endif // ai_action_skill_to_h__
