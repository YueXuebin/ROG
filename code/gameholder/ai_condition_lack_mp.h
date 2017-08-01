#ifndef ai_condition_lack_mp_h__
#define ai_condition_lack_mp_h__

#include "ai_condition_base.h"

/*
 *	AI条件：蓝量低于n% 
 */
class AIConditionLackMP : public AIConditionBase
{
public:
    AIConditionLackMP();
    virtual ~AIConditionLackMP();

    virtual void onEnterState();
    virtual void ParseXML(TiXmlElement* pDoc);
    virtual bool IsSatisfy();
private:
    void Reset();

private:
    int32   m_mp;
    int32   m_probability;
    bool    m_random;
};

#endif // ai_condition_lack_mp_h__
