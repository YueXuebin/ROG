#ifndef CLIENT_HANDLER_H
#define CLIENT_HANDLER_H

#include "sdu.h"

#include "splitter_factory.h"

class Net_Packet;

class ClientHandler
{
public:
    ClientHandler(); 
    ~ClientHandler();

    DECLARE_SINGLETON(ClientHandler)

    bool Init(const char* ip, uint32 port);

    static void RecvNetPacket(uint32 eventType, uint32 sessionID, Net_Packet* packet, bool& delpacket);     ///< 接收网络消息
    static void RecvMsg(uint32 sessionID, Net_Packet* packet, bool& delpacket);                             ///< 接收网络消息

private:
    void OnAccept(uint32 sessionID, uint32 addrIP, uint16 addrPort);
    void OnClose(uint32 sessionID, uint32 addrIP, uint16 addrPort);
    void OnError(uint32 sessionID, uint32 addrIP, uint16 addrPort);
    static void OnRecv(uint64 playerID, uint32 sessionID, uint32 msgID, void* data, void* userdata);

    void RecvLoginGameReq(uint32 sessionID, void* data);

    void RecvLoadPlayerReq(uint32 sessionID, void* data);

    void RecvEncryptPrivateKeyNtf(uint32 sessionID, void* data);

    uint32              m_serverID;         ///< 服务器ID
    AfPacketSplitterFactory m_splitterFactory;  ///< 拆包器工厂

    bool CheckMsgCount(uint32 sessionId, uint8 count); 
    void RemoveCount(uint32 sessionId);
    std::map<uint32, uint8> m_MsgCounterMap;
};

#endif
