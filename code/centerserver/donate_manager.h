#ifndef donate_manager_h__
#define donate_manager_h__

/*
 *	壕气冲天(魔石捐献)
 */
class DonateManager
{
public:
    DonateManager();
    ~DonateManager();

    DECLARE_SINGLETON(DonateManager)

    void Init();
    void OnRecv(uint64 playerID, uint32 sessionID, uint32 msgID, void* data);

    void Update();

    void SetConfig(const std::string& configJson);

    void SaveDB();

private:
    void OnAddBindmoneyNtf(void* data);

    void SendDonateInfo(uint64 playerID = 0);

    void SendReward();

private:
    DONATE_ALL_INFO m_donate_info;

    GAME_SETTING_DONATE_CONFIG m_configInfo;

    uint32 m_minute;                                        // 每分钟检测一次
    uint32 m_do_once;                                       // 只加一次
};

#endif // donate_manager_h__
