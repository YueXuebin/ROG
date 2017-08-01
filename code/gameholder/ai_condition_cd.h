#ifndef ai_condition_cd_h__
#define ai_condition_cd_h__

#include "ai_condition_base.h"

/*
 *	AI条件：技能在CD中
 */
class AIConditionCD : public AIConditionBase
{
public:
    AIConditionCD();
    virtual ~AIConditionCD();

    virtual void ParseXML(TiXmlElement* pDoc);
    virtual bool IsSatisfy();

private:
    int32   m_skillid;
    int32   m_cd;

};

#endif // ai_condition_cd_h__
