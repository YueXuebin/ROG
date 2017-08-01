#ifndef guild_task_config_h__
#define guild_task_config_h__

#include "task_define.h"

typedef struct RewardAddtionItem
{
    int32   guild_level;
    int32   guild_point;
    int32   score;
    int32   gamepoint;
    int32   exp;

    void    Clear()
    {
        guild_level = 0;
        guild_point = 0;
        score = 0;
        gamepoint = 0;
        exp = 0;
    }
}RewardAddtionItem;

// 公会任务配置表 guild_task.xml
class GuildTaskConfig
{
public:
    GuildTaskConfig();
    ~GuildTaskConfig();

    DECLARE_SINGLETON(GuildTaskConfig)

    bool LoadConfig(const char* path);

public:
    uint32  GetMaxTaskNum() {return m_max_task_num;}
    uint8   GetUseTokenNum() {return m_use_token_num;}
    uint32  GetTokenId() {return m_token_id;}
    uint8   GetCanTaskNum() {return m_can_task_num;}

    //ItemProList  GetQualityPro() {return m_qualityProList;}
    REWARD_TABLE GetRewardAddtion(int32 guildLevel);
    //const std::map<int32, REWARD_TABLE>& getBoxRewardList() {return m_boxRewardList;}
    const REWARD_TABLE* GetBoxReward(int score);
private:
    uint32  m_max_task_num;     // 最大任务数
    uint8   m_use_token_num;    // 消耗公会令数
    uint32  m_token_id;         // 公会令ID
    uint8   m_can_task_num;     // 能接的任务数

    //ItemProList  m_qualityProList;
    std::map<int32, RewardAddtionItem>  m_rewardAddtionList;
    std::map<int32, REWARD_TABLE>       m_boxRewardList;        // 公会宝箱奖励 <积分, 奖励>
};

#endif // guild_task_config_h__
