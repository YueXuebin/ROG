#include "gameholder_pch.h"
#include "answer_manager.h"
#include "answer_question_manager.h"


AnswerManager::AnswerManager( Player* player ) :
m_player(player)
{

}

AnswerManager::~AnswerManager()
{


}

void AnswerManager::LoadAnswerInfo(const ANSWER_INFO& info)
{
    m_answer_info = info;
}

void AnswerManager::FillAnswerInfo(ANSWER_INFO& info)
{
    info = m_answer_info;
}

void AnswerManager::CalAnswerInfo( uint32 readTime )
{
    if(m_answer_info.answer_version < readTime)
    {
        m_answer_info.score = 0;
        m_answer_info.question_id = 0;
        m_answer_info.result_id = 0;
        m_answer_info.add_score = 0;
        m_answer_info.score = 0;
        m_answer_info.continuation = 0;
        m_answer_info.use_double_flag = 0;
        m_answer_info.buy_double_num = 0;
        m_answer_info.sys_double_num = 0;
    }

    m_answer_info.answer_version = readTime;
}

bool AnswerManager::IsInAnswer()
{
    return AnswerQuestionManager::Instance()->IsPlayerInAnswer(m_player);
}
