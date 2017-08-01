/*
*   交易管理器
*/
#ifndef trade_manager_h__
#define trade_manager_h__

typedef std::map<uint32, uint32 > BuyMap;      // 批量购买用的结构<id, buyNum> < <111,1>, <222,3> >
typedef std::map<uint32, int32 >   CostMap;   // 批量查询花费 <type, cost(有可能是负数)>

class Player;
class TradeObject;

class TradeManager
{
public:
    TradeManager();
    ~TradeManager();

    DECLARE_SINGLETON(TradeManager)

    void OnCSRecv(uint64 playerID, uint32 sessionID, uint32 msgID, void* data);   ///< 接收网络消息

    ///< 发送交易订单请求，买东西给自己
    void SendTradeOrderReq(Player* player, int32 pay_reason, 
        int32 item_type, uint32 item_id, int32 item_num, int32 total_price, const std::string& ext = "");

    ///< 发送交易订单请求，买东西给别人(也可以是自己)
    void SendTradeOrderToReq(uint64 buyerID, const std::string& buyerAccount, const std::string& buyerName, 
                            const std::string& receiverName, int32 pay_reason, 
                            int32 item_type, uint32 item_id, int32 item_num, int32 unit_price, int32 total_price, 
                            uint32 product_id = 0, int32 product_num = 0, int32 param_1 = 0, const std::string& ext = "");

    ///< 发送交易订单请求，买东西给别人(也可以是自己)(rog新的订单交易方法入口)
    void TradeOrder(uint64 buyerID, const std::string& buyerAccount, const std::string& buyerName, 
        const std::string& receiverName, int32 pay_reason, 
        int32 item_type, uint32 item_id, int32 item_num, int32 unit_price, int32 total_price, 
        uint32 product_id = 0, int32 product_num = 0, int32 param_1 = 0, const std::string& ext = "");
    ///< 发送交易订单请求，买东西给别人(也可以是自己)(rog服务相关交易方法入口)
    bool TradeOrderService(uint64 buyerID, const std::string& buyerAccount, const std::string& buyerName, 
        const std::string& receiverName, int32 pay_reason, 
        int32 item_type, uint32 item_id, int32 item_num, int32 unit_price, int32 total_price, 
        uint32 product_id = 0, int32 product_num = 0, int32 param_1 = 0, const std::string& ext = "");

    ///< 批量交易
    bool TradeListOrder(uint64 buyerID, const BuyMap* buyItems, int32 pay_reason);

    void SendTradeOrderStateNtf(Player* player, uint64 tradeOrderID, int32 orderState);
    uint64 GenTradeUUID();   // 生成交易订单号

private:
    ///< 接收交易订单确认消息
    void RecvTradeOrderAck(void* data);
    ///< 接收交易成长点消息
    void RecvTradeOrderGrowthPoint(uint64 playerID, void* data);
    ///< 公会转盘的处理
    bool GuildTrunTable(uint64 playerID, REWARD_TABLE rewardTable, std::string str);
};


#endif // trade_manager_h__
