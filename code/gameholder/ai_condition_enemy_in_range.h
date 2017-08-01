#ifndef ai_condition_enemy_in_range_h__
#define ai_condition_enemy_in_range_h__

#include "ai_condition_base.h"

/*
 *	AI条件：敌人在范围内(如果有把最近的敌人设置为目标)
 */
class AIConditionEnemyInRange : public AIConditionBase
{
public:
    AIConditionEnemyInRange();
    virtual ~AIConditionEnemyInRange();

    virtual void ParseXML(TiXmlElement* pDoc);
    virtual bool IsSatisfy();

private:
    int32   m_Range;
    int32   m_type;
    bool    m_existence;
};

#endif // ai_condition_enemy_in_range_h__
