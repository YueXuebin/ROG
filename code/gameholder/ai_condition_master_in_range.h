#ifndef ai_condition_master_in_range_h__
#define ai_condition_master_in_range_h__

#include "ai_condition_base.h"

/*
 *	AI条件：主人是否在范围内
 */
class AIConditionMasterInRange : public AIConditionBase
{
public:
    AIConditionMasterInRange();
    virtual ~AIConditionMasterInRange();

    virtual void ParseXML(TiXmlElement* pDoc);
    virtual bool IsSatisfy();

private:
    int32   m_Range;
    bool    m_existence;
};

#endif // ai_condition_master_in_range_h__
