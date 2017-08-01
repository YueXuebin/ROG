#ifndef mulserver_battle_config_h__
#define mulserver_battle_config_h__

class MultserverConfig
{
public:
    MultserverConfig();
    ~MultserverConfig();

    DECLARE_SINGLETON(MultserverConfig)

    bool LoadConfig(const char* fileName);

    int32 GetRewardByRank(int32 rank);
private:
    std::map<int32, int32> m_rewardMap;
};

#endif // mulserver_battle_config_h__