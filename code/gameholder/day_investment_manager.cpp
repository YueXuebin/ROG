#include "gameholder_pch.h"
#include "player.h"
#include "day_investment_manager.h"
#include "day_investment_config.h"
#include "trade_manager.h"
#include "investment_manager.h"
#include "game_util.h"
#include "gameplaytimes_notifier.h"
#include "item_manager.h"
#include "mail_manager.h"
#include "mail_title_config.h"
DayInvestmentManager::DayInvestmentManager( Player* player ) : m_player(player)
{
    Clear();
}

DayInvestmentManager::~DayInvestmentManager()
{

}

void DayInvestmentManager::Clear()
{
    m_waiting_invest_trade_call_back = false;

    m_invest_begin_time = 0;
    m_day_investment_map.clear();

    int32 invest_total_day = DayInvestmentConfig::Instance()->GetInvestmentTotalDay();
    for (int32 i=0; i<=invest_total_day; i++)
    {
        m_day_investment_map.insert(std::make_pair(i, NO_REWARD));
    }
}

void DayInvestmentManager::LoadDayInvestmentInfo(DAY_INVESTMENT_INFO& day_investment_info)
{
    m_invest_begin_time = day_investment_info.invest_begin_time;

    m_recharge_num      = day_investment_info.recharge_num;
    
    DayInvestmentStatusMap::iterator iter;
    for (uint32 i = 0; i < day_investment_info.info_list.size(); i++)
    {
        iter = m_day_investment_map.find(i);
        if (iter != m_day_investment_map.end())
        {
            iter->second = day_investment_info.info_list[i].status;
        }
    }
}

void DayInvestmentManager::FillDayInvestmentInfo(DAY_INVESTMENT_INFO& day_investment_info)
{
    day_investment_info.invest_begin_time = m_invest_begin_time;
    day_investment_info.recharge_num      = m_recharge_num;

    uint32 day_investment_info_count = m_day_investment_map.size();

    ONE_DAY_INVESTMENT_INFO info;
    for (uint32 i = 0; i < day_investment_info_count; i++)
    {
        info.Clear();
        info.day = i;
        info.status = (uint8)m_day_investment_map[i];
        day_investment_info.info_list.push_back(info);
    }
}

void DayInvestmentManager::OnRecv(uint32 msgID, void * data)
{
    switch (msgID)
    {
    case  MSG_DAY_INVESTMENT_REQ:
        {
            //OnDayInvestmentReq();
            break;
        }
    case MSG_DAY_INVESTMENT_INFO_REQ:
        {
            OnDayInvestmentInfoReq();
            break;
        }
    case MSG_DAY_INVESTMENT_ACCEPT_REWARD_REQ:
        {
            OnDayInvestmentAcceptRewardReq(data);
            break;
        }
    default:
        break;
    }
}

void DayInvestmentManager::IsActivateDayInvestment()
{
    if(!m_player) return;

    bool b_exe = false;
    if(!IsInvested()) //没有投资
    {
        int32 num = m_player->GetRechargeCount();
        int32 arrive_num = DayInvestmentConfig::Instance()->GetInvestmentMagicStoneCount();  //300磨蚀
        if((num >= m_recharge_num + arrive_num)&&  num > 0)
        {
            // 记录投资时间
            m_invest_begin_time = (uint32)Crown::SDNow().GetTimeValue();
            b_exe = true;
        }

    }

    Update();

    int32 active = 0;
    if(!b_exe && !IsInvested()) //没有到达投资
    {
       int32 invest_total_day = DayInvestmentConfig::Instance()->GetInvestmentTotalDay();
        active = invest_total_day;
    }
    else
    {
        active = NeedActiveTime();
    }
    int32 needmoney = 0;
    int32 num = m_player->GetRechargeCount();
    if(num >= m_recharge_num)
        needmoney = num - m_recharge_num;

    if(b_exe)
        SendDayInvestmentAck(ERR_SUCCEED, needmoney, active);                   // 填充返回给客户端的回执
    else
    {
        SendDayInvestmentAck(ERR_FAILED, needmoney, active);
    }
}
//void DayInvestmentManager::OnDayInvestmentReq()
//{
//    // 正在等待扣费回调
//    if (m_waiting_invest_trade_call_back)
//        return;
//
//    // 扣魔石投资
//    int32 investment_magic_stone =  DayInvestmentConfig::Instance()->GetInvestmentMagicStoneCount();
//
//    // 已投资
//    if (IsInvested())
//    {
//        SendDayInvestmentAck(ERR_FAILED, investment_magic_stone);
//        return;
//    }
//
//    if (m_player->GetMoneyPoint() < investment_magic_stone)
//    {
//        SendDayInvestmentAck(ERR_NO_ENOUGH_MONEYPOINT, investment_magic_stone);
//        return;
//    }
//
//    // 发送魔石交易订单,回调内再给客户端回执
//    TradeManager::Instance()->SendTradeOrderReq(m_player, REWARD_REASON_DAY_INVESTMENT,
//        PROP_DAY_INVESTMENT, 0, investment_magic_stone, investment_magic_stone);
//
//    m_waiting_invest_trade_call_back = true;
//}

