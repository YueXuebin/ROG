#include "gameholder_pch.h"
#include "investment_plan_manager.h"
#include "player.h"
#include "day_investment_config.h"
#include "trade_manager.h"
#include "vip_config.h"
#include "day_investment_manager.h"
#include "level_investment_manager.h"
#include "game_setting_manager.h"
#include "task_festive_config.h"

InvestmentPlanManager::InvestmentPlanManager(Player* player):
m_player(player)
{

}

InvestmentPlanManager::~InvestmentPlanManager()
{

}

void InvestmentPlanManager::LoadInfo(const INVESTMENT_PLAN_INFO& info)
{
    m_investmentPlanInfo = info;
}

void InvestmentPlanManager::FillInfo(INVESTMENT_PLAN_INFO& info)
{
    info = m_investmentPlanInfo;
}

void InvestmentPlanManager::OnRecv(uint32 msgID, void * data)
{
    switch(msgID)
    {
    case MSG_INVESTMENT_PLAN_INFO_REQ:
        onInvestmentPlanInfoReq();
        break;
    case MSG_INVESTMENT_PLAN_INVESTMENT_REQ:
        onInvestmentReq(data);
        break;
    case MSG_INVESTMENT_PLAN_ADDITIONAL_REQ:
        onAdditionalReq(data);
        break;
    case MSG_INVESTMENT_PLAN_REWARD_REQ:
        onRewardReq(data);
        break;
    case MSG_INVESTMENT_PLAN_REWARD_ALL_REQ:
        onRewardAllReq();
        break;
    }
}

void InvestmentPlanManager::onInvestmentPlanInfoReq()
{
    // 初始化投资理财信息
    const std::map<int32, InvestmentPlanType>& planMapTmpl = DayInvestmentConfig::Instance()->GetInvestmentPlanMap();
    for(std::map<int32, InvestmentPlanType>::const_iterator planIt = planMapTmpl.begin(); planIt != planMapTmpl.end(); ++planIt)
    {
        INVESTMENT_PLAN_TYPE_INFO* onePlan = GetInvestmentOnePlan(planIt->first);
        if(onePlan == NULL)
        {
            INVESTMENT_PLAN_TYPE_INFO newPlan;
            newPlan.id = planIt->first;
            newPlan.investment_time = 0;

            for(InvestmentPlanDayMap::const_iterator dayTmpIt = planIt->second.dayMap.begin(); dayTmpIt != planIt->second.dayMap.end(); ++dayTmpIt)
            {
                INVESTMENT_PLAN_DAY_STATE dayState;
                dayState.day = dayTmpIt->first;
                dayState.state = NO_REWARD;
                dayState.moneypoint = 0;
                newPlan.day_info_list.push_back(dayState);
            }

            m_investmentPlanInfo.type_info_list.push_back(newPlan);
        }
    }

    CheckState();

    m_player->SendToGate(MSG_INVESTMENT_PLAN_INFO_ACK, &m_investmentPlanInfo);
}

void InvestmentPlanManager::onInvestmentReq(void* data)
{
    INVESTMENT_PLAN_INVESTMENT_REQ* recvMsg = (INVESTMENT_PLAN_INVESTMENT_REQ*)data;

    INVESTMENT_PLAN_TYPE_INFO* pPlan = GetInvestmentOnePlan(recvMsg->id);
    if(!pPlan)
        return;

    const InvestmentPlanType* planTmp = DayInvestmentConfig::Instance()->GetInvestmentPlanTmp(recvMsg->id);
    if(!planTmp)
        return;

    const TASK_FESTIVAL_TEMPLATE* festivalTmpl = TaskFestiveConfig::Instance()->GetFestivalTmpl(m_player->GetCenterSvrID(), FESTIVAL_INVESTMENT_PLAN_ID);
    if(!festivalTmpl)
    {
        sendInvestmentAck(ERR_ACTIVITY_NO_OPEN);
        return;
    }

    uint32 nowTime = (uint32)Crown::SDNow().GetTimeValue();
    if(nowTime < festivalTmpl->begin_time || nowTime > festivalTmpl->end_time)
    {
        sendInvestmentAck(ERR_ACTIVITY_NO_OPEN);
        return;
    }

    int32 vipNum = VipConfig::Instance()->GetVipValue(planTmp->vipPrivilege, m_player->GetVipLevel());
    if(vipNum == 0)
    {
        sendInvestmentAck(ERR_NO_ENOUGH_VIP);
        return;
    }

    if(pPlan->investment_time != 0)     // 投资过了
    {
        sendInvestmentAck(ERR_FAILED);
        return;
    }

    InvestmentPlanDayMap::const_iterator dayIt = planTmp->dayMap.begin();
    if(dayIt == planTmp->dayMap.end())
        return;

    std::map<uint32, REWARD_TABLE>::const_iterator rewardTmpIt = dayIt->second.moneypointRewardMap.find(recvMsg->moneypoint);
    if(rewardTmpIt == dayIt->second.moneypointRewardMap.end())          // 没有这个魔石的投资
    {
        sendInvestmentAck(ERR_FAILED);
        return;
    }

    if(m_player->GetMoneyPoint() < (int32)recvMsg->moneypoint)
    {
        sendInvestmentAck(ERR_NO_ENOUGH_MONEYPOINT);
        return;
    }

    std::string str = "";
    str += Crown::ToString(recvMsg->id);
    str += "|";
    str += Crown::ToString(recvMsg->moneypoint);
    TradeManager::Instance()->SendTradeOrderReq(m_player, REWARD_REASON_INVESTMENT_PLAN, 0, 0, 0, recvMsg->moneypoint, str);
}

