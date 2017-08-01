#ifndef ai_condition_lack_hp_h__
#define ai_condition_lack_hp_h__

#include "ai_condition_base.h"

/*
 *	AI条件：血量低于n% 
 */
class AIConditionLackHP : public AIConditionBase
{
public:
    AIConditionLackHP();
    virtual ~AIConditionLackHP();

    virtual void onEnterState();
    virtual void ParseXML(TiXmlElement* pDoc);
    virtual bool IsSatisfy();
private:
    void Reset();

private:
    int32   m_hp;
    int32   m_probability;
    bool    m_random;
};

#endif // ai_condition_lack_hp_h__
