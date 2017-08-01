#ifndef FLASH_HANDLER_H
#define FLASH_HANDLER_H

#include "sdu.h"

#include "splitter_factory.h"

class Net_Packet;

class FlashHandler
{
public:
    FlashHandler();
    ~FlashHandler();

    DECLARE_SINGLETON(FlashHandler)

    bool Init(const char* ip, uint32 port);

    static void RecvNetPacket(uint32 eventType, uint32 sessionID, Net_Packet* packet, bool& delpacket);     ///< 接收网络消息
    static void RecvMsg(uint32 sessionID, Net_Packet* packet, bool& delpacket);                             ///< 接收网络消息
    
    void Update();
    void Connect();

private:
    void OnAccept(uint32 sessionID);
    void OnClose(uint32 sessionID);
    void OnError(uint32 sessionID);

    void SendFlashVerify(uint32 sessionID);

    uint32              m_serverID;         ///< 服务器ID
    BinarySplitterFactory   m_splitterFactory;  ///< 拆包器工厂
    bool                    m_NeedReconnect;
    uint32                  m_closedtime;       // 上次关闭连接的时间
    char                    m_ip[20];           // IP地址
    uint32                  m_port;             // 端口号
};

#endif
