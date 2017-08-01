#ifndef ai_condition_timer_h__
#define ai_condition_timer_h__

#include "ai_condition_base.h"

/*
 *	AI条件：计时器
 */
class AIConditionTimer : public AIConditionBase
{
public:
    AIConditionTimer();
    virtual ~AIConditionTimer();

    virtual void ParseXML(TiXmlElement* pDoc);

    virtual void onEnterState();
    virtual bool IsSatisfy();

    virtual void Update(int32 deltaTime);

private:
    void Reset();

private:
    int32   m_Timer;
    int32   m_Time;

    int32   m_MinTime;
    int32   m_MaxTime;
};


#endif // ai_condition_timer_h__
