#ifndef ai_condition_master_onhit_h__
#define ai_condition_master_onhit_h__

#include "ai_condition_base.h"

/*
 *	AI条件：主人受到攻击(跟宠)
 */
class AIConditionMasterOnhit : public AIConditionBase
{
public:
    AIConditionMasterOnhit();
    virtual ~AIConditionMasterOnhit();

    virtual void onEnterState();
    virtual bool IsSatisfy();

    virtual void onMasterHit(BattleUnit* pAttacker);
private:
    bool    m_IsHit;
};

#endif // ai_condition_master_onhit_h__
