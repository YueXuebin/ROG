#include "centerserver_pch.h"
#include "trade_manager.h"
#include "trade_job.h"
#include "game_handler.h"
#include "config_loader.h"
#include "player.h"
#include "player_manager.h"
#include "mail_manager.h"
#include "mail_title_config.h"
#include "dongyi_log.h"
IMPLEMENT_SINGLETON(TradeManager)

TradeManager::TradeManager()
{

}

TradeManager::~TradeManager()
{

}

// ==================================================
// 接收信息
// ==================================================
void TradeManager::OnRecv(uint64 playerID, uint32 sessionID, uint32 msgID, void* data)
{
    switch(msgID)
    {
    case MSG_TRADE_ORDER_REQ:
        RecvTradeOrderReq(data);
        break;
    case MSG_TRADE_ORDER_STATE_NTF:
        RecvTradeOrderStateNtf(data);
        break;
    default:
        break;
    }
}

// ==================================================
// 发送交易订单的物品
// ==================================================
void TradeManager::DeliverTradeOrder(TRADE_ORDER_INFO& tradeOrder)
{
    uint64 buyerID = 0;
    uint64 receiverID = 0;
    uint64 tradeOrderID = 0;
    Player* buyer = 0;
    std::string mailTitle;
    std::string mailMessage;
    std::vector<REWARD_ITEM> list;
    REWARD_ITEM attach;
    TRADE_ORDER_GROWTH_POINT sendMsg;

    // 计算角色ID
    buyerID = U64ID(tradeOrder.buyer_id.id_h, tradeOrder.buyer_id.id_l);
    receiverID = U64ID(tradeOrder.receiver_id.id_h, tradeOrder.receiver_id.id_l);

    // 添加附件
    attach.Clear();
    //attach.type = tradeOrder.item_type;
    attach.id = tradeOrder.item_id;
    attach.num = tradeOrder.item_num;
    list.push_back(attach);
    // 获得订单ID
    tradeOrderID = U64ID(tradeOrder.order_id.id_h, tradeOrder.order_id.id_l);

    // 获取角色
    buyer = PlayerManager::Instance()->GetPlayerByPlayerID(buyerID);

    // 发送返还点
    if(buyer)
    {
        sendMsg.total_price = tradeOrder.total_price;
        buyer->SendToGame(MSG_TRADE_ORDER_GROWTH_POINT, &sendMsg);
    }

    int32 moneyPoint = buyer ? buyer->GetMoneyPoint() : 0;

    // 魔石消费
    DoingGameLog::Instance()->WriteCenterLog(LOG_TRADE, buyerID, 
        WriterToLogByMoneyPoint(tradeOrder.pay_reason, tradeOrder.item_type, tradeOrder.item_num, tradeOrder.item_id, tradeOrderID, tradeOrder.order_state, tradeOrder.total_price, tradeOrder.unit_price, moneyPoint));

    // 如果购买者和玩家
    if(buyerID == receiverID)
    {
        if(buyer && (buyer->GetState() == Player::kStateInGame))
        {
            // 如果玩家在线,则通过GameServer给角色,否则通过邮件发送给玩家
            tradeOrder.err_code = ERR_SUCCEED;
            SendTradeOrderAck(buyer, tradeOrder);
            //buyer->SendToGame(MSG_TRADE_ORDER_ACK, &tradeOrder);
            return;
        }
        /*
        mailTitle = MailTitleConfig::Instance()->GetMailTitle(MAIL_TITLE_ID_MALL_BUY);
        mailMessage = MailTitleConfig::Instance()->GetMailMessage(MAIL_TITLE_ID_MALL_BUY);
        // 发送邮件
        MailManager::Instance()->SendMail(0, tradeOrder.receiver_name, mailTitle, mailMessage, MAIL_TYPE_PLAYER_SYS, list, tradeOrderID);
        */
        MailManager::SendMessageMail(0, tradeOrder.receiver_name, list, MAIL_TITLE_ID_MALL_BUY, 0);
    }
    else
    {
        // 购买者发送通知
        if(buyer)
        {
            ERROR_CODE sendMsg;
            sendMsg.errcode = ERR_SUCCEED;
            buyer->SendToGame(MSG_MALL_BUY_NTF, &sendMsg);  // 为了重置状态
            buyer->NotifyErrorCode(ERR_MALL_PRODUCT_SEND_TO_RECEIVER);
        }

//         if (tradeOrder.item_type == PROP_VIP_CARD_OFFER)   // 赠送VIP时效卡
//         {
//             MailManager::Instance()->SendMessageMail(0, tradeOrder.receiver_name, list, MAIL_MESSAGE_TYPE_VIP, 1, tradeOrder.buyer_name.c_str());
//         }
//         else
//         {
//             MailManager::Instance()->SendMessageMail(0, tradeOrder.receiver_name, list, MAIL_MESSAGE_TYPE_FRIEND, 1, tradeOrder.buyer_name.c_str());
//         }
    }
}

// ==================================================
// 生成交易订单ID
// ==================================================
uint64 TradeManager::GenTradeUUID()
{
    static uint16 serialNum = 0;
    // 时间戳 32bit, 序列号16bit, 
    uint32 high = (uint32)time(0);
    uint32 low = (++serialNum << 16);
    low |= ConfigLoader::Instance()->GetRegionID() & 0xFFFF;   // 区ID

    uint64 sn = high;
    sn = sn << 32;
    sn += low;

    return sn;
}

// ==================================================
// 请求商城数据
// ==================================================
void TradeManager::RecvTradeOrderReq(void* data)
{
    TRADE_ORDER_INFO* recvMsg = (TRADE_ORDER_INFO*)data;
    uint64 orderID = 0;

    if(!recvMsg) return;

    // 生成交易订单ID
    orderID = GenTradeUUID();

    // 赋值交易订单ID
    recvMsg->order_id.id_l = U64ID_LOW(orderID);
    recvMsg->order_id.id_h = U64ID_HIGH(orderID);

    // 初始化订单状态
    recvMsg->order_state = TRADE_STATE_INIT;

    TradeJob::Instance()->GetReceiverIDAndAccount(*recvMsg);
}

// ==================================================
// 发送订单状态通知
// ==================================================
void TradeManager::RecvTradeOrderStateNtf(void* data)
{
    TRADE_ORDER_STATE_NTF* recvMsg = (TRADE_ORDER_STATE_NTF*) data;
    uint64 tradeOrderID = 0;

    if(!recvMsg) return;

    tradeOrderID = U64ID(recvMsg->order_id.id_h, recvMsg->order_id.id_l);

    TradeJob::Instance()->SetTradeInfoDeliverState(tradeOrderID, recvMsg->order_state);
}

// ==================================================
// 发送交易应答
// ==================================================
void TradeManager::SendTradeOrderAck(Player* buyer, TRADE_ORDER_INFO& tradeOrder)
{
    buyer->SendToGame(MSG_TRADE_ORDER_ACK, &tradeOrder);
}
