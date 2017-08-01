/*
 *	�����
 */

#ifndef answer_question_config
#define answer_question_config

typedef struct RankReward
{
    void Clear()
    {
        m_minRank   = 0;
        m_maxRank   = 0;
        m_rewards.Clear();
    }

    uint32          m_minRank;
    uint32          m_maxRank;
    REWARD_TABLE    m_rewards;
}RankReward;

class AnswerQuestionConfig
{
public:
    AnswerQuestionConfig();
    ~AnswerQuestionConfig();

    DECLARE_SINGLETON(AnswerQuestionConfig)

    bool LoadConfig(const char* path);

    uint8 GetRightResult(uint32 questionID);
    uint8 GetTimeScore(uint8 time);
    uint8 GetContinationScore(uint8 num);

    int32 GetBuyTimes() { return m_buy_times; }
    int32 GetDoubleTimes() { return m_double_times; }
    int32 GetBuyTimeMoneyPoint() { return m_buy_times_money_point; }

    const REWARD_TABLE* GetRankReward(uint32 rank);
    int32 GetGenQuestionNum() { return m_gen_question_num; }
    int32 GetSysDoubleTimes() { return m_sys_double_times; }

    //��������
    void GenQuestion(uint32 num, std::vector<uint32>& list);

    //const REWARD_TABLE* GetTopRankReward(int32 rank);

private:
    bool LoadQuestion(TiXmlElement* answerEle);
    bool LoadTimeScore(TiXmlElement* answerEle);
    bool LoadContinuation(TiXmlElement* answerEle);
    bool LoadReward(TiXmlElement* answerEle);

    std::map<uint32, uint8>         m_questionTemple;   // ����ģ��     <����ID, �����>
    std::map<uint8, uint8>          m_timeScore;        // ʱ�����ģ�� <ʱ��, ����>
    std::map<uint8, uint8>          m_continuation;     // ��������     <������, ����>
    std::vector<RankReward>         m_rankReward;       ///< ������

    int32                           m_buy_times;                ///< �������
    int32                           m_double_times;             ///< ˫������
    int32                           m_buy_times_money_point;    ///< ����˫��ʹ�õ�ħʯ��
    int32                           m_gen_question_num;         ///< ���������
    int32                           m_sys_double_times;         ///< ϵͳ���͵Ĵ�����
    //std::map<int32, REWARD_TABLE> m_toprank_reward;     ///< ���н���
};

#endif