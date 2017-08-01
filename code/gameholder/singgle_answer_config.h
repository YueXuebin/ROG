#ifndef singleaswerconfig_h
#define singleaswerconfig_h
class SingleAnswerConfig
{
public:
    SingleAnswerConfig(void);

    ~SingleAnswerConfig(void);

    DECLARE_SINGLETON(SingleAnswerConfig)

    bool LoadConfig(const char* path);

    int32 CalRewardNum(int32 playerlevel, uint32 rightnum); //���㽱������Ŀ

    uint32 GetCDTimeByChangeNum(int32 changenum);           //������ս�������ҵ�cdʱ��

    uint32 GetEveryTime() {return m_once_time;}

    int32 GetQuesitonNum() {return m_question_num;}

    uint32 GetRewardItemId() {return m_reward_item;}

    int32 GetCDMoney() {return m_cdMoney;}

    //���ݴ����ĸ�������õ��Ľ���
    void RewardTableByRightNum(int32 playerlevel, uint32 rightnum ,REWARD_TABLE& rewardtable);
private:

    void ReadRewards(TiXmlElement* singleEle);

    void ReadResults(TiXmlElement* singleEle);

    void ReadTimes(TiXmlElement* singleEle);

    void ReadAnswerReward(TiXmlElement* rewardEle, REWARD_TABLE& rewardTable);

private:

    uint32 m_once_time;              //ÿ�δ����ʱ��

    int32 m_question_num;           //�������

    uint32 m_reward_item;           //����itemid

    int32 m_cdMoney;                //��cd1���ӻ�ħʯ

    std::map<int32, REWARD_TABLE> m_levelRewardList;      //������ <����-������>
    std::map<uint32, int32> m_rightPecentage;       //��ȷ�ٷֱ� <������������ٷֱ�>
    std::map<int32, uint32> m_numtoCD;              //��սcdʱ�� <������ʱ��>
};
#endif
