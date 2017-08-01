#ifndef ai_action_stop_h__
#define ai_action_stop_h__

#include "ai_action_base.h"

/*
 *	AI行为：停止
 */
class AIActionStop : public AIActionBase
{
public:
    AIActionStop();
    virtual ~AIActionStop();

    virtual void Do();
};

#endif // ai_action_stop_h__
