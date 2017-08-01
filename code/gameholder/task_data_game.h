#ifndef task_data_game_h__
#define task_data_game_h__


#include "task_data_target.h"


// 普通任务
class GameTask : public TargetTask
{
    CnDeclareRTTI
public:
    GameTask(const TASK_TEMPLATE* task_template, Player* player);
    virtual ~GameTask() = 0;

    virtual bool IsInValidTime() {return true;}     // 普通任务没有任务时间

    virtual bool IsNeedOnlineTime() {return false;} // 普通任务无需记录在线时长

protected:
    bool RewardMultiple(uint32 multiple, REWARD_TABLE& rewardList, LevelRewardsList* taskList);
};


#endif // task_data_game_h__
