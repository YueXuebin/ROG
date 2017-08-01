#ifndef proclamation_manager_h__
#define proclamation_manager_h__

class ProclamationManager
{
public:
    ProclamationManager(void);
    ~ProclamationManager(void);

    DECLARE_SINGLETON(ProclamationManager)

    void OnRecv(uint64 player, uint32 sessionID, uint32 msgID, void* data);

    void Init();

    void ReloadProclamationList();

    GLOBAL_PROCLAMATION_BAG& GetProclamationList() {return m_proclamationList;}              //  公告信息

private:
    void SendProclamationList(uint32 gameserverID = 0);

    GLOBAL_PROCLAMATION_BAG m_proclamationList;

};

#endif