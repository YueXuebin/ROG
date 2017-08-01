/*
*    通用奖励配置,各模块都可用
*/
#ifndef common_reward_config_h__
#define common_reward_config_h__

typedef std::map<uint32, REWARD_TABLE> CommonRewardMap;

class CommonRewardConfig
{
public:
    CommonRewardConfig();
    ~CommonRewardConfig();

    DECLARE_SINGLETON(CommonRewardConfig)

    void Clear();

    bool LoadConfig(const char* path);

    const REWARD_TABLE* GetReward(uint32 rewardId);

private:
    CommonRewardMap  m_common_reward_map;

};

#endif
