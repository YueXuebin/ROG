#ifndef LOG_HANDLER_H
#define LOG_HANDLER_H

#include "sdu.h"

#include "splitter_factory.h"

class Net_Packet;

class UdpLogHandler
{
public:
    UdpLogHandler();
    ~UdpLogHandler();

    DECLARE_SINGLETON(UdpLogHandler)

    bool Init(const char* ip, uint32 port);

    static void RecvNetPacket(uint32 eventType, uint32 sessionID, Net_Packet* packet, bool& delpacket);     // 接收网络消息
    static void RecvMsg(uint32 sessionID, Net_Packet* packet, bool& delpacket);                             // 接收网络消息

    void Update();
    void Connect();

private:
    enum{
        VALID_BUFF_LEN  = 512,
        MAX_BUFF_LEN    = 1024
    };

    void RecvClientLog(uint32 sessionID, Net_Packet* packet);

private:
    uint32                  m_serverID;             // 服务器ID
    BinarySplitterFactory   m_splitterFactory;      // 拆包器工厂
    bool                    m_NeedReconnect;
    uint32                  m_ClosedTime;       // 上次关闭连接的时间
    char                    m_ip[20];           // IP地址
    uint32                  m_port;             // 端口号
};

#endif