void InvestmentPlanManager::CBInvestment(std::string str)
{
    std::vector<std::string> strList = Crown::SDSplitStrings(str, '|');
    if(strList.size() != 2)
        return;

    uint32 id = Crown::SDAtou(strList[0].c_str());
    uint32 moneypoint = Crown::SDAtou(strList[1].c_str());

    INVESTMENT_PLAN_TYPE_INFO* pPlan = GetInvestmentOnePlan(id);
    if(!pPlan)
        return;

    pPlan->investment_time = (uint32)Crown::SDNow().GetTimeValue();
    pPlan->cur_moneypoint = moneypoint;

    for(std::vector<INVESTMENT_PLAN_DAY_STATE>::iterator dayIt = pPlan->day_info_list.begin(); dayIt != pPlan->day_info_list.end(); ++dayIt)
    {
        dayIt->moneypoint = moneypoint;
    }
    CheckState();

    sendInvestmentAck(ERR_SUCCEED);

    m_player->SendToGate(MSG_INVESTMENT_PLAN_INFO_ACK, &m_investmentPlanInfo);
}

void InvestmentPlanManager::onAdditionalReq(void* data)
{
    INVESTMENT_PLAN_ADDITIONAL_REQ* recvMsg = (INVESTMENT_PLAN_ADDITIONAL_REQ*)data;

    INVESTMENT_PLAN_TYPE_INFO* pPlan = GetInvestmentOnePlan(recvMsg->id);
    if(!pPlan)
        return;

    const InvestmentPlanType* planTmp = DayInvestmentConfig::Instance()->GetInvestmentPlanTmp(recvMsg->id);
    if(!planTmp)
        return;

    if(pPlan->investment_time == 0)     // 没投资过了 不能追加
        return;

    int32 dayNum = m_player->GetDaysBetweenDates(Crown::SDNow().GetTimeValue(), pPlan->investment_time) + 1;
    if(dayNum > (int32)planTmp->additional_overdue_day)
    {
        sendAdditionalAck(ERR_INVESTMENT_PLAN_ADDITIONAL_TIME_OVER);
        return;
    }

    if(recvMsg->moneypoint < pPlan->cur_moneypoint)
        return;

    int32 vipNum = VipConfig::Instance()->GetVipValue(planTmp->vipPrivilege, m_player->GetVipLevel());
    if(vipNum == 0)
    {
        sendAdditionalAck(ERR_NO_ENOUGH_VIP);
        return;
    }

    InvestmentPlanDayMap::const_iterator dayIt = planTmp->dayMap.begin();
    if(dayIt == planTmp->dayMap.end())
        return;

    std::map<uint32, REWARD_TABLE>::const_iterator rewardTmpIt = dayIt->second.moneypointRewardMap.find(recvMsg->moneypoint);
    if(rewardTmpIt == dayIt->second.moneypointRewardMap.end())          // 没有这个魔石的投资
    {
        sendAdditionalAck(ERR_FAILED);
        return;
    }

    if(m_player->GetMoneyPoint() < (int32)(recvMsg->moneypoint - pPlan->cur_moneypoint))
    {
        sendAdditionalAck(ERR_NO_ENOUGH_MONEYPOINT);
        return;
    }

    std::string str = "";
    str += Crown::ToString(recvMsg->id);
    str += "|";
    str += Crown::ToString(recvMsg->moneypoint);
    str += "|";
    str += Crown::ToString(dayNum);
    TradeManager::Instance()->SendTradeOrderReq(m_player, REWARD_REASON_INVESTMENT_PLAN_ADDITIONAL, 0, 0, 0, recvMsg->moneypoint - pPlan->cur_moneypoint, str);
}

