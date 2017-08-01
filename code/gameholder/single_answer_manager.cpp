#include "gameholder_pch.h"
#include "single_answer_manager.h"
#include "answer_question_config.h"
#include "player.h"
#include "vip_config.h"
#include "singgle_answer_config.h"
#include "item_manager.h"
#include "trade_manager.h"
#include "event_define.h"
#include "gameplaytimes_notifier.h"

SingleAnswerManager::SingleAnswerManager(Player* player) :
m_Player(player)
{
    Clear();

}

SingleAnswerManager::~SingleAnswerManager(void)
{
}
void SingleAnswerManager::Clear()
{
    m_gen_questions.clear();
    m_start_time = 0;
    m_right_num = 0;
}

void SingleAnswerManager::OnRecv(uint32 msgID, void* data)
{
    switch(msgID)
    {
    case MSG_SINGLE_ANSWER_NTF:
        {
            InitSingleAnswer(data);
        }
        break;
    case MSG_SINGLE_ANSWER_QUESTION_REQ:
        {
            QuestionReq(data);
        }
        break;
    case MSG_SINGLE_ANSWER_RESULT_REQ:
        {
            ResultReq(data);
        }
        break;
    case MSG_SINGLE_ANSWER_CD_TIME_REQ:
        {
            TimeCDReq(data);
        }
        break;
    }
}

void SingleAnswerManager::LoadInfo(SINGLE_ANSWER_INFO& info)
{
    m_single_answer_info = info;
}

void SingleAnswerManager::FillInfo(SINGLE_ANSWER_INFO& info)
{
    info = m_single_answer_info;
}

void SingleAnswerManager::NotifyNum()
{
    m_Player->GetNotifyManager()->SetGameplayTimes(NOTIFY_SINGLE_ANSWER, 
        m_single_answer_info.num, VipConfig::Instance()->GetVipValue(VIP_PRIVILEGE_53 ,m_Player->GetVipLevel()));
}

void SingleAnswerManager::CalDailyRefresh()
{
    //int32 maxNum = VipConfig::Instance()->GetVipValue(VIP_PRIVILEGE_53 ,m_Player->GetVipLevel());
    //if(m_Player->GetGainbackResourceManager()->IsSaveMaxCount())
    //    m_Player->SaveYesterdayCount(OPEN_SYS_SINGLE_ANSWER, maxNum);
    //else
    //    m_Player->SaveYesterdayCount(OPEN_SYS_SINGLE_ANSWER, m_single_answer_info.num);

    //m_single_answer_info.num = maxNum;
    //m_single_answer_info.into_answer = 0;
    //m_single_answer_info.change_num = 1;
}

void SingleAnswerManager::ReceiveEvent( uint32 frameCount, uint32 eventId, int32 arg1, int32 arg2, int32 arg3 )
{
    switch(eventId)
    {
    case EVENT_CHANGE_VIP:  //活动完成后
        {
            m_single_answer_info.num =CalHowTimes();
            NotifyNum();
        }
        break;
    }
}
void SingleAnswerManager::InitSingleAnswer(void* data)
{
    Clear();
    AnswerQuestionConfig::Instance()->GenQuestion(SingleAnswerConfig::Instance()->GetQuesitonNum(), m_gen_questions);

    SINGLE_ANSWER_INI_ACK sendMsg;
    sendMsg.num =  CalHowTimes();
    sendMsg.cdtime = CalCDinterval();

    NotifyNum();
    m_Player->SendToGate(MSG_SINGLE_ANSWER_ACK, &sendMsg);
}
void SingleAnswerManager::QuestionReq(void* data)
{
    SINGLE_QUESTION_ACK sendMsg;
    sendMsg.errcode = ERR_FAILED;


    //if(OpenSysConfig::Instance()->IsOpenSys(OPEN_SYS_SINGLE_ANSWER, m_Player) != ERR_SUCCEED)
    //{
    //    m_Player->NotifyErrorCode(ERR_FUNCT_NOT_OPEN);
    //    return;
    //}

    if(m_single_answer_info.num <= 0 || (CalCDinterval() > 0))
    {
        m_Player->SendToGate(MSG_SINGLE_ANSWER_QUESTION_ACK,&sendMsg);
        return;
    }

    if(m_gen_questions.size() <= 0)
         CnWarn("single answer num为0\n");
       // AnswerQuestionConfig::Instance()->GenQuestion(SingleAnswerConfig::Instance()->GetQuesitonNum(), m_gen_questions);
    sendMsg.question_id  = CalQuestionId();
    if(sendMsg.question_id)
        sendMsg.errcode = ERR_SUCCEED;
    m_Player->SendToGate(MSG_SINGLE_ANSWER_QUESTION_ACK,&sendMsg);
}
void SingleAnswerManager::ResultReq(void* data)
{
    //if(OpenSysConfig::Instance()->IsOpenSys(OPEN_SYS_SINGLE_ANSWER, m_Player) != ERR_SUCCEED)
    //{
    //    m_Player->NotifyErrorCode(ERR_FUNCT_NOT_OPEN);
    //    return;
    //}

    SINGLE_ANSWER_RESULT_ACK single_result;
    single_result.errcode = ERR_FAILED;
    SINGLE_RESLUT_REQ* recvMsg = (SINGLE_RESLUT_REQ*)data;
    if(!recvMsg)
        return;
    if((m_questionID == 0) ||(m_single_answer_info.num <= 0) || (m_questionID != recvMsg->question_id) || (CalCDinterval() > 0))
    {
        single_result.question_id = recvMsg->question_id;
        m_Player->SendToGate(MSG_SINGLE_ANSWER_RESULT_ACK, &single_result);
        return;
    }

    //进入过答题
    m_single_answer_info.into_answer = 1;

    uint32 nowtime = (uint32)Crown::SDNow().GetTimeValue();
    uint32 intertime = nowtime - m_start_time;
    if ( ( intertime <=  SingleAnswerConfig::Instance()->GetEveryTime()) && (recvMsg->result == m_right_resultID))
    {
        m_right_num = m_right_num + 1;
        single_result.errcode = ERR_SUCCEED;
    }

    single_result.question_id = CalQuestionId();
    m_Player->SendToGate(MSG_SINGLE_ANSWER_RESULT_ACK, &single_result);

    if(single_result.question_id == 0)
        RewardAck();
}
void SingleAnswerManager::RewardAck()
{
    if(m_gen_questions.size() <= 0 && (m_single_answer_info.into_answer == 1))
    {
        //int32 rewardNum = SingleAnswerConfig::Instance()->CalRewardNum(m_Player->GetLevel(),m_right_num);
       // m_Player->AddSoulPoint(rewardNum, REWARD_REASON_ANSWER);
        REWARD_TABLE tablereward;
        SingleAnswerConfig::Instance()->RewardTableByRightNum(m_Player->GetLevel(),m_right_num, tablereward);
        m_Player->TakeInRewardTable(&tablereward);

        m_right_resultID = 0;
        m_right_num = 0;
        m_single_answer_info.change_num = m_single_answer_info.change_num + 1;
        m_single_answer_info.cd_start_time = (uint32)Crown::SDNow().GetTimeValue();

        SINGLE_ANSWER_INI_ACK sendMsg;
        sendMsg.cdtime = CalCDinterval();
        sendMsg.num =  CalHowTimes();
        m_single_answer_info.num = sendMsg.num;

        NotifyNum();

        m_Player->SendToGate(MSG_SINGLE_ANSWER_REWARD_ACK, &sendMsg);
    }
}

