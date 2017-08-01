#ifndef GATESERVER_H
#define GATESERVER_H


class TransferClientProxy;
class TransferServerProxy;

class Net_Packet;
typedef void (*RecvNetPacket)(uint32 eventType, uint32 sessionID, Net_Packet* packet, bool& delpacket);

class GateServer
{
public:
    GateServer();       // 构造
    ~GateServer();      // 析构

    DECLARE_SINGLETON(GateServer)

    bool Init(uint32 id, const char* path);                             // 初始化
    void UnInit();      // 销毁

    void Run();         // 运行
    void UpdateNet();                                                   // 更新网络

    uint64 GenLoginUUID();

private:    
    bool ConnectToGame(uint32 id, const char* ip, uint32 port);         // 连接游戏服务器

    void SendToCenter(uint32 msgID, void* data);                        // 给中心服务器发送消息

    void BeforeCloss();
    void BeforeCloseUpdate();

    uint16 m_serialNum;                                                 // 序列号
    std::map<uint32, TransferClientProxy*> m_gameProxy;                 // 游戏服务器代理
};

#endif
