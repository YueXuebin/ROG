#ifndef pay_job_h__
#define pay_job_h__

class DBExecutor;
class DBRecorder;
class Player;

class PayJob
{
public:
    PayJob() { m_dbExecutor = 0; };
    ~PayJob() {};

    DECLARE_SINGLETON(PayJob)

    // 设置SQL执行
    void SetDBExecutor(DBExecutor* val) {m_dbExecutor = val;}

    void InsertPayOrder(PAY_ORDER_INFO& payOrde);           // 插入充值订单信息

    void GetTotalRechargeByPlayerId(uint64 playerId);       // 获取玩家总共的充值数
    void GetDailyRechargeByPlayerId(uint64 playerId);       // 获取玩家今天的充值数

    void ReloadRechargeInfo(uint64 playerID);               ///< 重新加载充值信息

private:
    PayJob( const PayJob& );
    PayJob& operator=( const PayJob& );
    PayJob* operator&();
    const PayJob* operator&() const;

    void UpdatePlayerMoneyPoint(PAY_ORDER_INFO& payOrder);   // 更新角色魔石数量

    void CBInsertPayOrder(DBRecorder& res, PAY_ORDER_INFO payOrder);

    void CBUpdatePlayerMoneyPoint(DBRecorder& res, PAY_ORDER_INFO payOrder);

    void CBGetTotalRechargeByPlayerId(DBRecorder& res, uint64 playerId);    // 获取玩家总共的充值数回调
    void CBGetDailyRechargeByPlayerId(DBRecorder& res, uint64 playerId);    // 获取玩家今天的充值数回调

    void CBReloadRechargeInfo(DBRecorder& res, uint64 playerID);            ///< 获得充值信息

    DBExecutor*         m_dbExecutor;
};


#endif // pay_job_h__
