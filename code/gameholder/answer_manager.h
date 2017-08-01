#ifndef answer_manager_h__
#define answer_manager_h__

class Player;

/*
 *	�����ʴ�
 */
class AnswerManager
{
public:
    AnswerManager(Player* player);
    ~AnswerManager(void);

    void LoadAnswerInfo(const ANSWER_INFO& info);
    void FillAnswerInfo(ANSWER_INFO& info);

    ANSWER_INFO& GetAnswerInfo() { return m_answer_info; }

    void CalAnswerInfo(uint32 readTime);                // ���ô���ʱ��

    bool IsInAnswer();                                  // �Ƿ����ڴ�����

private:
    Player* m_player;

    ANSWER_INFO             m_answer_info;                          // ������Ϣ

};

#endif // answer_manager_h__
