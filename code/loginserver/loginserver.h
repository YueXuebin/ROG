#ifndef LOGINSERVER_H
#define LOGINSERVER_H

#include "sdu.h"
#include "message_encoder.h"

class RealTransferServer;

class LoginServer
{
public:
    LoginServer();       // 构造
    ~LoginServer();      // 析构

    DECLARE_SINGLETON(LoginServer)

    bool Init();        // 初始化
    void UnInit();      // 销毁

    void Run();         // 运行

    static void ProcMsg(uint32 sessionID, uint32 msgID, void* msg);     // 处理消息

    void Send(uint32 sessionID, uint32 msgID, void* data)
    {
        if(m_encoder) m_encoder->Encode(sessionID, msgID, data);
    }

private:
    CMessageEncoder*    m_encoder;      // 消息解码编码器
    RealTransferServer* m_transfer;     // 传输层服务端
};

#endif
