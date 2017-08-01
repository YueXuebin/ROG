#ifndef ai_condition_enemy_exist_h__
#define ai_condition_enemy_exist_h__

#include "ai_condition_base.h"

/*
 *	AI条件：范围内有多少个怪
 */
class AIConditionEnemyExist : public AIConditionBase
{
public:
    AIConditionEnemyExist();
    virtual ~AIConditionEnemyExist();

    virtual void ParseXML(TiXmlElement* pDoc);
    virtual bool IsSatisfy();

private:
    int32   m_radius;
    int32   m_number;
    int32   m_cond;
};

#endif // ai_condition_enemy_exist_h__
