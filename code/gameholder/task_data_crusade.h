#ifndef task_data_crusade_h__
#define task_data_crusade_h__


#include "task_data_game.h"


// 讨伐令任务
class CrusadeTask : public GameTask
{
    CnDeclareRTTI
public:
    CrusadeTask(const TASK_TEMPLATE* task_template, Player* player);
    virtual ~CrusadeTask();

    virtual uint8 GetType() {return TASK_TYPE_CRUSADE;}

    virtual void OnRewards();

    virtual void OnAdd();
};


#endif // task_data_crusade_h__
