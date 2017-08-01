#ifndef task_data_daily_h__
#define task_data_daily_h__

#include "task_data_game.h"


// 日常任务
class DailyTask : public GameTask
{
    CnDeclareRTTI
public:
    DailyTask(const TASK_TEMPLATE* task_template, Player* player);
    virtual ~DailyTask();

    virtual uint8 GetType() {return TASK_TYPE_DAILY;}

    virtual void OnRewards();

    virtual void OnAdd();

};



#endif // task_data_daily_h__
