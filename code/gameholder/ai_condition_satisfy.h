#ifndef ai_cond_satisfy_h__
#define ai_cond_satisfy_h__

#include "ai_condition_base.h"

/*
 *	AI条件：永远满足
 */
class AIConditionSatisfy : public AIConditionBase
{
public:
    AIConditionSatisfy();
    virtual ~AIConditionSatisfy();

    virtual void ParseXML(TiXmlElement* pDoc);
    virtual bool IsSatisfy();

private:
    int32 m_pro;
};


#endif // ai_cond_satisfy_h__