void DayInvestmentManager::CBDayInvestmentTrade(uint32 investment_magic_stone)
{
    m_waiting_invest_trade_call_back = false;

    // 记录投资时间
    m_invest_begin_time = (uint32)Crown::SDNow().GetTimeValue();

    Update();

    // 填充返回给客户端的回执
    SendDayInvestmentAck(ERR_SUCCEED, investment_magic_stone);

    // 发通告
    INVESTMENT_OPERATION_NTF info;
    info.Clear();
    info.name = m_player->GetName();
    info.type = InvestmentManager::TYPE_DAY_INVESTMENT;
    info.operation = InvestmentManager::OPERATION_STORE;
    info.money = investment_magic_stone;
    InvestmentManager::Instance()->InvestmentOperationNTF(m_player, &info);
}

void DayInvestmentManager::OnDayInvestmentInfoReq()
{
    Update();


    DAY_INVESTMENT_INFO_ACK sendMsg;
    sendMsg.Clear();

    int32 needmoney = 0;
    int32 num = m_player->GetRechargeCount();
    if(num >= m_recharge_num)
        needmoney = num - m_recharge_num;
    sendMsg.invested_money = needmoney;

    if (IsInvested())
    {
        //sendMsg.invested_money = DayInvestmentConfig::Instance()->GetInvestmentMagicStoneCount();
          sendMsg.need_active_time = NeedActiveTime();
    }
    else
    {
        //sendMsg.invested_money = 0;
        int32 invest_total_day = DayInvestmentConfig::Instance()->GetInvestmentTotalDay();
        sendMsg.need_active_time = invest_total_day;
    }

    int32 count = DayInvestmentConfig::Instance()->GetInvestmentTotalDay()+1; //+1,加上第0天的
    FillDayInvestmentInfoList(count, sendMsg.info_list);

    m_player->SendToGate(MSG_DAY_INVESTMENT_INFO_ACK, &sendMsg);
}

int32 DayInvestmentManager::NeedActiveTime()
{
    time_t cur_time = Crown::SDNow().GetTimeValue();
    int32 interval_day = GetDaysBetweenDates(cur_time, m_invest_begin_time)+1; //+1,加上第0天的
    int32 invest_total_day = DayInvestmentConfig::Instance()->GetInvestmentTotalDay();
    if(interval_day >= invest_total_day)
        return (invest_total_day);//30天
    else 
        return (invest_total_day - interval_day);
}
void DayInvestmentManager::FillDayInvestmentInfoList(int32 count, std::vector<ONE_DAY_INVESTMENT_INFO>& info_list)
{
    ONE_DAY_INVESTMENT_INFO info;
    DayInvestmentStatusMap::iterator iter;
    for (int32 i=0; i<count; i++)
    {
        iter = m_day_investment_map.find(i);
        if (iter != m_day_investment_map.end())
        {
            info.Clear();
            info.day = i;
            info.status = (int32)iter->second;
            info_list.push_back(info);
        }
        else
        {
            info.Clear();
            info.day = i;
            info.status = (int32)NO_REWARD;
            info_list.push_back(info);
        }
    }
}

