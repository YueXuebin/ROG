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
// ������Ϣ
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
// ���ͽ��׶�������Ʒ
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

    // �����ɫID
    buyerID = U64ID(tradeOrder.buyer_id.id_h, tradeOrder.buyer_id.id_l);
    receiverID = U64ID(tradeOrder.receiver_id.id_h, tradeOrder.receiver_id.id_l);

    // ��Ӹ���
    attach.Clear();
    //attach.type = tradeOrder.item_type;
    attach.id = tradeOrder.item_id;
    attach.num = tradeOrder.item_num;
    list.push_back(attach);
    // ��ö���ID
    tradeOrderID = U64ID(tradeOrder.order_id.id_h, tradeOrder.order_id.id_l);

    // ��ȡ��ɫ
    buyer = PlayerManager::Instance()->GetPlayerByPlayerID(buyerID);

    // ���ͷ�����
    if(buyer)
    {
        sendMsg.total_price = tradeOrder.total_price;
        buyer->SendToGame(MSG_TRADE_ORDER_GROWTH_POINT, &sendMsg);
    }

    int32 moneyPoint = buyer ? buyer->GetMoneyPoint() : 0;

    // ħʯ����
    DoingGameLog::Instance()->WriteCenterLog(LOG_TRADE, buyerID, 
        WriterToLogByMoneyPoint(tradeOrder.pay_reason, tradeOrder.item_type, tradeOrder.item_num, tradeOrder.item_id, tradeOrderID, tradeOrder.order_state, tradeOrder.total_price, tradeOrder.unit_price, moneyPoint));

    // ��������ߺ����
    if(buyerID == receiverID)
    {
        if(buyer && (buyer->GetState() == Player::kStateInGame))
        {
            // ����������,��ͨ��GameServer����ɫ,����ͨ���ʼ����͸����
            tradeOrder.err_code = ERR_SUCCEED;
            SendTradeOrderAck(buyer, tradeOrder);
            //buyer->SendToGame(MSG_TRADE_ORDER_ACK, &tradeOrder);
            return;
        }
        /*
        mailTitle = MailTitleConfig::Instance()->GetMailTitle(MAIL_TITLE_ID_MALL_BUY);
        mailMessage = MailTitleConfig::Instance()->GetMailMessage(MAIL_TITLE_ID_MALL_BUY);
        // �����ʼ�
        MailManager::Instance()->SendMail(0, tradeOrder.receiver_name, mailTitle, mailMessage, MAIL_TYPE_PLAYER_SYS, list, tradeOrderID);
        */
        MailManager::SendMessageMail(0, tradeOrder.receiver_name, list, MAIL_TITLE_ID_MALL_BUY, 0);
    }
    else
    {
        // �����߷���֪ͨ
        if(buyer)
        {
            ERROR_CODE sendMsg;
            sendMsg.errcode = ERR_SUCCEED;
            buyer->SendToGame(MSG_MALL_BUY_NTF, &sendMsg);  // Ϊ������״̬
            buyer->NotifyErrorCode(ERR_MALL_PRODUCT_SEND_TO_RECEIVER);
        }

//         if (tradeOrder.item_type == PROP_VIP_CARD_OFFER)   // ����VIPʱЧ��
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
// ���ɽ��׶���ID
// ==================================================
uint64 TradeManager::GenTradeUUID()
{
    static uint16 serialNum = 0;
    // ʱ��� 32bit, ���к�16bit, 
    uint32 high = (uint32)time(0);
    uint32 low = (++serialNum << 16);
    low |= ConfigLoader::Instance()->GetRegionID() & 0xFFFF;   // ��ID

    uint64 sn = high;
    sn = sn << 32;
    sn += low;

    return sn;
}

// ==================================================
// �����̳�����
// ==================================================
void TradeManager::RecvTradeOrderReq(void* data)
{
    TRADE_ORDER_INFO* recvMsg = (TRADE_ORDER_INFO*)data;
    uint64 orderID = 0;

    if(!recvMsg) return;

    // ���ɽ��׶���ID
    orderID = GenTradeUUID();

    // ��ֵ���׶���ID
    recvMsg->order_id.id_l = U64ID_LOW(orderID);
    recvMsg->order_id.id_h = U64ID_HIGH(orderID);

    // ��ʼ������״̬
    recvMsg->order_state = TRADE_STATE_INIT;

    TradeJob::Instance()->GetReceiverIDAndAccount(*recvMsg);
}

// ==================================================
// ���Ͷ���״̬֪ͨ
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
// ���ͽ���Ӧ��
// ==================================================
void TradeManager::SendTradeOrderAck(Player* buyer, TRADE_ORDER_INFO& tradeOrder)
{
    buyer->SendToGame(MSG_TRADE_ORDER_ACK, &tradeOrder);
}
