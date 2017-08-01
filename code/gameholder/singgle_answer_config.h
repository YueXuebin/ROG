#ifndef singleaswerconfig_h
#define singleaswerconfig_h
class SingleAnswerConfig
{
public:
    SingleAnswerConfig(void);

    ~SingleAnswerConfig(void);

    DECLARE_SINGLETON(SingleAnswerConfig)

    bool LoadConfig(const char* path);

    int32 CalRewardNum(int32 playerlevel, uint32 rightnum); //计算奖励的数目

    uint32 GetCDTimeByChangeNum(int32 changenum);           //根据挑战次数，找到cd时间

    uint32 GetEveryTime() {return m_once_time;}

    int32 GetQuesitonNum() {return m_question_num;}

    uint32 GetRewardItemId() {return m_reward_item;}

    int32 GetCDMoney() {return m_cdMoney;}

    //根据答对题的个数计算得到的奖励
    void RewardTableByRightNum(int32 playerlevel, uint32 rightnum ,REWARD_TABLE& rewardtable);
private:

    void ReadRewards(TiXmlElement* singleEle);

    void ReadResults(TiXmlElement* singleEle);

    void ReadTimes(TiXmlElement* singleEle);

    void ReadAnswerReward(TiXmlElement* rewardEle, REWARD_TABLE& rewardTable);

private:

    uint32 m_once_time;              //每次答题的时间

    int32 m_question_num;           //问题个数

    uint32 m_reward_item;           //奖励itemid

    int32 m_cdMoney;                //秒cd1分钟花魔石

    std::map<int32, REWARD_TABLE> m_levelRewardList;      //级别奖励 <级别-奖励数>
    std::map<uint32, int32> m_rightPecentage;       //正确百分比 <答对数，奖励百分比>
    std::map<int32, uint32> m_numtoCD;              //挑战cd时间 <次数，时间>
};
#endif
