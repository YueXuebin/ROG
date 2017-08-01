#ifndef answer_manager_h__
#define answer_manager_h__

class Player;

/*
 *	勇者问答
 */
class AnswerManager
{
public:
    AnswerManager(Player* player);
    ~AnswerManager(void);

    void LoadAnswerInfo(const ANSWER_INFO& info);
    void FillAnswerInfo(ANSWER_INFO& info);

    ANSWER_INFO& GetAnswerInfo() { return m_answer_info; }

    void CalAnswerInfo(uint32 readTime);                // 设置答题时间

    bool IsInAnswer();                                  // 是否正在答题中

private:
    Player* m_player;

    ANSWER_INFO             m_answer_info;                          // 答题信息

};

#endif // answer_manager_h__
