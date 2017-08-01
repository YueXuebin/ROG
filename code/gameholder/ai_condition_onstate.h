#ifndef ai_condition_onstate_h__
#define ai_condition_onstate_h__

#include "ai_condition_base.h"

/*
 *	AI条件：处于某状态
 */
class AIConditionOnState : public AIConditionBase
{
public:
    AIConditionOnState();
    virtual ~AIConditionOnState();

    virtual void ParseXML(TiXmlElement* pDoc);
    virtual bool IsSatisfy();

private:
    uint32  m_InStateId;
};

#endif // ai_condition_onstate_h__
