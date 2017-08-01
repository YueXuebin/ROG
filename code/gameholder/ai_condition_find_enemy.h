#ifndef ai_condition_find_enemy_h__
#define ai_condition_find_enemy_h__

#include "ai_condition_base.h"

/*
 *	AI条件：查找范围内指定的敌人(如果有把最近的敌人设置为目标)
 */
class AIConditionFindEnemy : public AIConditionBase
{
public:
    AIConditionFindEnemy();
    virtual ~AIConditionFindEnemy();

    virtual void ParseXML(TiXmlElement* pDoc);
    virtual bool IsSatisfy();

private:
    int32   m_Range;
    int32   m_group;
    std::list<uint32> m_creatureList;
};

#endif // ai_condition_find_enemy_h__
