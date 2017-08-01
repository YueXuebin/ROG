/*
 *	勇者争霸奖励
 */
#ifndef player_door_reward_config_h__
#define player_door_reward_config_h__

typedef struct PlayerDoorRewardTemplate
{
    uint32                  rank;               ///< 排行
    uint32                  honor_point;        ///< 荣誉
    uint32                  game_point;         ///< 金钱
    uint32                  soul_point;         ///< 精魂
    std::vector<REWARD_ITEM>        items;         ///< 奖励物品

    void Clear()
    {
        rank                = 0;
        honor_point         = 0;
        game_point          = 0;
        soul_point          = 0;
        items.clear();
    }
} PlayerDoorRewardTemplate;

class PlayerDoorRewardConfig
{
public:
    PlayerDoorRewardConfig();
    ~PlayerDoorRewardConfig();

    DECLARE_SINGLETON(PlayerDoorRewardConfig)

    bool LoadConfig(const char* path);

    const PlayerDoorRewardTemplate*  GetPlayerDoorRewardTemplate(uint32 rank);

    uint32 GetHonorPointByRank(uint32 rank);
    uint32 GetGamePointByRank(uint32 rank);
    uint32 GetSoulPointByRank(uint32 rank);

private:
    std::map<uint32, PlayerDoorRewardTemplate> m_playerDoorRewards;
};

#endif // player_door_reward_config_h__
