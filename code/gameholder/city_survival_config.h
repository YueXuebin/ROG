/*
*    圣光生存战配置
*/
#ifndef city_survival_config_h__
#define city_survival_config_h__

typedef std::map<uint32, REWARD_TABLE> SurvivalRewardMap;

#include "td_config.h"

class CitySurvivalConfig
{
public:
    CitySurvivalConfig();
    ~CitySurvivalConfig();

    DECLARE_SINGLETON(CitySurvivalConfig)

    void Clear();

    bool LoadConfig(const char* path);

    bool LoadBaseRewardConfig(TiXmlElement* tdEle);
    bool LoadRankReward(TiXmlElement* tdEle, TdRankRewardList& rewardList);

    const REWARD_TABLE* GetReward(uint32 wave, uint32 rank);

private:
    //SurvivalRewardMap  m_reward_map;

    TdWaveRewardsList m_baseRewards;
};

#endif // city_survival_config_h__
