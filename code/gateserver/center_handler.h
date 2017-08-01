#ifndef CENTER_HANDLER_H
#define CENTER_HANDLER_H

#include "afpacket_splitter.h"

class CenterHandler
{
public:
    CenterHandler();
    ~CenterHandler();

    DECLARE_SINGLETON(CenterHandler)

    bool Init(const char* ip, uint32 port);

    void Update();          // 检查状态
    

    static void RecvNetPacket(uint32 eventType, uint32 sessionID, Net_Packet* packet, bool& delpacket);
    static void RecvMsg(uint32 sessionID, Net_Packet* packet, bool& delpacket);

    bool Send(uint32 msgID, void* data, uint64 playerID=0);
    bool Send(Net_Packet* packet);

    uint32 GetState() { return m_state; }

    void SendLogoutGame(uint64 loginID);

private:
    void Connect();         // 连接网络

    void OnConnect(uint32 sessionID);
    void OnClose(uint32 sessionID);
    void OnError(uint32 sessionID);
    static void OnRecv(uint64 playerID, uint32 sessionID, uint32 msgID, void* data, void* userdata);

    void SendServerInfoReq();                                           // 发送请求服务器信息
    void RecvServerInfoAck(void* data);                                 // 接收服务器信息

    void SendServerActiveReq();                                         // 发送请求服务器激活
    void RecvServerActiveAck(void* data);                               // 接收服务器激活

    void RecvServerListAck(void* data);                                 // 接受服务器列表

    void RecvServerReadyAck(void* data);                                // 接受所有服务器准备完毕通知

    void RecvLoginGameAck(uint64 loginID, void* data);                  // 收到登陆游戏错误消息
    
    void RecvLoadPlayerAck(uint64 loginID, void* data);                 // 收到加载角色错误消息

    void RecvKickoutGame(uint64 loginID, void* data);                   // 收到踢出游戏消息

    void RecvSetPlayerIDReq(void* data);                                        // 请求设置玩家ID
    void SendSetPlayerIDAck(uint16 errCode, uint64 loginID, uint64 playerID);   // 回复设置玩家ID

    void RecvSetGameSvrIDReq(void* data);                               // 请求设置游戏服务器ID
    void SendSetGameSvrIDAck(uint16 errCode, uint64 playerID);          // 回复试着游戏服务器ID

private:
    uint32                  m_state;            // 状态
    bool                    m_NeedReconnect;
    uint32                  m_closedtime;       // 上次关闭连接的时间

    char                    m_ip[20];           // IP地址
    uint32                  m_port;             // 端口号
    uint32                  m_clientID;         // 客户端ID
    AfPacketSplitterFactory m_splitter_factory; // 拆包器工厂
};

#endif
