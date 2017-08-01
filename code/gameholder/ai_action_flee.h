#ifndef ai_action_flee_h__
#define ai_action_flee_h__

#include "ai_action_base.h"

/*
 *	AI行为：逃跑
 */
class AIActionFlee : public AIActionBase
{
public:
    AIActionFlee();
    virtual ~AIActionFlee();

    virtual void ParseXML(TiXmlElement* pDoc);
    virtual void Do();

private:
    int32   m_range;        // 最大距离
    int32   m_range_min;    // 最小距离
};

#endif // ai_action_flee_h__
