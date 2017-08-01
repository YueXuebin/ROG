#ifndef task_data_branch_h__
#define task_data_branch_h__


#include "task_data_game.h"


// 支线任务
class BranchTask : public GameTask
{
    CnDeclareRTTI
public:
    BranchTask(const TASK_TEMPLATE* task_template, Player* player);
    virtual ~BranchTask();

    virtual uint8 GetType() {return TASK_TYPE_BRANCH;}

    virtual void OnRewards();
};



#endif // task_data_branch_h__
