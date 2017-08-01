/*
 *	修罗场配置
 */
#ifndef ashura_config_h__
#define ashura_config_h__

typedef struct AshRewardList 
{
    void clear()
    {
        m_gamepoint = 0;
        m_soulpoint = 0;
        m_exp       = 0;
        m_reward.clear();
    }

    int32   m_gamepoint;
    int32   m_soulpoint;
    int32   m_exp;
    std::map<uint32, REWARD_TABLE>   m_reward;      // 奖励 <数量, 奖励>
}AshRewardList;

typedef struct AshLevelReward
{
    void clear()
    {
        m_level             = 0;
        m_maxGamePoint      = 0;
        m_maxSoulPoint      = 0;
        m_maxExp            = 0;
        m_killRewardList.clear();
        m_deadRewardList.clear();
    }

    int32 m_level;
    int32 m_maxGamePoint;
    int32 m_maxSoulPoint;
    int32 m_maxExp;
    AshRewardList m_killRewardList;
    AshRewardList m_deadRewardList;
}AshLevelReward;

typedef std::map<int32, AshLevelReward> AshLevelRewardList;

typedef struct AshuraRankReward
{
    void Clear()
    {
        m_rank = 0;
        m_rewardTable.Clear();
    }

    int32 m_rank;
    REWARD_TABLE m_rewardTable;
}AshuraRankReward;

typedef std::map<int32, AshuraRankReward> AshRankRewardList;

class AshuraConfig
{
public:
    AshuraConfig();
    virtual ~AshuraConfig();

    DECLARE_SINGLETON(AshuraConfig)

    bool LoadConfig(const char* path);

    bool LoadAshuraConfig(const char* path);
    bool LoadLevelReward(TiXmlElement* ashuraEle);
    bool LoadKillOrDeadReward(TiXmlElement* levelEle, AshRewardList& ashRewardList);
    bool LoadAttack(TiXmlElement* attackEle);

    bool LoadKillRankConfig(const char* path);
    bool LoadDeadRankConfig(const char* path);

    const AshLevelReward* GetReward(int32 level);
    uint32 GetRefreshTime() {return m_refreshTime;}
    uint32 GetRefreshCount() {return m_refreshCount;}

    const REWARD_TABLE*  GetKillRankReward(int32 rank);
    const REWARD_TABLE*  GetDeadRankReward(int32 rank);
private:
    AshLevelRewardList   m_ashReward;       // 奖励
    uint32 m_refreshTime;                   // 时刻
    uint32 m_refreshCount;                  // 每天领取次数

    int32 m_attackUp;                      // 攻击力提高
    int32 m_attackDown;                      // 攻击力降低

    AshRankRewardList m_killRankReward;
    AshRankRewardList m_deadRankReward;
};


#endif // ashura_config_h__
