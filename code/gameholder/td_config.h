#ifndef td_config_h__
#define td_config_h__

typedef struct TdRankReward 
{
    void Clear()
    {
        id = 0;
        rewardTable.Clear();
    }

    int32 id;
    REWARD_TABLE rewardTable;
}TdRankReward;
typedef std::map<int32, TdRankReward> TdRankRewardList;
typedef std::map<int32, TdRankRewardList> TdWaveRewardsList;

class TdConfig
{
public:
    TdConfig();
    ~TdConfig();

    DECLARE_SINGLETON(TdConfig)

    bool LoadConfig(const char* path);

    bool LoadBaseRewardConfig(TiXmlElement* tdEle);
    bool LoadRankReward(TiXmlElement* tdEle, TdRankRewardList& rewardList);

    int32 GetStartTime() {return m_startTime;}

    const REWARD_TABLE* GetRewards(uint32 wave, int32 rank);

private:
    int32 m_startTime;

    TdWaveRewardsList m_baseRewards;
};

#endif // td_config_h__
