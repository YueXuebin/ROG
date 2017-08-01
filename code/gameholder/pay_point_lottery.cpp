//充值积分抽奖
#include "gameholder_pch.h"
#include "pay_point_lottery.h"
#include "player.h"
#include "pay_point_lottery_config.h"
#include "item_manager.h"
#include "package_manager.h"
#include "game_util.h"
#include "game_setting_manager.h"
#include "task_festive_config.h"

PayPointLotteryManager::PayPointLotteryManager(Player *player) : m_player(player)
{
}

PayPointLotteryManager::~PayPointLotteryManager()
{
    Clear();
}

void PayPointLotteryManager::OnRecv(uint32 msgID, void* data)
{
    switch (msgID)
    {
    case MSG_PAY_POINT_INFO_REQ:
        {
            OnMsgPayPointInfoReq(msgID, data);
        }
        break;
    case MSG_GET_PAY_POINT_LOTTERY_COST_ITEM_REQ:
        {
            OnMsgGetPayPointLotteryCostItemReq(msgID, data);
        }
        break;
    case MSG_PAY_POINT_LOTTERY_ONCE_START_REQ:
        {
            OnMsgPayPointLotteryOnceStartReq(msgID, data);
        }
        break;
    case MSG_PAY_POINT_LOTTERY_ALL_START_REQ:
        {
            OnMsgPayPointLotteryAllStartReq(msgID, data);
        }
        break;
    case MSG_GET_PAY_POINT_LOTTERY_OUTPU_REQ:
        {
            OnMsgGetPayPointLotteryOutputReq(msgID, data);
        }
        break;
    case MSG_PAY_POINT_LOTTERY_EXCHANGE_ITEM_REQ:
        {
            OnMsgPayPointLotteryExchangeItemReq(msgID, data);
        }
        break;
    }
}

void PayPointLotteryManager::CalDailyRefresh()
{
    if (m_payPointLotteryInfo.pay_point != 0)
    {
        m_payPointLotteryInfo.pay_point = 0;
        PushPayPointLotteryInfoAck(ERR_SUCCEED);
    }
}

void PayPointLotteryManager::Clear()
{
    AddLotteryOutputItem(); //如果有未领取的物品，人物销毁时领取
}

void PayPointLotteryManager::ActiveLotteryCostItemExchange()
{
    if (m_payPointLotteryInfo.exchange_enable == 0)
    {
        m_payPointLotteryInfo.exchange_enable = 1;
        PushPayPointLotteryInfoAck(ERR_SUCCEED);
    }
}

void PayPointLotteryManager::LoadPayPointLotteryInfo(PAY_POINT_LOTTERY_INFO &info)
{
    m_payPointLotteryInfo = info;
    //记录时间是否已过期
    if (!IsEnableTime(m_payPointLotteryInfo.record_time))
    {
        m_payPointLotteryInfo.Clear();
    }

    PushPayPointLotteryInfoAck(ERR_SUCCEED);
}

void PayPointLotteryManager::FillPayPointLotteryInfo(PAY_POINT_LOTTERY_INFO &info)
{
    info = m_payPointLotteryInfo;
}

void PayPointLotteryManager::OnMsgPayPointInfoReq(uint32 msgID, void *data)
{
    PushPayPointLotteryInfoAck(ERR_SUCCEED);
    PAY_POINT_LOTTERY_NTF sendMsg;
    sendMsg.local_get_record = m_payPointLotteryInfo.local_get_record;
    m_player->SendToGate(MSG_PAY_POINT_LOTTERY_NTF, &sendMsg);
}

void PayPointLotteryManager::OnMsgGetPayPointLotteryCostItemReq(uint32 msgID, void *data)
{
    PayPointLotteryConfig *cfg = PayPointLotteryConfig::Instance();

    if (!IsEnableTime((uint32)Crown::SDNow().GetTimeValue()))
    {
        return;
    }

    //每次只能兑换一次
    if (m_payPointLotteryInfo.exchange_enable != 1)
    {
        return;
    }

    if (m_payPointLotteryInfo.get_count >= cfg->GetMaxGetCount())
    {
        return;
    }

    //m_player->GetItemManager()->AddItemByNum(cfg->GetLotteryCostItemTID(), 1, REWARD_REASON_PAY_POINT_LOTTERY_EXCHANGE);
    m_payPointLotteryInfo.get_count += 1;
    m_payPointLotteryInfo.exchange_enable = 0;
    PushPayPointLotteryInfoAck(ERR_SUCCEED);
}

