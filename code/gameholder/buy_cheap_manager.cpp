#include "gameholder_pch.h"
#include "buy_cheap_manager.h"
#include "player.h"
#include "task_festive_config.h"
#include "buy_cheap_config.h"
#include "game_util.h"
#include "trade_manager.h"

BuyCheapMgr::BuyCheapMgr(Player* player) :
m_player(player)
{
    m_buyCheapInfo.Clear();
}

BuyCheapMgr::~BuyCheapMgr()
{

}

void BuyCheapMgr::LoadInfo(const BUY_CHEAP_INFO& info)
{
    m_buyCheapInfo = info;
}

void BuyCheapMgr::FillInfo(BUY_CHEAP_INFO& info)
{
    info = m_buyCheapInfo;
}

void BuyCheapMgr::OnRecv(uint32 msgID, void* data)
{
    switch(msgID)
    {
    case MSG_BUY_CHEAP_INFO_REQ:
        onBuyCheapInfoReq();
        break;
    case MSG_BUY_CHEAP_BUY_REQ:
        onBuyCheapBuyReq(data);
        break;
    case MSG_BUY_CHEAP_REWARD_REQ:
        onBuyCheapRewardReq();
        break;
    default:
        break;
    }
}

void BuyCheapMgr::CalDailyRefresh()
{
    m_buyCheapInfo.Clear();

    onBuyCheapInfoReq();
}

void BuyCheapMgr::onBuyCheapInfoReq()
{
    const TASK_FESTIVAL_TEMPLATE* festivalTmp = TaskFestiveConfig::Instance()->GetFestivalTmpl(m_player->GetCenterSvrID(), FESTIVAL_BUY_CHEAP_ID);
    if(!festivalTmp)
    {
        CnWarn("no Festival buy cheap id=403\n");
        return;
    }

    uint32 nowTime = (uint32)Crown::SDNow().GetTimeValue();
    if(festivalTmp->begin_time > nowTime || nowTime > festivalTmp->end_time)
    {
        //m_player->NotifyErrorCode(ERR_ACTIVITY_NO_OPEN);
        return;
    }

    int32 day = m_player->GetDaysBetweenDates(nowTime, festivalTmp->begin_time);
    const BUY_CHEAPS_ITEM* buyCheapsItem = BuyCheapConfig::Instance()->GetBuyCheapByDay(day);
    if(!buyCheapsItem)
    {
        CnAssert(false);
        return;
    }

    m_buyCheapInfo.config_day = buyCheapsItem->day;

    // 没有数据初始化
    if(m_buyCheapInfo.discount_list.size() == 0)
    {
        for (int32 i = 1; i <= 4; ++i)
        {
            BUY_CHEAP_STATE discount;
            discount.id = i;

            if(i == 1)
                discount.state = CAN_ACCEPT_REWARD;
            else
                discount.state = NO_REWARD;

            m_buyCheapInfo.discount_list.push_back(discount);
        }

        m_buyCheapInfo.ultimate_reward = NO_REWARD;
    }

    m_player->SendToGate(MSG_BUY_CHEAP_INFO_ACK, &m_buyCheapInfo);
}

void BuyCheapMgr::onBuyCheapBuyReq(void* data)
{
    BUY_CHEAP_BUY_REQ* recvMsg = (BUY_CHEAP_BUY_REQ*)data;
    int32 err_code = ERR_FAILED;

    // 活动时间没到 不能买
    const TASK_FESTIVAL_TEMPLATE* festivalTmp = TaskFestiveConfig::Instance()->GetFestivalTmpl(m_player->GetCenterSvrID(), FESTIVAL_BUY_CHEAP_ID);
    if(!festivalTmp)
    {
        CnWarn("no Festival buy cheap id=403\n");
        return;
    }

    uint32 nowTime = (uint32)Crown::SDNow().GetTimeValue();
    if(festivalTmp->begin_time > nowTime || nowTime > festivalTmp->end_time)
    {
        m_player->NotifyErrorCode(ERR_ACTIVITY_NO_OPEN);
        return;
    }

    BUY_CHEAP_STATE* buyCheapState = GetCheapState(recvMsg->buy_index);
    if(!buyCheapState)
    {
        CnAssert(false);
        return;
    }

    if(buyCheapState->state != CAN_ACCEPT_REWARD)
    {
        sendBuyCheapStateAck(err_code, recvMsg->buy_index);
        return;
    }

    const BUY_CHEAPS_ITEM* buyCheapTmpl = BuyCheapConfig::Instance()->GetBuyCheapByDay(m_buyCheapInfo.config_day);
    if(!buyCheapTmpl)
    {
        CnAssert(false);
        return;
    }

    int32 point = (int32)((real)buyCheapTmpl->original_cost * ToPercent(GetDiscount(buyCheapTmpl, recvMsg->buy_index)));

    err_code = m_player->CheckItemOne(PROP_MONEYPOINT, point);
    if(err_code != ERR_SUCCEED)
    {
        sendBuyCheapStateAck(err_code, recvMsg->buy_index);
        return;
    }

    TradeManager::Instance()->SendTradeOrderReq(m_player, REWARD_REASON_BUY_CHEAP, 0, 0, 0, point, Crown::ToString(recvMsg->buy_index).c_str());
}

