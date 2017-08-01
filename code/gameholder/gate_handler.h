#ifndef GATE_HANDLER_H
#define GATE_HANDLER_H

class TransferServerProxy;
class Player;
class Scene;

class GateHandler
{
public:
    GateHandler();
    ~GateHandler();

    DECLARE_SINGLETON(GateHandler)

    bool Init(const char* ip, uint32 port);

    // 接收网络消息
    static void RecvMsg(uint64 playerID, uint32 sessionID, uint32 msgID, void* data, void* userdata);

    void SendChangeDungeonAck(uint16 errCode, Player* player, Scene* scene);

    bool SendByGateID(uint32 gateID, uint32 msgID, void* data, uint64 playerID);
    bool SendByCenterGateID(uint32 centerID, uint32 gateID, uint32 msgID, void* data, uint64 playerID=0);
    void SendToAll(uint32 msgID, void* data, uint64 playerID = 0);

    bool IsValid() {return m_gateServer != NULL;}
    const std::string& GetListenIp() {return m_ListenIp;}
    uint32  GetListenPort() {return m_ListenPort;}

    void CloseAll();       // 断开所有Gate
    void Close(uint32 closeCenterId);        // 断开属于某center的所有gate
private:
    void OnAccept(uint32 sessionID, void* data);
    void OnConnect(uint32 sessionID, void* data);
    void OnClose(uint32 sessionID, void* data);
    void OnError(uint32 sessionID, void* data);
    void OnRecv(uint64 playerID, uint32 sessionID, uint32 msgID, void* data, void* userdata);

    void UnregisterServer(uint32 sessionID);

    void SendServerGateIDReq(uint32 sessionID);
    void RecvServerGateIDAck(uint32 sessionID, void* data);

    bool IsValidClientMsgID(uint32 msgID);          // 判断从客户端传递过来的消息ID是否有效（因为客户端可能发送过来服务器消息）

private:
    TransferServerProxy*        m_gateServer;
    std::map<uint32, uint32>    m_sessionMap;       // <regionID, sessionID>
    std::set<uint32>            m_clientMsgID;      // 保存客户端可以使用的消息ID号

    std::string                 m_ListenIp;         // 监听Ip
    uint32                      m_ListenPort;       // 监听端口
};

#endif
