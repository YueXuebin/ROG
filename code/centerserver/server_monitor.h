#ifndef server_monitor_h__
#define server_monitor_h__

/*
 *	服务器状态监控
 */
enum{
    SERVER_STATE_INIT       = 0,            // 服务器尚未初始化
    SERVER_STATE_RUNNING    = 1,            // 服务器正常运行
    SERVER_STATE_EXCEPTION  = 2,            // 服务器出现了异常(比如某个server掉了)
};

class ServerMonitor
{
public:
    ServerMonitor();
    ~ServerMonitor();

    DECLARE_SINGLETON(ServerMonitor)

    void OnAllServerReady();

    void OnGameServerClose(uint32 gameSvrID);
    void OnGateServerClose(uint32 gateRegionID);
    void OnFlashServerClose();
    void OnTimerServerClose();

    uint32 GetState() {return m_state;}
private:
    void SetState(uint32 state);

private:
    uint32  m_state;
};


#endif // server_monitor_h__
