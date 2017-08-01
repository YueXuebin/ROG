#ifndef move_random_action_h__
#define move_random_action_h__

// Ëæ»úÒÆ¶¯

#include "move_action.h"


class MoveRandomAction : public MoveAction
{
public:
    MoveRandomAction(Robot* pRobot);
    virtual ~MoveRandomAction();

};


#endif // move_random_action_h__
