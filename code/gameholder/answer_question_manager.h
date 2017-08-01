#ifndef ANSWER_QUESTION_MANAGER
#define ANSWER_QUESTION_MANAGER

class Player;

// ==================================================
// 答题系统
// ==================================================
class AnswerQuestionManager
{
public:
    AnswerQuestionManager();
    ~AnswerQuestionManager();

    DECLARE_SINGLETON(AnswerQuestionManager);

    enum{
        ANSWER_STATE_END = 0,
        ANSWER_STATE_READY = 1,
        ANSWER_STATE_PROCESS = 2,
        ANSWER_STATE_RANK = 3,
    };

    void Clear();

    void OnRecv(Player* player, uint32 msgID, void* data);

    bool IsPlayerInAnswer(Player* player);

private:
    // 与服务器通信
    void OnAnswerQuestionNtf(void* data);                           // 接受问题
    void OnAnswerResultNtf(void* data);                             // 答题结果通知
    void OnAnswerTopRankReq(Player* player, void* data);            // 答题TOP排行请求
    void OnAnswerTopRankAck(void* data);                            // 答题TOP排行应答（CenterServer发送过来的同步消息）
    void OnAnswerPlayerRankReq(Player* player, void* data);         // 答题个人排名请求
    void OnAnswerPlayerRewardReq(Player* player, void* data);       // 答题个人奖励请求
    void OnAnswerPlayerRewardAck(Player* player, void* data);       // 答题个人奖励应答
    void OnAnswerEndNtf(void* data);                                // 答题结束
    void OnAnswerExit(Player* player);                              // 退出答题

    void SendAnswerScoreSave(Player* player, int32 score);          // 发送积分
    void SendAnswerDoubleScoreAck(Player* player, uint16 errcode);  // 发送答题倍积分应答

    // 与客户端通信
    void OnAnswerReadAck(void* data);                               // 准备答题通知
    void OnAnswerQuestionCmf(Player* player, void* data);           // 回答问题
    void OnBuyDouble(Player* player, void* data);                   // 购买双倍积分

    void SendReadyToPlayer(Player* player);                         // 发送准备答题给刚上线玩家
    void SendReadyToAllPlayers(uint32 readyTime, int32 leftTime);   // 发送准备答题

    uint32 GetActivityID() { return m_activityID; }                 ///< 获得活动ID
    void SetActivityID(uint32 val) { m_activityID = val; }          ///< 设置活动ID

    int32 GetState() { return m_state; }                            ///< 获得状态
    void SetState(int32 val) { m_state = val; }                     ///< 设置状态

    void SendAnswerQuestionNtf(Player* player);                     ///< 发送题目通知

    void CountScore(Player* player, uint8 result);                  // 计算积分
    void SendToJoinPlayer(uint32 msgID,void* data);                 // 发送给参加答题的玩家

private:
    int32           m_state;                                        // 活动状态

    uint32          m_activityID;                                   // 活动ID号
    uint32          m_questionID;                                   // 当前问题
    uint32          m_questionSN;                                   // 当前问题序号

    uint8           m_rightResult;                                  // 当前问题答案
    uint32          m_nextQuestionTime;                             // 下一道题的时间
    uint32          m_openTime;                                     // 开启时间
    uint32          m_readyTime;                                    // 版本时间

    ANSWER_TOPRANK_INFO m_toprank;                                  // 排名信息
};



#endif