int32 SingleAnswerManager::CalHowTimes()
{
    int32 vipnum = VipConfig::Instance()->GetVipValue(VIP_PRIVILEGE_53 ,m_Player->GetVipLevel()) ;
    if(m_single_answer_info.change_num  == 1)
        return vipnum;
    else if(vipnum >= m_single_answer_info.change_num && (m_single_answer_info.change_num >1))
        return vipnum - (m_single_answer_info.change_num - 1);
    else
        return 0;
}
void SingleAnswerManager::TimeCDReq(void* data)
{
    //if(OpenSysConfig::Instance()->IsOpenSys(OPEN_SYS_SINGLE_ANSWER, m_Player) != ERR_SUCCEED)
    //{
    //    m_Player->NotifyErrorCode(ERR_FUNCT_NOT_OPEN);
    //    return;
    //}

    int32 time = CalCDinterval()/MINUTES_SECONDS;
    int32 howmunch = time * SingleAnswerConfig::Instance()->GetCDMoney();

    SINGLE_ANSWER_CD_TIME_ACK sendMsg;
    if(m_Player->GetMoneyPoint() < howmunch)
    {
        sendMsg.errcode = ERR_NO_ENOUGH_MONEYPOINT;
        m_Player->SendToGate(MSG_SINGLE_ANSWER_CD_TIME_ACK, &sendMsg);
    }

    TradeManager::Instance()->SendTradeOrderReq(m_Player, REWARD_REASON_SINGLE_ANSWER_CD_TIME, PROP_SINGLE_ANSWER_CD_TIME, 0, 0, howmunch);
}
void SingleAnswerManager::TimeCDReq()
{
    m_single_answer_info.cd_start_time = 0;

    SINGLE_ANSWER_CD_TIME_ACK sendMsg;
    sendMsg.cdtime = 0;
    sendMsg.errcode = ERR_SUCCEED;
    m_Player->SendToGate(MSG_SINGLE_ANSWER_CD_TIME_ACK, &sendMsg);
}
uint32 SingleAnswerManager::CalCDinterval()
{
    uint32 interval = 0;
    uint32 _finishitime = (uint32)Crown::SDNow().GetTimeValue();
    uint32 gettimecd = SingleAnswerConfig::Instance()->GetCDTimeByChangeNum(m_single_answer_info.change_num);
    uint32 cdtime = 0;
    if(_finishitime > m_single_answer_info.cd_start_time)
        cdtime = _finishitime - m_single_answer_info.cd_start_time;

    if(cdtime < gettimecd)
        interval =  gettimecd - cdtime; 
    else
        interval = 0;

    return interval;
}
uint32 SingleAnswerManager::CalQuestionId()
{
    uint32 question = 0;
    if(m_gen_questions.size() > 0)
    {
        uint32 question = m_gen_questions.front();
        m_gen_questions.erase(m_gen_questions.begin());

        m_questionID = question;                                                    // 当前问题的ID
        m_right_resultID = AnswerQuestionConfig::Instance()->GetRightResult(question); // 当前问题的答案
        m_start_time = (uint32)Crown::SDNow().GetTimeValue();

    }
    else
        m_questionID = 0;

    return m_questionID;
}