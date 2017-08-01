#ifndef task_data_main_h__
#define task_data_main_h__

#include "task_data_game.h"


// 主线任务
class MainTask : public GameTask
{
    CnDeclareRTTI
public:
    MainTask(const TASK_TEMPLATE* task_template, Player* player);
    virtual ~MainTask();

    virtual uint8 GetType() {return TASK_TYPE_MAIN;}
    virtual void OnRewards();
};



#endif // task_data_main_h__