void DayInvestmentManager::OnDayInvestmentAcceptRewardReq( void *data )
{
    Update();

    DAY_INVESTMENT_ACCEPT_REWARD_REQ* recvMsg = (DAY_INVESTMENT_ACCEPT_REWARD_REQ*)data;

    if (!CheckIfCanAcceptReward(recvMsg->day))
    {
        DAY_INVESTMENT_ACCEPT_REWARD_ACK sendMsg;
        sendMsg.Clear();
        sendMsg.errcode = ERR_FAILED;
        sendMsg.day = recvMsg->day;
        m_player->SendToGate(MSG_DAY_INVESTMENT_ACCEPT_REWARD_ACK, &sendMsg);
        return;
    }

    // 置领奖标记
    DayInvestmentStatusMap::iterator iter;
    iter = m_day_investment_map.find(recvMsg->day);
    if (iter != m_day_investment_map.end())
    {
        iter->second = ACCEPTED_REWARD;
    }

    // 请求存档
    SaveDayInvestment(recvMsg->day);
}

void DayInvestmentManager::CalDailyRefresh()
{
    // 到达30天后若奖励没有领取，通过邮件发送
    time_t cur_time = Crown::SDNow().GetTimeValue();
    int32 interval_day = GetDaysBetweenDates(cur_time, m_invest_begin_time)+1; //+1,加上第0天的
    int32 invest_total_day = DayInvestmentConfig::Instance()->GetInvestmentTotalDay();
    if(interval_day >= invest_total_day && m_invest_begin_time > 0 )
    {
        m_recharge_num = m_player->GetRechargeCount();

        REWARD_TABLE rewardTable;
        rewardTable.reason = REWARD_REASON_DAY_INVESTMENT;
        rewardTable.Clear();

        GetHaveNotAward(rewardTable);

        if(m_player)
            MailManager::SendMail(m_player->GetCenterSvrID(), m_player->GetPlayerID(), "", "", Crown::ToString(MAIL_TITLE_ID_DAY_INVESTED), MAIL_TYPE_PLAYER_SYS, rewardTable.items, m_player->GetPlayerID());
            //MailManager::SendMail(m_player->GetCenterSvrID(), m_player->GetPlayerID(), "", MailTitleConfig::Instance()->GetMailTitle(MAIL_TITLE_ID_DAY_INVESTED), "", MAIL_TYPE_PLAYER_SYS, rewardTable.items, m_player->GetPlayerID());

        Clear();

        DAY_INVESTMENT_INFO_ACK sendMsg;
        sendMsg.Clear();
        sendMsg.need_active_time = NeedActiveTime();
        sendMsg.invested_money = 0;
        int32 count = DayInvestmentConfig::Instance()->GetInvestmentTotalDay()+1; //+1,加上第0天的
        FillDayInvestmentInfoList(count, sendMsg.info_list);
        m_player->SendToGate(MSG_DAY_INVESTMENT_INFO_ACK, &sendMsg);
    }
}

void DayInvestmentManager::GetAllReward()
{
    Update();

    for(DayInvestmentStatusMap::iterator dayInvestmentIt = m_day_investment_map.begin(); dayInvestmentIt != m_day_investment_map.end(); ++dayInvestmentIt)
    {
        if (!CheckIfCanAcceptReward(dayInvestmentIt->first))
            continue;

        dayInvestmentIt->second = ACCEPTED_REWARD;

        // 请求存档
        SaveDayInvestment(dayInvestmentIt->first);
    }
}

//30天到了，计算还没有领取的奖励
void DayInvestmentManager::GetHaveNotAward(REWARD_TABLE& rewardTable)
{
    //for(DayInvestmentStatusMap::iterator iter = m_day_investment_map.begin(); iter != m_day_investment_map.end(); iter++)
    //{
    //    if(iter->second == ACCEPTED_REWARD) continue;

    //    DayInvestmentInfo* info = DayInvestmentConfig::Instance()->GetDayInvestmentInfo(iter->first);
    //    if (!info) continue;

    //    bool b_find = false;
    //    for(std::vector<REWARD_ITEM>::iterator curit = rewardTable.items.begin(); curit != rewardTable.items.end(); curit++)
    //    {
    //        if((curit->id == info->reward.id) && (curit->type == info->reward.type))
    //        {
    //            curit->num = curit->num + info->reward.num;
    //            b_find = true;
    //            break;
    //        }
    //    }

    //    if(!b_find)
    //    {
    //        REWARD_ITEM attachment;
    //        attachment.Clear();
    //        attachment.type = info->reward.type;
    //        attachment.id = info->reward.id;
    //        attachment.num = info->reward.num;
    //        rewardTable.items.push_back(attachment);
    //    }
    //    iter->second = NO_REWARD;
    //}
}
void DayInvestmentManager::SaveDayInvestment( uint32 day )
{
    SAVE_DAY_INVESTMENT_INFO_REQ req;
    req.Clear();
    req.day = day;
    FillDayInvestmentInfo(req.investment_info);

    m_player->SendToCenter(MSG_SAVE_DAY_INVESTMENT_INFO_REQ, &req);
}

