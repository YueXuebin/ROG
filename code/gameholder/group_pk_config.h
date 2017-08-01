#ifndef group_pk_config_h__
#define group_pk_config_h__

typedef struct GPKkillScore
{
    void Clear()
    {
        num = 0;
        player_score = 0;
        group_score = 0;
    }

    int32 num;
    int32 player_score;
    int32 group_score;
}GPKkillScore;

class GroupPkConfig
{
public:
    GroupPkConfig();
    ~GroupPkConfig();

    DECLARE_SINGLETON(GroupPkConfig)

    bool LoadConfig(const char* path);

    bool LoadScore(TiXmlElement* groupEle);
    bool LoadRewards(TiXmlElement* groupEle);

    const GPKkillScore* GetComboKillScore(int32 combo);
    const GPKkillScore* GetKillTotalScore(int32 killTotal);

    int32 GetScoreTimeSecond() {return m_scoreTimeSecond;}
    int32 GetScoreTimePlayerScore() {return m_scoreTimePlayerScore;}

    int32 GetComboStopNum() {return m_comboStopNum;}
    int32 GetComboStopPlayerScore() {return m_comboStopPlayerScore;}
    int32 GetComboStopGroupScore() {return m_comboStopGroupScore;}

    const REWARD_TABLE* GetRewards(int32 score);
private:
    std::map<int32, GPKkillScore>  m_comboKill; // 连杀积分
    std::map<int32, GPKkillScore>  m_killTotal; // 累积杀人积分

    int32   m_scoreTimeSecond;                  // 每隔几秒
    int32   m_scoreTimePlayerScore;             // 给多少积分

    int32   m_comboStopNum;                     // 阻止多少杀给积分
    int32   m_comboStopPlayerScore;             // 给个人多少积分
    int32   m_comboStopGroupScore;              // 给团队多少积分

    std::map<int32, REWARD_TABLE>   m_rewards;  // 积分, 奖励
};

#endif // group_pk_config_h__
