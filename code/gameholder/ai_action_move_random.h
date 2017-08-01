#ifndef ai_action_move_random_h__
#define ai_action_move_random_h__

#include "ai_action_base.h"

/*
 *	AI行为：朝范围内随机点移动
 */
class AIActionMoveRandom : public AIActionBase
{
public:
    AIActionMoveRandom();
    virtual ~AIActionMoveRandom();

    virtual void ParseXML(TiXmlElement* pDoc);
    virtual void Do();

private:
    bool    m_IsBasePose;       // 是否基于基准位置
    int32   m_MinAngle;
    int32   m_MaxAngle;
    int32   m_MinRadius;
    int32   m_MaxRadius;


};

#endif // ai_action_move_random_h__
