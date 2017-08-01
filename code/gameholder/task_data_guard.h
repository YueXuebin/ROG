#ifndef task_data_guard_h__
#define task_data_guard_h__


#include "task_data_game.h"


// 护卫任务
class GuardTask : public GameTask
{
    CnDeclareRTTI
public:
    GuardTask(const TASK_TEMPLATE* task_template, Player* player);
    virtual ~GuardTask();

    virtual uint8 GetType() {return TASK_TYPE_GUARD;}

    virtual void OnRewards();

    virtual void OnAdd();
private:
    void SendGuardAchive(uint32 npcid, Player* player);
};



#endif // task_data_guard_h__