void DayInvestmentManager::CBSaveDayInvestment( void* data )
{
    SAVE_DAY_INVESTMENT_INFO_ACK* recv_ack = (SAVE_DAY_INVESTMENT_INFO_ACK*) data;

    DAY_INVESTMENT_ACCEPT_REWARD_ACK ack;
    ack.Clear();
    ack.errcode = ERR_FAILED;
    ack.day = recv_ack->day;

    uint32 reward_money = DayInvestmentConfig::Instance()->GiveDayInvestmentReward(m_player, recv_ack->day);
    if (reward_money > 0)
    {
        ack.errcode = ERR_SUCCEED;

        // 发通告
        INVESTMENT_OPERATION_NTF info;
        info.Clear();
        info.name = m_player->GetName();
        info.type = InvestmentManager::TYPE_DAY_INVESTMENT;
        info.operation = InvestmentManager::OPERATION_FETCH;
        info.money = reward_money;
        InvestmentManager::Instance()->InvestmentOperationNTF(m_player, &info);
    }

    m_player->SendToGate(MSG_DAY_INVESTMENT_ACCEPT_REWARD_ACK, &ack);
}

void DayInvestmentManager::Update()
{
    if (!IsInvested())
        return;

    bool hasReward = false;
    time_t cur_time = Crown::SDNow().GetTimeValue();
    int32 interval_day = GetDaysBetweenDates(cur_time, m_invest_begin_time)+1; //+1,加上第0天的
    int32 invest_total_day = DayInvestmentConfig::Instance()->GetInvestmentTotalDay();
    int32 min_legal_day = Crown::SDMin(invest_total_day, interval_day);

    // 根据最小合法天数设置可以领奖的标记
    for (int32 i=0; i<=min_legal_day; i++)
    {
        if (m_day_investment_map[i] == NO_REWARD)
        {
            m_day_investment_map[i] = CAN_ACCEPT_REWARD;
            
            hasReward = true;
        }
    }

    // 遍历计数已经领过奖励的总天数
    int32 accepted_reward_count = 0;
    int32 can_reward_count = 0;
    DayInvestmentStatusMap::iterator iter;
    for (iter = m_day_investment_map.begin(); iter != m_day_investment_map.end(); iter++)
    {
        if (iter->second == (int32)ACCEPTED_REWARD)
        {
            accepted_reward_count++;
        }

        if(iter->second == (int32)CAN_ACCEPT_REWARD)
        {
            can_reward_count++;
        }
    }

    // 所有投资天数的奖励都领了,且间隔天数已经超过投资总天数, 重置投资信息
    if ((accepted_reward_count > invest_total_day) && (interval_day > invest_total_day))
    {
        Clear();
    }

    // 通知客户端是否有奖励可领
    //m_player->GetNotifyManager()->RewardNotify(NOTIFY_REWARD_INVESTMENT, hasReward);
}

bool DayInvestmentManager::CheckIfCanAcceptReward(int32 day)
{
    DayInvestmentStatusMap::iterator iter;
    iter = m_day_investment_map.find(day);
    if (iter == m_day_investment_map.end())
    {
        // 没有该天
        return false;
    }

    if (iter->second != CAN_ACCEPT_REWARD)
    {
        // 该天奖励不可领
        return false;
    }

    return true;
}

bool DayInvestmentManager::IsInvested()
{
    return m_invest_begin_time > 0;
}

void DayInvestmentManager::SendDayInvestmentAck(int32 errCode, uint32 investedMoney,int32 active_time)
{
    DAY_INVESTMENT_ACK sendMsg;
    sendMsg.Clear();
    sendMsg.errcode = errCode;
    sendMsg.invested_money = investedMoney;
    sendMsg.need_active_time = active_time;
    int32 count = DayInvestmentConfig::Instance()->GetInvestmentTotalDay()+1; //+1,加上第0天的
    FillDayInvestmentInfoList(count, sendMsg.info_list);
    m_player->SendToGate(MSG_DAY_INVESTMENT_ACK, &sendMsg);
}
