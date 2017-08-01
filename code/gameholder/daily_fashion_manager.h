#ifndef daily_fashion_manager_h__
#define daily_fashion_manager_h__

/*
 *	ÿ�ջ���
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

    void OnDailyFashionInfoAck(void* data, uint64 playerID);                        // ÿ�ճ齱��Ϣ
    void GetDailyFashionInfo(void* data, uint64 playerID);                          // ����ÿ�ճ齱��Ϣ
    void OnDailyFashionDrawReq(void* data, uint64 playerID);                        // �齱����


    DAILY_DRAW_FASHION_INFO m_dailyFashionInfo;
};
#endif