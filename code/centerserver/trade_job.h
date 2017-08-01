#ifndef trade_job_h__
#define trade_job_h__

class DBExecutor;
class DBRecorder;
class Player;

class TradeJob
{
public:
    TradeJob() { m_dbExecutor = 0; };
    ~TradeJob() {};

    DECLARE_SINGLETON(TradeJob)

    // 设置SQL执行
    void SetDBExecutor(DBExecutor* val) {m_dbExecutor = val;}

    void LoadMallData();                                                    // 加载商城数据
    void SaveMallData();                                                    // 保存商城数据

    void GetReceiverIDAndAccount(TRADE_ORDER_INFO& tradeOrder);             // 获取接收者的ID和账户名
    void InsertTradeInfo(TRADE_ORDER_INFO& tradeOrder);                     // 插入加入交易订单信息
    void SetTradeInfoPayState(TRADE_ORDER_INFO& tradeOrder);                // 设置交易订单状态
    void SetTradeInfoDeliverState(uint64 orderID, int32 orderState);        // 设置交易订单状态

    bool SaveInvestmentOperation(void* data);                               ///< 保存投资操作
    void LoadInvestmentOperation(uint32 count, uint64 playerID);            ///< 发送最新的count个投资操作给玩家

    void SaveGlobalLimitSellDate(){};                          // 保存全局商品限量数据
private:
    TradeJob( const TradeJob& );
    TradeJob& operator=( const TradeJob& );
    TradeJob* operator&();
    const TradeJob* operator&() const;

    void DeductMoneyPoint(TRADE_ORDER_INFO& tradeOrder);                                        // 保存交易数据

    void CBGetReceiverIDAndAccount(DBRecorder& res, TRADE_ORDER_INFO tradeOrder);               // 查询接收者的ID和账号名

    void CBInsertTradeInfo(DBRecorder& res, TRADE_ORDER_INFO tradeOrder);                       // 插入订单回调

    void CBSetTradeInfoPayState(DBRecorder& res, TRADE_ORDER_INFO tradeOrder);                  // 设置订单状态

    void CBSetTradeInfoDeliverState(DBRecorder& res, uint64 tradeOrderID, int32 orderState);    // 设置订单状态

    void CBDeductMoneyPoint(DBRecorder& res, TRADE_ORDER_INFO tradeOrder);                      // 减少魔石

    //void CBGetMallData(DBRecorder& res);                                                      // 加载商城数据

    void CBLoadInvestmentOperation(DBRecorder& res, uint64 playerID);                           // 加载投资操作

    void CBSaveGlobalLimitSellDate(DBRecorder& res){};                                            // 保存全局商品限量数据回调

    DBExecutor*         m_dbExecutor;
};


#endif // trade_job_h__
