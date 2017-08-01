/*
 * 大冒险表
 */
#ifndef adventure_config_h__
#define adventure_config_h__

typedef struct AdventureTemplate
{
    uint32                      id;             ///< ID
    std::string                 name;           ///< 名字
    int32                       gamepoint;      ///< 游戏币
    uint32                      take_time;      ///< 需要时间
    std::vector<REWARD_ITEM>    reward_list;    ///< 奖励列表
    void Clear()
    {
        id                  = 0;
        name                = "";
        gamepoint           = 0;
        take_time           = 0;
        reward_list.clear();
    }
} AdventureTemplate;

class AdventureConfig
{
public:
    AdventureConfig();
    ~AdventureConfig();

    DECLARE_SINGLETON(AdventureConfig)

    bool LoadConfig(const char* path);

    uint32 GetRefreshTime() { return m_refresh_time; }

    int32 GetMaxAdventureTimes() { return m_max_adventure_times; }

    int32 GetMaxRobTimes() { return m_max_rob_times; }

    const AdventureTemplate*  GetAdventureTemplate(uint32 adventureID);

private:
    uint32                              m_refresh_time;         ///< 每日刷新时间
    int32                               m_max_adventure_times;  ///< 最大冒险次数
    int32                               m_max_rob_times;        ///< 最大打劫次数
    std::map<uint32, AdventureTemplate> m_adventures;           ///< 打劫地图
};

#endif // adventure_config_h__
