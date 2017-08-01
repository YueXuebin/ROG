#ifndef welcome_reward_config_h_
#define welcome_reward_config_h_

class WelcomeRewardConfig
{
public:
    WelcomeRewardConfig(void);
    ~WelcomeRewardConfig(void);

    DECLARE_SINGLETON(WelcomeRewardConfig) 

    bool LoadConfig(const char* path);

    REWARD_TABLE& GetRewadTable();

private:
    REWARD_TABLE rewardTable;
};
#endif