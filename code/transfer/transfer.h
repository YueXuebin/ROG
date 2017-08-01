#ifndef Transfer_h__
#define Transfer_h__

#include "sdu.h"
#include "message_encoder.h"		// 协议

class Encrypt;

// 开启flash验证线程
void InitFlashCheckThread();

// 无论是客户端还是服务器都使用这个回调函数
typedef void (*TransferRecvFunc)(uint64 playerID, uint32 sessionID, uint32 msgID, void* data, void* userdata);

// 客户端代理的Recv函数
typedef void (*CliRecvFunc)(uint32 MsgId, void* pData, void* pUserData);

class TransferProxy
{
public:
    TransferProxy();
    virtual ~TransferProxy();
    virtual void SetRecvFunc(TransferRecvFunc func, void* userdata) = 0;
    virtual void Recv(uint64 playerID, uint32 sessionID, uint32 msgID, void* data) = 0;
    virtual bool IsValid() = 0;

    virtual void Update() {}

    virtual void BeforeDecode(Net_Packet* packet) {}
    virtual void AfterEncode(Net_Packet* packet) {}

    virtual bool SendPacket(Net_Packet* packet) {return false;}
};

class TransferClientProxy : public TransferProxy
{
public:
    TransferClientProxy() { }
    virtual ~TransferClientProxy() {}

    virtual bool Send(uint32 msgID, void* data, uint64 playerID = 0) = 0;

    virtual void Connect(bool AutoReconnect = false, uint32 ReconnectTime = 5000) = 0;      // n秒自动重连
    virtual void Close() = 0;

    virtual void SetEncrpty(Encrypt* pEncryptor) {}     // 设置加密器
};

// 服务端代理的Recv函数
typedef void (*SrvRecvFunc)(uint32 SessionId, uint32 MsgId, void* pData, void* pUserData);

class TransferServerProxy : public TransferProxy
{
public:
    TransferServerProxy() { }
    virtual ~TransferServerProxy() {}

    virtual bool Send(uint32 sessionID, uint32 msgID, void* pData, uint64 playerID = 0) = 0;

    virtual void Close(uint32 sessionID) = 0;

};

// ================================================================================
TransferClientProxy* CreateTransferClientProxy(const char* ip, int32 port, TransferRecvFunc func, void* userdata, bool useBigQueue=true);
TransferServerProxy* CreateTransferServerProxy(const char* ip, int32 port, TransferRecvFunc func, void* userdata);
void DestoryTransferClientProxy(TransferClientProxy* pProxy);
void DestoryTransferServerProxy(TransferServerProxy* pProxy);
void TransferUnpdate();
int32 TransferSend(uint32 sessionID, uint32 msgID, void* data, uint64 playerID=0, TransferProxy* pProxy=NULL);
bool TransferSendPacket(uint32 sessionID, Net_Packet* packet, TransferProxy* pProxy=NULL);

// record protocol data into a file, empty string denotes not recording.
extern std::string g_ProtoRecord;

#endif // Transfer_h__
