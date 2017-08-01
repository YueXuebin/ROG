#ifndef proclamation_manager_h__
#define proclamation_manager_h__


// 警示公告
class Player;
class ProclamationManager
{
public:
    ProclamationManager(void);
    ~ProclamationManager(void);

    DECLARE_SINGLETON(ProclamationManager)

    void OnServerReady();

    void OnRecv(uint32 msgID, void* data);

    void OnPlayerProclamationReq(Player* player, void* data);                    // 请求公告

private:

    void LoadProclamationInfo(GLOBAL_PROCLAMATION_BAG& info);                           //更新配置信息

    GLOBAL_PROCLAMATION_BAG m_proclamationList;
};


#endif