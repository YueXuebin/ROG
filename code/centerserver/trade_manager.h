/*
*   交易管理器
*/

#ifndef trade_manager_h__
#define trade_manager_h__

#include "player.h"

class TradeManager
{
public:
    TradeManager();
    ~TradeManager();

    DECLARE_SINGLETON(TradeManager)

    void OnRecv(uint64 playerID, uint32 sessionID, uint32 msgID, void* data);   ///< 接收网络消息

    void DeliverTradeOrder(TRADE_ORDER_INFO& tradeOrder);

    void SendTradeOrderAck(Player* buyer, TRADE_ORDER_INFO& tradeOrder);                       ///< 发送交易应答
private:
    uint64 GenTradeUUID();                          ///< 生成交易订单ID
    void RecvTradeOrderReq(void* data);             ///< 请求交易订单
    void RecvTradeOrderStateNtf(void* data);        ///< 发送订单状态通知
};

#endif // mall_manager_h__
