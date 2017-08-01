#ifndef task_data_festive_h__
#define task_data_festive_h__

#include "task_data_target.h"


// 活动任务
class FestiveTask : public TargetTask
{
    CnDeclareRTTI
public:
    FestiveTask(const TASK_TEMPLATE* task_template, Player* player);
    virtual ~FestiveTask();

    virtual uint8 GetType() {return TASK_TYPE_FESTIVAL;}

    virtual void OnAdd();

    virtual bool IsInValidTime();
    virtual bool IsNeedOnlineTime();

    virtual void OnRewards();
};



#endif // task_data_festive_h__
