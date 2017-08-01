#ifndef ai_action_seek_object_h__
#define ai_action_seek_object_h__

#include "ai_action_base.h"

class BattleUnit;
class AIController;

/*
 *	AI行为：寻找目标
 */
class AIActionSeekObject : public AIActionBase
{
public:
    AIActionSeekObject();
    virtual ~AIActionSeekObject();

    virtual void ParseXML(TiXmlElement* pDoc);
    virtual void Do();

private:
    int32   m_type;
    int32   m_radius;
};

#endif // ai_action_seek_object_h__