void InvestmentPlanManager::CBAdditional(std::string str)
{
    std::vector<std::string> strList = Crown::SDSplitStrings(str, '|');
    if(strList.size() != 3)
        return;

    uint32 id = Crown::SDAtou(strList[0].c_str());
    uint32 moneypoint = Crown::SDAtou(strList[1].c_str());
    uint32 dayNum = Crown::SDAtou(strList[2].c_str());

    INVESTMENT_PLAN_TYPE_INFO* pPlan = GetInvestmentOnePlan(id);
    if(!pPlan)
        return;

    pPlan->cur_moneypoint = moneypoint;

    for(std::vector<INVESTMENT_PLAN_DAY_STATE>::iterator dayIt = pPlan->day_info_list.begin(); dayIt != pPlan->day_info_list.end(); ++dayIt)
    {
        if(dayIt->day >= dayNum && dayIt->state != ACCEPTED_REWARD)
            dayIt->moneypoint = moneypoint;
    }
    CheckState();

    sendAdditionalAck(ERR_SUCCEED);

    m_player->SendToGate(MSG_INVESTMENT_PLAN_INFO_ACK, &m_investmentPlanInfo);
}

void InvestmentPlanManager::onRewardReq(void* data)
{
    INVESTMENT_PLAN_REWARD_REQ* recvMsg = (INVESTMENT_PLAN_REWARD_REQ*)data;

    INVESTMENT_PLAN_TYPE_INFO* pPlan = GetInvestmentOnePlan(recvMsg->id);
    if(!pPlan)
        return;

    const InvestmentPlanType* planTmp = DayInvestmentConfig::Instance()->GetInvestmentPlanTmp(recvMsg->id);
    if(!planTmp)
        return;

    if(pPlan->investment_time == 0)     // 没投资过了 不可能能领奖励
    {
        CnAssert(false);
        return;
    }

    std::vector<INVESTMENT_PLAN_DAY_STATE>::iterator dayIt = pPlan->day_info_list.begin();
    for(; dayIt != pPlan->day_info_list.end(); ++dayIt)
    {
        if (dayIt->day == recvMsg->day)
            break;
    }

    if(dayIt == pPlan->day_info_list.end())
    {
        sendRewardAck(ERR_FAILED);
        return;
    }

    if(dayIt->state != CAN_ACCEPT_REWARD)
    {
        sendRewardAck(ERR_FAILED);
        return;
    }
    
    InvestmentPlanDayMap::const_iterator dayItTmp = planTmp->dayMap.find(recvMsg->day);
    if(dayItTmp == planTmp->dayMap.end())
    {
        sendRewardAck(ERR_FAILED);
        return;
    }

    std::map<uint32, REWARD_TABLE>::const_iterator rewardIt = dayItTmp->second.moneypointRewardMap.find(dayIt->moneypoint);
    if(rewardIt == dayItTmp->second.moneypointRewardMap.end())
    {
        sendRewardAck(ERR_FAILED);
        return;
    }

    dayIt->state = ACCEPTED_REWARD;

    m_player->TakeInRewardTable(&rewardIt->second);

    sendRewardAck(ERR_SUCCEED);
    CheckState();

    m_player->SendToGate(MSG_INVESTMENT_PLAN_INFO_ACK, &m_investmentPlanInfo);
}

