#ifndef GAME_HANDLER_H
#define GAME_HANDLER_H

#include "sdu.h"
#include "splitter_factory.h"

class Net_Packet;

class GameHandler
{
    struct GameServerInfo
    {
        uint32  serverID;             // gameserverID
        uint32  centerID;
        std::string ip;
        uint32  port;
        uint32  sessionID;
        uint32  state;
        bool    needReconnect;
        uint32  closedtime;
    };

public:
    GameHandler();
    ~GameHandler();

    DECLARE_SINGLETON(GameHandler)

    bool Connect(uint32 serverID,uint32 centerID, const char* ip, uint32 port);
    void Update();

    uint32 GetSessionID(uint32 gameSvrID,uint32 centerID);

    static void RecvNetPacket(uint32 eventType, uint32 sessionID, Net_Packet* packet, bool& delpacket);     // 接收网络消息
    static void RecvMsg(uint32 sessionID, Net_Packet* packet, bool& delpacket);                             // 接收网络消息

    bool Send(uint32 sessionID, uint32 msgID, void* data, uint64 playerID = 0);
private:
    void Reconnect(GameServerInfo* pInfo);

    void OnConnect(uint32 sessionID);
    void OnClose(uint32 sessionID);
    void OnError(uint32 sessionID);
    void OnDisconnectGame(uint32 sessionID);

    static void OnRecv(uint64 playerID, uint32 sessionID, uint32 msgID, void* data, void* userdata);

    void RecvServerGateIDReq(uint32 sessionID, void* data);
    void SendServerGateIDAck(uint32 sessionID);

private:
    typedef std::map<uint32, GameServerInfo*> GameServerInfoMap;        // <sessionID, GameServerInfo>
    GameServerInfoMap       m_GameServerInfos;

    AfPacketSplitterFactory m_splitterFactory;      ///< 拆包器工厂
};

#endif
