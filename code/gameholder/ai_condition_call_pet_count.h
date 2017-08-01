#ifndef ai_condition_call_pet_count_h__
#define ai_condition_call_pet_count_h__

#include "ai_condition_base.h"


/*
 *	AI条件：召唤宠物数量规则
 */
class AIConditionCallPetCount : public AIConditionBase
{
public:
    AIConditionCallPetCount();
    virtual ~AIConditionCallPetCount();

    virtual void ParseXML(TiXmlElement* pDoc);
    virtual bool IsSatisfy();

private:
    uint32  m_number;                  // 数量
    int32   m_cond ;                   // 规则 (>= or == or<=)
};

#endif // ai_condition_call_pet_count_h__
