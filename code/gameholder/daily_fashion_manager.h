#ifndef daily_fashion_manager_h__
#define daily_fashion_manager_h__

/*
 *	每日化身
 */
class DailyFashionManager
{
public:
    DailyFashionManager(void);
    ~DailyFashionManager(void);

    DECLARE_SINGLETON(DailyFashionManager)

    void OnRecv(uint64 playerID, uint32 msgID, void* data);

    void OnServerReady();
    
private:

    void OnDailyFashionInfoAck(void* data, uint64 playerID);                        // 每日抽奖信息
    void GetDailyFashionInfo(void* data, uint64 playerID);                          // 请求每日抽奖信息
    void OnDailyFashionDrawReq(void* data, uint64 playerID);                        // 抽奖请求


    DAILY_DRAW_FASHION_INFO m_dailyFashionInfo;
};
#endif