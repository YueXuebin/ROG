// 夺旗
#ifndef capture_flag_config_h__
#define capture_flag_config_h__

// 旗子奖励积分
class FlagsScore
{
public:
    FlagsScore() {Clear();}
    ~FlagsScore() {}

    void Clear()
    {
        num = 0;
        score = 0;
    }

    int32 num;          // 旗子数
    int32 score;        // 积分
};
typedef std::map<int8, FlagsScore> FlagsScoreList;

typedef std::map<int32, REWARD_TABLE> FlagRankRewardList;           // 排名, 奖励
typedef std::map<int32, FlagRankRewardList> FlagPlayerRewardList;   // 积分, 奖励

class CaptureFlagConfig
{
public:
    CaptureFlagConfig();
    ~CaptureFlagConfig();

    DECLARE_SINGLETON(CaptureFlagConfig)

    bool LoadConfig(const char* path);
    bool LoadGroupScore(TiXmlElement* rootEle);
    bool LoadScoreRewards(TiXmlElement* rootEle);
    bool LoadGroupRewards(TiXmlElement* rootEle);

    uint32  GetAddScoreTime() {return m_time;}
    int32   GetAddGroupScore(int32 num);
    int32   GetKillScore() {return m_killscore;}
    int32   GetCaptureScore() {return m_capture_score;}

    const REWARD_TABLE* GetRankRewardList(int32 score, int32 rank);
    const REWARD_TABLE* GetSuccessRewardList() {return &m_successRewardList;}
    const REWARD_TABLE* GetFalseRewardList() {return &m_falseRewardList;}
private:
    FlagsScoreList  m_flagsScoreList;           // 团队积分列表
    int32           m_time;                     // 多长时间加一次团队积分(毫秒)

    int32           m_killscore;                // 杀人奖励个人积分
    int32           m_capture_score;            // 夺旗奖励个人积分

    FlagPlayerRewardList m_RewardList;          // 个人排名奖励列表

    REWARD_TABLE    m_successRewardList;        // 成功奖励列表
    REWARD_TABLE    m_falseRewardList;          // 失败奖励列表
};

#endif // capture_flag_config_h__
