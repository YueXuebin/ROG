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

    void Init();                                                                        // 初始化
    void OnRecv(uint64 playerID, uint32 sessionID, uint32 msgID, void* data);

    void SaveDB();                                                                      // 保存数据
    void DailyRefresh();                                                                // 点刷新
    void UpdateDailyInfo();                                                             // 更新数据

private:

    void SendDailyFashionInfo(uint64 playerID = 0);                                     // 同步数据到gameserver

    DAILY_DRAW_FASHION_INFO m_dailyFashionConfig;
};

#endif // donate_manager_h__