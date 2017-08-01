/*
*	真实传输层客户端
*/

#ifndef RealTransferClient_h__
#define RealTransferClient_h__

#ifdef WIN32

#include "transfer.h"

class TransferSocket;
class CMessageEncoder;


class RealTransferClient : public TransferClientProxy
{
    friend class TransferSocket;
public:
    RealTransferClient();
    virtual ~RealTransferClient();

    bool                Init(const char* ip, int32 port);

    virtual bool        Send(uint32 MsgId, void* pData);

    void                SendRawData(char* data, uint32 dataLen);

    TransferRecvFunc GetRecvFunc() { return 0; }

    virtual void        SetRecvFunc(CliRecvFunc pRecvFunc, void* pUserData);
    //CliRecvFunc         GetRecvFunc() { return m_pRecvFunc; }

    void*               GetUserData() { return m_pUserDate; }

    CMessageEncoder*    GetDecoder() { return m_pDecoder; }
    //void                SetDecoder(CMessageEncoder* val) { m_pDecoder = val; }

    virtual void Update();

    void    OnReceive(uint32 MsgId, void* pData);
private:
    

    std::string         m_pConnectIpStr;
    int32               m_Port;

    TransferSocket*     m_pSdSock;			//  收发网络包的对象

    CliRecvFunc         m_pRecvFunc;
    void*               m_pUserDate;
    CMessageEncoder*    m_pDecoder;
};

#endif // WIN32

#endif // RealTransferClient_h__
