#ifndef task_data_achive_h__
#define task_data_achive_h__


#include "task_data_game.h"


// Ã÷∑•¡Ó»ŒŒÒ
class AchiveTask : public GameTask
{
    CnDeclareRTTI
public:
    AchiveTask(const TASK_TEMPLATE* task_template, Player* player);
    virtual ~AchiveTask();

    virtual uint8 GetType() {return TASK_TYPE_ACHIVE;}

    virtual void OnRewards();

    virtual void OnAdd();
};


#endif // task_data_achive_h__
