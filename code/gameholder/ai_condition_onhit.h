#ifndef ai_condition_onhit_h__
#define ai_condition_onhit_h__

#include "ai_condition_base.h"

/*
 *	AI条件：被攻击
 */
class AIConditionOnHit : public AIConditionBase
{
public:
    AIConditionOnHit();
    virtual ~AIConditionOnHit();

    virtual void onEnterState();
    virtual bool IsSatisfy();
    virtual void ParseXML(TiXmlElement* pDoc);

    virtual void onHit(BattleUnit* pAttacker);

private:
    bool    m_IsHit;

};

#endif // ai_condition_onhit_h__
