#ifndef GAMEINFO_HANDLER_H
#define GAMEINFO_HANDLER_H

class TransferClientProxy;

class ServerInfoHandler
{
public:
    ServerInfoHandler();
    ~ServerInfoHandler();

    DECLARE_SINGLETON(ServerInfoHandler)

    bool Init(const char* ip, uint32 port);

    void Update();          ///< 检查状态

    // 接收网络数据
    void OnRecv(uint64 playerID, uint32 sessionID, uint32 msgID, void* data);
    
    void Print(MONITOR_INFO_ACK* Center_Info);
    void DungeonInfoPrint(std::vector<DUNGEON_INFO>* allDungeonInfo);

private:
    void OnAccept(uint32 sessionID, void* data);
    void OnConnect(uint32 sessionID, void* data);
    void OnClose(uint32 sessionID, void* data);
    void OnError(uint32 sessionID, void* data);


    uint32                  m_state;                                    ///< 状态
    uint32                  m_closedtime;                               ///< 上次关闭连接的时间
    TransferClientProxy*    m_pInfoClient;                               ///< 中心服务器
};

#endif