void PayPointLotteryManager::OnMsgPayPointLotteryOnceStartReq(uint32 msgID, void *data)
{
    PayPointLotteryStart(1);
}

void PayPointLotteryManager::OnMsgPayPointLotteryAllStartReq(uint32 msgID, void *data)
{
    //int lotteryCostItemNum = m_player->GetItemManager()->GetItemNum(PayPointLotteryConfig::Instance()->GetLotteryCostItemTID());
    //PayPointLotteryStart(lotteryCostItemNum);
}

void PayPointLotteryManager::OnMsgGetPayPointLotteryOutputReq(uint32 msgID, void *data)
{
    AddLotteryOutputItem();
}

void PayPointLotteryManager::OnMsgPayPointLotteryExchangeItemReq(uint32 msgID, void *data)
{
    if (!IsEnableTime((uint32)Crown::SDNow().GetTimeValue()))
    {
        //不在活动时间内
        PushPayPointLotteryInfoAck(ERR_PAY_POINT_EXCHANGE_ITEM_OUT_DATA);
        return;
    }

    PAY_POINT_LOTTERY_EXCHANGE_ITEM_REQ *recvMsg = (PAY_POINT_LOTTERY_EXCHANGE_ITEM_REQ*)data;

    const PAY_POINT_LOTTERY_ITEM *item = PayPointLotteryConfig::Instance()->GetExchangeItemByIdx(recvMsg->idx);
    if (!item)
    {
        //兑换物品不存在
        PushPayPointLotteryInfoAck(ERR_PAY_POINT_EXCHANGE_ITEM_NOT_EXIST);
        return;
    }

    if (m_payPointLotteryInfo.pay_point < item->cost)
    {
        //对换值不足
        PushPayPointLotteryInfoAck(ERR_PAY_POINT_EXCHANGE_ITEM_POINT_LACK);
        return;
    }

    m_payPointLotteryInfo.pay_point -= item->cost;
    PushPayPointLotteryInfoAck(ERR_SUCCEED);
    //m_player->GetItemManager()->AddItemByNum(item->tid, item->number, REWARD_REASON_PAY_POINT_LOTTERY_EXCHANGE);
}

void PayPointLotteryManager::PayPointLotteryStart(int count)
{
    PayPointLotteryConfig *cfg = PayPointLotteryConfig::Instance();

    if (!IsEnableTime((uint32)Crown::SDNow().GetTimeValue()))
    {
        //不在活动时间内
        PushPayPointLotteryInfoAck(ERR_PAY_POINT_LOTTERY_OUT_DATA);
        return;
    }

    //int lotteryCostItemNum = m_player->GetItemManager()->GetItemNum(cfg->GetLotteryCostItemTID());
    //count = Crown::SDMin(count, lotteryCostItemNum);
    //if (count <= 0)
    //{
    //    //物品不足
    //    PushPayPointLotteryInfoAck(ERR_PAY_POINT_LOTTERY_COST_ITEM_LACK);
    //    return;
    //}

    unsigned int lotteryPackageTID = cfg->GetLotteryPackageTID(m_player->GetVipLevel());
    if (lotteryPackageTID == 0)
    {
        //未找到对应抽奖包
        PushPayPointLotteryInfoAck(ERR_PAY_POINT_LOTTERY_FAIL);
        return;
    }

    AddLotteryOutputItem();	//如果有上次未领取的物品，先领取

    int outputNum = 0;

    for (int i = 0; i < count; ++i)
    {
        const TREASURE_PACKAGE_ITEM *packetItem = m_player->GetPackageManager()->GetPackageItem(lotteryPackageTID, PACKAGE_TYPE_PAY_POINT_LOTTERY);
        if (!packetItem)
        {
            continue;
        }
        int idx = cfg->GetLotteryItemIdxByTID(packetItem->item_id);
        if (idx == -1)
        {
            continue;
        }
        m_lotteryOutputItemIdxList.push_back(idx);
        outputNum += 1;
    }

    if (outputNum == 0)
    {
        //抽奖未成功
        PushPayPointLotteryInfoAck(ERR_PAY_POINT_LOTTERY_FAIL);
        return;
    }

    //更新物品和信息
    //m_player->GetItemManager()->DelItemByID(cfg->GetLotteryCostItemTID(), outputNum);
    m_payPointLotteryInfo.pay_point += outputNum;
    PushPayPointLotteryInfoAck(ERR_SUCCEED);

    //同步客户端产出
    PAY_POINT_LOTTERY_OUTPUT_ACK ack;
    for (int i = 0; i < outputNum; ++i)
    {
        int idx = m_lotteryOutputItemIdxList[i];
        ack.idx_list.push_back(idx);
    }

    m_player->SendToGate(MSG_PAY_POINT_LOTTERY_OUTPUT_ACK, &ack);
}

