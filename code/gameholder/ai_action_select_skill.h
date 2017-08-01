#ifndef ai_action_select_skill_h__
#define ai_action_select_skill_h__

#include "ai_action_base.h"

/*
 *	AI行为：选择技能
 */
class AIActionSelectSkill : public AIActionBase
{
public:
    AIActionSelectSkill();
    virtual ~AIActionSelectSkill();

    virtual void ParseXML(TiXmlElement* pDoc);
    virtual void Do();
};


#endif // ai_action_select_skill_h__
