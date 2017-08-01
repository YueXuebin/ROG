/*
*   ¥Ã‚π‹¿Ì∆˜
*/
#ifndef answer_question_manager_h
#define answer_question_manager_h

#include "message_encoder.h"

class AnswerManager
{
public:
    AnswerManager();
    ~AnswerManager();

    DECLARE_SINGLETON(AnswerManager)

   void OnRecv(uint64 player, uint32 sessionID, uint32 msgID, void* data);

private:
    void RecvAnswerScoreSave(uint64 playerID, uint32 sessionID, void* data);
    void RecvAnswerPlayerRankReq(uint64 playerID, uint32 sessionID, void* data);
    void RecvAnswerRewardReq(uint64 playerID, uint32 sessionID, void* data);
    void RecvReadyAnswerAck(uint64 playerID, uint32 sessionID, void* data);
    void RecvAnswerQuestionNtf(uint64 playerID, uint32 sessionID, void* data);
    void RecvAnswerResultNtf(uint64 playerID, uint32 sessionID, void* data);
    void RecvAnswerRefRankNtf(uint64 playerID, uint32 sessionID, void* data);
    void RecvAnswerEndNtf(uint64 playerID, uint32 sessionID, void* data);

    static int32        m_rank;
};


#endif // team_manager_h__