BUY_CHEAP_STATE* BuyCheapMgr::GetCheapState(int32 index)
{
    for(std::vector<BUY_CHEAP_STATE>::iterator it = m_buyCheapInfo.discount_list.begin(); it != m_buyCheapInfo.discount_list.end(); ++it)
    {
        if(it->id == index)
            return &(*it);
    }

    return NULL;
}

int32 BuyCheapMgr::GetDiscount(const BUY_CHEAPS_ITEM* buyCheapsTmpl, int32 index)
{
    switch(index)
    {
    case 1:
        return buyCheapsTmpl->discount_1;
    case 2:
        return buyCheapsTmpl->discount_2;
    case 3:
        return buyCheapsTmpl->discount_3;
    case 4:
        return buyCheapsTmpl->discount_4;
    default:
        CnAssert(false);
        break;
    }

    return 0;
}

void BuyCheapMgr::sendBuyCheapStateAck(int32 err_code, int buy_index)
{
    BUY_CHEAP_BUY_ACK sendMsg;
    sendMsg.buy_index = buy_index;
    sendMsg.err_code = err_code;
    sendMsg.info = m_buyCheapInfo;
    m_player->SendToGate(MSG_BUY_CHEAP_BUY_ACK, &sendMsg);
}

void BuyCheapMgr::onBuyCheapRewardReq()
{
    BUY_CHEAP_REWARD_ACK ack;

    // 活动时间没到 不能领奖励
    const TASK_FESTIVAL_TEMPLATE* festivalTmp = TaskFestiveConfig::Instance()->GetFestivalTmpl(m_player->GetCenterSvrID(), FESTIVAL_BUY_CHEAP_ID);
    if(!festivalTmp)
    {
        CnWarn("no Festival buy cheap id=403\n");
        return;
    }

    uint32 nowTime = (uint32)Crown::SDNow().GetTimeValue();
    if(festivalTmp->begin_time > nowTime || nowTime > festivalTmp->end_time)
    {
        m_player->NotifyErrorCode(ERR_ACTIVITY_NO_OPEN);
        return;
    }

    for (std::vector<BUY_CHEAP_STATE>::iterator it = m_buyCheapInfo.discount_list.begin(); it != m_buyCheapInfo.discount_list.end(); ++it)
    {
        if(it->state == NO_REWARD)
        {
            BUY_CHEAP_REWARD_ACK ack;
            ack.err_code = ERR_FAILED;
            m_player->SendToGate(MSG_BUY_CHEAP_REWARD_ACK, &ack);
            return;
        }
    }

    m_buyCheapInfo.ultimate_reward = ACCEPTED_REWARD;
    ack.info = m_buyCheapInfo;

    const BUY_CHEAPS_ITEM* buyCheaps = BuyCheapConfig::Instance()->GetBuyCheapByDay(m_buyCheapInfo.config_day);
    if(!buyCheaps)
    {
        CnAssert(false);
        return;
    }

    m_player->TakeInRewardTable(PROP_ITEM, buyCheaps->ultimate_item_id, buyCheaps->ultimate_item_num, REWARD_REASON_BUY_CHEAP);

    ack.err_code = ERR_SUCCEED;
    m_player->SendToGate(MSG_BUY_CHEAP_REWARD_ACK, &ack);
}

void BuyCheapMgr::CBBuyCheapBuy(const std::string& str)
{
    int32 index = atoi(str.c_str());

    const BUY_CHEAPS_ITEM* buyCheapTmpl = BuyCheapConfig::Instance()->GetBuyCheapByDay(m_buyCheapInfo.config_day);
    if(!buyCheapTmpl)
    {
        CnAssert(false);
        return;
    }

    m_player->TakeInRewardTable(PROP_ITEM, buyCheapTmpl->discount_item_id, buyCheapTmpl->discount_item_num, REWARD_REASON_BUY_CHEAP);

    BUY_CHEAP_STATE* buyCheapState = GetCheapState(index);
    if(!buyCheapState)
    {
        CnAssert(false);
        return;
    }

    buyCheapState->state = ACCEPTED_REWARD;

    BUY_CHEAP_STATE* nextBuyCheapState = GetCheapState(index + 1);
    if(nextBuyCheapState)
    {
        nextBuyCheapState->state = CAN_ACCEPT_REWARD;
    }

    sendBuyCheapStateAck(ERR_SUCCEED, index);
}

