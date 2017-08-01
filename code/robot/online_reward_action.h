#ifndef online_reward_action_h__
#define online_reward_action_h__

// 领取在线奖励
#include "action_base.h"

class OnlineRewardAction : public ActionBase
{
public:
    OnlineRewardAction(Robot* pRobot);
    virtual ~OnlineRewardAction();
    virtual void OnStart();

    virtual void OnRecv(uint32 msgID, void* data);

private:

};


#endif // online_reward_action_h__