void PayPointLotteryManager::AddLotteryOutputItem()
{
    for (unsigned int i = 0; i < m_lotteryOutputItemIdxList.size(); ++i)
    {
        int idx = m_lotteryOutputItemIdxList[i];
        const PAY_POINT_LOTTERY_ITEM *item = PayPointLotteryConfig::Instance()->GetLotteryItemByIdx(idx);
        if (!item)
        {
            continue;
        }
        //m_player->GetItemManager()->AddItemByNum(item->tid, item->number, REWARD_REASON_PAY_POINT_LOTTERY_OUTPUT);

        if (PayPointLotteryConfig::Instance()->isWorldRadioItem(item->tid))
        {
            //全服公告
            SERVER_MARQUEE marqueeMsg;
            marqueeMsg.id = MARQUEE_PAY_POINT_LOTTERY;
            marqueeMsg.parame_list.push_back(MarQueePlayerName(m_player->GetVipLevel(), m_player->GetName()));
            marqueeMsg.parame_list.push_back(MarQueeItem(PROP_ITEM, item->number, item->tid));
            m_player->OnMarQuee(marqueeMsg);
        }

        PAY_POINT_LOTTERY_GET_RECORD payReward;
        payReward.owner_name = m_player->GetName();
        payReward.item_tid = item->tid;
        payReward.item_color = item->color;
        m_payPointLotteryInfo.local_get_record.push_back(payReward);

        int32 count = m_payPointLotteryInfo.local_get_record.size() - 10;
        if(count > 0)
        {
            m_payPointLotteryInfo.local_get_record.erase(m_payPointLotteryInfo.local_get_record.begin(), m_payPointLotteryInfo.local_get_record.begin() + count);
        }

        PAY_POINT_LOTTERY_NTF sendMsg;
        sendMsg.local_get_record = m_payPointLotteryInfo.local_get_record;
        m_player->SendToGate(MSG_PAY_POINT_LOTTERY_NTF, &sendMsg);
    }
    m_lotteryOutputItemIdxList.clear();
}

bool PayPointLotteryManager::IsEnableTime(uint32 time) const
{
    const TASK_FESTIVAL_TEMPLATE* festivalTmp = TaskFestiveConfig::Instance()->GetFestivalTmpl(m_player->GetCenterSvrID(), FESTIVAL_PAY_POINT_LOTTERY_ID);
    if(!festivalTmp)
        return false;

    return time >= festivalTmp->begin_time && time <= festivalTmp->end_time;
}

void PayPointLotteryManager::PushPayPointLotteryInfoAck(unsigned short err)
{
    //同步客户端数据
    PAY_POINT_LOTTERY_INFO_ACK ack;

    ack.errcode = err;

    if (ack.errcode == ERR_SUCCEED)
    {
        ack.exchange_enable = m_payPointLotteryInfo.exchange_enable;
        ack.pay_point = m_payPointLotteryInfo.pay_point;
        ack.get_count = m_payPointLotteryInfo.get_count;
    }

    m_player->SendToGate(MSG_PAY_POINT_INFO_ACK, &ack);
}