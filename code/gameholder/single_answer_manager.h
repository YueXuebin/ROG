#ifndef single_answer_manager_h
#define single_answer_manager_h


class Player;

class SingleAnswerManager
{
public:
    SingleAnswerManager(Player* player);
    ~SingleAnswerManager(void);

    void Clear();

    void OnRecv(uint32 msgID, void* data);

    void ReceiveEvent( uint32 frameCount, uint32 eventId, int32 arg1, int32 arg2, int32 arg3 );

    void LoadInfo(SINGLE_ANSWER_INFO& info);

    void FillInfo(SINGLE_ANSWER_INFO& info);

    void CalDailyRefresh();

    void TimeCDReq();

    void NotifyNum();

private:

    void InitSingleAnswer(void* data);

    void QuestionReq(void* data);

    void ResultReq(void* data);

    void TimeCDReq(void* data);

    void RewardAck();

    uint32 CalCDinterval();                        //计算cd的时间差

    uint32 CalQuestionId();                        //计算问题id

    int32 CalHowTimes();                           //计算多少次
private:

    Player* m_Player;

    SINGLE_ANSWER_INFO m_single_answer_info;

    uint32 m_start_time;                            //记录开始答题

    uint32 m_questionID;                            //开始答题的id
    uint32 m_right_resultID;                        //问题答案
    int32 m_right_num;

    std::vector<uint32> m_gen_questions;            //生成问题
};
#endif
