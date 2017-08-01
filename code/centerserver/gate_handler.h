#ifndef GATE_HANDLER_H
#define GATE_HANDLER_H

class AFTransferServerProxy;

class GateHandler
{
public:
    GateHandler();
    ~GateHandler();

    DECLARE_SINGLETON(GateHandler)

    bool Init(const char* ip, uint32 port);

    void RegistRegionID(uint32 centGateID, uint32 sessionID);
    void UnregistRegionID(uint32 sessionID);

    bool IsConnectGate(uint32 serverID);

    uint32  GetRegionIDBySessionID(uint32 sessionID);

    // 接收网络消息
    static void RecvMsg(uint64 playerID, uint32 sessionID, uint32 msgID, void* data, void* userdata);

    void SendByID(uint32 centGateID, uint32 msgID, void* data, uint64 playerID=0);
    void SendToAll(uint32 msgID, void* data, uint64 playerID=0);

    void SendSetPlayerIDReq(uint32 serverID, uint64 loginID, uint64 playerID);
    void SendSetGameSvrIDReq(uint32 serverID, uint64 playerID, uint32 gameSvrID);

    void SendKickOutGame(uint32 gateSvrID, uint64 loginID, uint16 errCode);     // 发送角色踢出游戏的请求
    void SendLoginGameAck(uint16 errCode, uint32 centGateID, uint64 loginID);     // 角色登陆应答
    void SendLoadPlayerAck(uint16 errCode, uint32 serverID, uint64 loginID);    // 角色加载应答
    void SendCreatePlayerAck(uint16 errCode, uint32 serverID, uint64 loginID);
    void SendChangeDungeonAck(uint16 errCode, uint32 serverID, uint64 playerID);
    void SendIsCreatePlayerNameAck(uint16 errCode, uint32 serverID, uint64 loginID);
    void SendAskPlayerAck(ASK_PLAYER_LIST& playerList, uint32 serverID, uint64 loginID);     // 查询角色应答
private:
    void OnAccept(uint32 sessionID, void* data);
    void OnConnect(uint32 sessionID, void* data);
    void OnClose(uint32 sessionID, void* data);
    void OnError(uint32 sessionID, void* data);
    void OnRecv(uint64 player, uint32 sessionID, uint32 msgID, void* data, void* userdata);

    void RecvSetPlayerIDAck(uint32 sessionID, void* data);
    void RecvSetGameSvrIDAck(uint32 sessionID, void* data);

    void RecvLogoutGame(uint32 sessionID, void* data);                      ///< 收到角色的离线请求

    AFTransferServerProxy* m_gateServer;
    std::map<uint32, uint32> m_centGateID2snID;                  // <regionID, sessionID>
};

#endif
