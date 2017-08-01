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

    void Init();                                                                        // ��ʼ��
    void OnRecv(uint64 playerID, uint32 sessionID, uint32 msgID, void* data);

    void SaveDB();                                                                      // ��������
    void DailyRefresh();                                                                // ��ˢ��
    void UpdateDailyInfo();                                                             // ��������

private:

    void SendDailyFashionInfo(uint64 playerID = 0);                                     // ͬ�����ݵ�gameserver

    DAILY_DRAW_FASHION_INFO m_dailyFashionConfig;
};

#endif // donate_manager_h__