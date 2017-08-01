#ifndef task_data_target_h__
#define task_data_target_h__

#include "task_data.h"


// 通过目标完成的任务
class TargetTask : public RoleTask
{
    CnDeclareRTTI
public:
    TargetTask(const TASK_TEMPLATE* task_template, Player* player);
    virtual ~TargetTask() = 0;

    virtual void  OnPreRewards();

    virtual bool InitTaskLinkage();

    virtual bool OnTriggerTaskTarget(uint32 targetType, uint32 arg1 = 0, uint32 arg2 = 0);
private:

};


#endif // task_data_target_h__