void InvestmentPlanManager::onRewardAllReq()
{
    CheckState();

    REWARD_TABLE rewardTable;

    // 投资计划全部领取
    for(std::vector<INVESTMENT_PLAN_TYPE_INFO>::iterator planIt = m_investmentPlanInfo.type_info_list.begin(); planIt != m_investmentPlanInfo.type_info_list.end(); ++planIt)
    {
        if(planIt->investment_time == 0)
            continue;

        const InvestmentPlanType* planTmp = DayInvestmentConfig::Instance()->GetInvestmentPlanTmp(planIt->id);
        if(!planTmp)
            return;

        for(std::vector<INVESTMENT_PLAN_DAY_STATE>::iterator dayIt = planIt->day_info_list.begin(); dayIt != planIt->day_info_list.end(); ++dayIt)
        {
            if(dayIt->state != CAN_ACCEPT_REWARD)
                continue;

            InvestmentPlanDayMap::const_iterator dayItTmp = planTmp->dayMap.find(dayIt->day);
            if(dayItTmp == planTmp->dayMap.end())
                continue;

            std::map<uint32, REWARD_TABLE>::const_iterator rewardIt = dayItTmp->second.moneypointRewardMap.find(dayIt->moneypoint);
            if(rewardIt == dayItTmp->second.moneypointRewardMap.end())
                continue;

            dayIt->state = ACCEPTED_REWARD;

            rewardTable.items.insert(rewardTable.items.end(), rewardIt->second.items.begin(), rewardIt->second.items.end());
        }
    }

    m_player->TakeInRewardTable(&rewardTable);
    CheckState();
    m_player->SendToGate(MSG_INVESTMENT_PLAN_INFO_ACK, &m_investmentPlanInfo);

    // 30天投资全部领取
    m_player->GetDayInvestmentManager()->GetAllReward();

    // 等级投资全部领取
    m_player->GetLevelInvestmentManager()->GetAllReward();
}

void InvestmentPlanManager::sendInvestmentAck(uint32 errCode)
{
    INVESTMENT_PLAN_INVESTMENT_ACK sendMsg;
    sendMsg.err_code = errCode;
    m_player->SendToGate(MSG_INVESTMENT_PLAN_INVESTMENT_ACK, &sendMsg);
}

void InvestmentPlanManager::sendAdditionalAck(uint32 errCode)
{
    INVESTMENT_PLAN_ADDITIONAL_ACK sendMsg;
    sendMsg.err_code = errCode;
    m_player->SendToGate(MSG_INVESTMENT_PLAN_ADDITIONAL_ACK, &sendMsg);
}

void InvestmentPlanManager::sendRewardAck(uint32 errCode)
{
    INVESTMENT_PLAN_REWARD_ACK sendMsg;
    sendMsg.err_code = errCode;
    m_player->SendToGate(MSG_INVESTMENT_PLAN_REWARD_ACK, &sendMsg);
}

INVESTMENT_PLAN_TYPE_INFO* InvestmentPlanManager::GetInvestmentOnePlan(uint32 id)
{
    for(std::vector<INVESTMENT_PLAN_TYPE_INFO>::iterator typeInfoIt = m_investmentPlanInfo.type_info_list.begin(); typeInfoIt != m_investmentPlanInfo.type_info_list.end(); ++typeInfoIt)
    {
        if(typeInfoIt->id == id)
            return &(*typeInfoIt);
    }

    return NULL;
}

void InvestmentPlanManager::CheckState()
{
    for(std::vector<INVESTMENT_PLAN_TYPE_INFO>::iterator planIt = m_investmentPlanInfo.type_info_list.begin(); planIt != m_investmentPlanInfo.type_info_list.end(); ++planIt)
    {
        if(planIt->investment_time == 0)
            continue;

        uint32 nowTime = (uint32)Crown::SDNow().GetTimeValue();
        uint32 rewardDayNum = m_player->GetDaysBetweenDates(nowTime, planIt->investment_time) + 1;      // 第一天就可以领取了

        for(std::vector<INVESTMENT_PLAN_DAY_STATE>::iterator dayIt = planIt->day_info_list.begin(); dayIt != planIt->day_info_list.end(); ++dayIt)
        {
            if(dayIt->state != NO_REWARD)
                continue;

            if(dayIt->day > rewardDayNum)
                continue;

            dayIt->state = CAN_ACCEPT_REWARD;
        }
    }

    // 所有奖励都领过就重置状态
    for(std::vector<INVESTMENT_PLAN_TYPE_INFO>::iterator planIt = m_investmentPlanInfo.type_info_list.begin(); planIt != m_investmentPlanInfo.type_info_list.end(); ++planIt)
    {
        if(planIt->investment_time == 0)
            continue;

        std::vector<INVESTMENT_PLAN_DAY_STATE>::iterator dayIt = planIt->day_info_list.begin();
        for(; dayIt != planIt->day_info_list.end(); ++dayIt)
        {
            if(dayIt->state != ACCEPTED_REWARD)
                break;
        }
        
        if (dayIt != planIt->day_info_list.end())
            break;

        planIt->investment_time = 0;
        planIt->cur_moneypoint  = 0;
        for(dayIt = planIt->day_info_list.begin(); dayIt != planIt->day_info_list.end(); ++dayIt)
        {
            dayIt->state = NO_REWARD;
            dayIt->moneypoint = 0;
        }
    }
}
