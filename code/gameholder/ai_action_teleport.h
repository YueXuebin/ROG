#ifndef ai_action_teleport_h__
#define ai_action_teleport_h__

#include "ai_action_base.h"

/*
 *	AI行为：传送
 */
class AIActionTeleport : public AIActionBase
{
public:
    AIActionTeleport();
    virtual ~AIActionTeleport();

    virtual void Do();

private:
    int32 m_type;
};

#endif // ai_action_teleport_h__
