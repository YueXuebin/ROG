/*
*   ªÓ‘æ∂»≈‰÷√
*/
#ifndef liveness_config_h__
#define liveness_config_h__

#include <vector>

typedef struct LivenessTarget
{
    int32 target;
    int32 count;
    int32 liveness;
    int32 sysId;

    void Clear()
    {
        target = 0;
        count = 0;
        liveness = 0;
    }
}LivenessTarget;

typedef struct LivenessReward
{
    int32 index;
    int32 liveness;
    int32 common_reward;

    void Clear()
    {
        index = 0;
        liveness = 0;
        common_reward = 0;
    }
}LivenessReward;

typedef std::map<int32, LivenessTarget> LivenessTargetConfigMap;
typedef std::map<int32, LivenessReward> LivenessRewardConfigMap;

class LivenessConfig
{
public:
    LivenessConfig();
    virtual ~LivenessConfig();

    DECLARE_SINGLETON(LivenessConfig)

    bool LoadConfig(const char* path);
    const LivenessTarget* GetTarget(int32 target);
    int32 GetRewardID(int32 index);
    const LivenessTargetConfigMap* GetTargetMap();
    const LivenessRewardConfigMap* GetRewardMap();

private:
    LivenessTargetConfigMap m_target_map;
    LivenessRewardConfigMap m_reward_map;

};

#endif
