/*
*   充值管理器
*/
#ifndef pay_manager_h__
#define pay_manager_h__

class PayManager
{
public:
    PayManager();
    ~PayManager();

    DECLARE_SINGLETON(PayManager)

    void OnRecv(uint64 playerID, uint32 sessionID, uint32 msgID, void* data);   // 接收网络消息

private:
    uint64 GenPayUUID();                                                        // 生成交易订单ID
    void RecvPayOrderReq(uint64 playerID, void* data);                                           // 请求交易订单
};

#endif // pay_manager_h__
