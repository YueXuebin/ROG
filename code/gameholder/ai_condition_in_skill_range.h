#ifndef ai_condition_in_skill_range_h__
#define ai_condition_in_skill_range_h__

#include "ai_condition_base.h"

/*
 *	AI条件：目标在技能范围内外
 */
class AIConditionInSkillRange : public AIConditionBase
{
public:
    AIConditionInSkillRange();
    virtual ~AIConditionInSkillRange();

    virtual void ParseXML(TiXmlElement* pDoc);
    virtual bool IsSatisfy();

private:
    int32   m_interval;
    int32   m_count;
    bool    m_inout;
    int32   m_DefaultSkillId;

};

#endif // ai_condition_in_skill_range_h__
