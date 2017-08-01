#include "centerserver_pch.h"
#include "answer_question_manager.h"
#include "player_job.h"
#include "game_handler.h"
#include "player.h"
#include "player_manager.h"

IMPLEMENT_SINGLETON(AnswerManager)
int32 AnswerManager::m_rank = 0;

AnswerManager::AnswerManager()
{
    m_rank = 0;
}

AnswerManager::~AnswerManager()
{

}

// ==================================================
// 接收网络消息
// ==================================================
void AnswerManager::OnRecv(uint64 playerID, uint32 sessionID, uint32 msgID, void* data)
{
    switch(msgID)
    {
    case MSG_ANSWER_SCORE_SAVE_NTF:
        RecvAnswerScoreSave(playerID, sessionID, data);
        break;
    case MSG_ANSWER_PLAYER_RANK_REQ:
        RecvAnswerPlayerRankReq(playerID, sessionID, data);
        break;
    case MSG_ANSWER_REWARD_REQ:
        RecvAnswerRewardReq(playerID, sessionID, data);
        break;
    case MSG_ANSWER_READY_ACK:
        RecvReadyAnswerAck(playerID, sessionID, data);
        break;
    case MSG_ANSWER_QUESTION_NTF:
        RecvAnswerQuestionNtf(playerID, sessionID, data);
        break;
    case MSG_ANSWER_RESULT_NTF:
        RecvAnswerResultNtf(playerID, sessionID, data);
        break;
    case MSG_ANSWER_GEN_RANK_NTF:
        RecvAnswerRefRankNtf(playerID, sessionID, data);
        break;
    case MSG_ANSWER_END_NTF:
        RecvAnswerEndNtf(playerID, sessionID, data);
        break;
    default:
        CnAssert(false);
        break;
    }
}

// ==================================================
// 接收积分表
// ==================================================
void AnswerManager::RecvAnswerScoreSave( uint64 playerID, uint32 sessionID, void* data )
{
    ANSWER_SCORE_SAVE* recvMsg = (ANSWER_SCORE_SAVE*)data;
    Player* player = nullptr;

    if(!recvMsg) return;

    player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
    if(!player)
    {
        return;
    }

    PlayerJob::Instance()->SaveAnswerScore(playerID, player->GetName(), recvMsg->score);
}

// ==================================================
// 请求答题个人排行
// ==================================================
void AnswerManager::RecvAnswerPlayerRankReq(uint64 playerID, uint32 sessionID, void* data)
{
    PlayerJob::Instance()->GetAnswerPlayerRank(playerID, MSG_ANSWER_PLAYER_RANK_REQ);
}

// ==================================================
// 请求答题个人排行
// ==================================================
void AnswerManager::RecvAnswerRewardReq(uint64 playerID, uint32 sessionID, void* data)
{
    PlayerJob::Instance()->GetAnswerPlayerRank(playerID, MSG_ANSWER_REWARD_REQ);
}

void AnswerManager::RecvReadyAnswerAck(uint64 playerID, uint32 sessionID, void* data)
{
    ANSWER_READY_ACK* recvMsg = (ANSWER_READY_ACK*)data;

    if(!recvMsg) return;

    GameHandler::Instance()->SendToAll(MSG_ANSWER_READY_ACK, data);
}

void AnswerManager::RecvAnswerQuestionNtf(uint64 playerID, uint32 sessionID, void* data)
{
    ANSWER_QUESTION_NTF* recvMsg = (ANSWER_QUESTION_NTF*)data;

    if(!recvMsg) return;

    GameHandler::Instance()->SendToAll(MSG_ANSWER_QUESTION_NTF, data);
}

void AnswerManager::RecvAnswerResultNtf(uint64 playerID, uint32 sessionID, void* data)
{
    ANSWER_RESULT_NTF* recvMsg = (ANSWER_RESULT_NTF*)data;

    if(!recvMsg) return;

    GameHandler::Instance()->SendToAll(MSG_ANSWER_RESULT_NTF, data);
}

void AnswerManager::RecvAnswerRefRankNtf(uint64 playerID, uint32 sessionID, void* data)
{
    ANSWER_GEN_RANK_NTF* recvMsg = (ANSWER_GEN_RANK_NTF*)data;

    if(!recvMsg) return;

    PlayerJob::Instance()->GetAnswerTopRank(recvMsg->isend);
}

void AnswerManager::RecvAnswerEndNtf(uint64 playerID, uint32 sessionID, void* data)
{
    ANSWER_END_NTF* recvMsg = (ANSWER_END_NTF*)data;

    if(!recvMsg) return;

    GameHandler::Instance()->SendToAll(MSG_ANSWER_END_NTF, recvMsg);
}
