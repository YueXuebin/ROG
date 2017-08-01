#ifndef ai_condition_master_attack_h__
#define ai_condition_master_attack_h__

#include "ai_condition_base.h"

/*
 *	AI条件：主人攻击
 */
class AIConditionMasterAttack : public AIConditionBase
{
public:
    AIConditionMasterAttack();
    virtual ~AIConditionMasterAttack();

    virtual void onEnterState();
    virtual bool IsSatisfy();

    virtual void onMasterAttack(BattleUnit* pDefender);
private:
    bool   m_attack;
};

#endif // ai_condition_master_attack_h__
