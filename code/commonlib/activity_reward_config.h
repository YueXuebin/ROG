
/*
*    活动奖励配置
*/
#ifndef activity_reward_config_h__
#define activity_reward_config_h__

typedef std::map<uint32, REWARD_TABLE> ActivityRewardMap;

class ActivityRewardConfig
{
public:
    ActivityRewardConfig();
    ~ActivityRewardConfig();

    DECLARE_SINGLETON(ActivityRewardConfig)

    void Clear();

    bool LoadConfig(const char* path);

    const REWARD_TABLE* GetReward(uint32 rewardId);

    //bool CanGiveReward(Player* player, uint32 rewardId);

    //bool GiveReward(Player* player, uint32 rewardId, uint32 rewardReason = REWARD_REASON_ACTIVITY_REWARD);

private:
    ActivityRewardMap  m_activity_reward_map;

};

#endif
