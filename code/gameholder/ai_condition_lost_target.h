#ifndef ai_condition_lost_target_h__
#define ai_condition_lost_target_h__

#include "ai_condition_base.h"

/*
 *	AI条件：丢失目标
 */
class AIConditionLostTarget : public AIConditionBase
{
public:
    AIConditionLostTarget();
    virtual ~AIConditionLostTarget();

    virtual void ParseXML(TiXmlElement* pDoc);
    virtual bool IsSatisfy();

private:
    int32   m_range;
};

#endif // ai_condition_lost_target_h__
