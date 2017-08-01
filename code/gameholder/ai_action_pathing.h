#ifndef ai_action_pathing_h__
#define ai_action_pathing_h__

#include "ai_action_base.h"

class Checkpoint;

/*
 *	AI行为：沿路径移动
 */
class AIActionPathing : public AIActionBase
{
public:
    AIActionPathing();
    virtual ~AIActionPathing();

    virtual void ParseXML(TiXmlElement* pDoc);
    virtual void Do();

private:
    int32   m_CheckRange;
};

#endif // ai_action_pathing_h__
