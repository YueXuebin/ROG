#ifndef ai_action_moveto_h__
#define ai_action_moveto_h__

#include "ai_action_base.h"

class BattleUnit;

/*
 *	AI行为：朝目标移动
 */
class AIActionMoveTo : public AIActionBase
{
public:
    AIActionMoveTo();
    virtual ~AIActionMoveTo();

    virtual void ParseXML(TiXmlElement* pDoc);
    virtual void Do();

private:
    void DoMoveTo();
    void MoveToTarget(BattleUnit* pTarget);

private:
    int32 m_type;
};

#endif // ai_action_moveto_h__
