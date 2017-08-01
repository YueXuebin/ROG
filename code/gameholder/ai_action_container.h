#ifndef ai_action_container_h__
#define ai_action_container_h__

#include "ai_container.h"
#include "ai_action_base.h"


class AIController;

/*
 *	AI行为：子AI状态机
 */
class AIActionContainer : public AIActionBase, AIContainer
{
    friend class AIFactory;
public:
    AIActionContainer();
    virtual ~AIActionContainer();

    virtual void ParseXML(TiXmlElement* pDoc);
    virtual void Do();

    virtual void Update(int32 deltaTime);

    virtual AIController* GetController();

private:


};


#endif // ai_action_container_h__
