#pragma once

/*
*	服务端的一个连接，继承使用
*/
#ifndef Session_h__
#define Session_h__

#include "message_encoder.h"

class RealTransferServer;

class Session : public Crown::ISDSession
{
public:
    Session(void);
    virtual ~Session(void);

    virtual void SDAPI SetConnection(Crown::ISDConnection* poConnection);
    virtual void SDAPI OnEstablish(void);
    virtual void SDAPI OnTerminate(void);
    virtual bool SDAPI OnError(int32 nModuleErr, int32 nSysErr);
    virtual void SDAPI OnRecv(const char* pBuf, uint32 dwLen);
    virtual void SDAPI Release(void);

    bool Send(char* data, uint32 dataLen);

    Crown::ISDConnection* GetConnection() {return m_poConnection;}

    void SetID(uint32 id);
    uint32 GetID() { return m_Id; }

    void SetTransfer(RealTransferServer* val) { m_pServerProxy = val; }
    CMessageEncoder* GetDecoder() { return m_poDecoder; }
    void SetDecoder(CMessageEncoder* val) { m_poDecoder = val; }
    //void SetRecvFunc(SrvRecvFunc val) { m_recvFunc = val; }

private:
    Crown::ISDConnection*   m_poConnection;
    CMessageEncoder*        m_poDecoder;
    uint32                  m_Id;               // 该Session的唯一标识id号，等于在SessionManager中的数组下标
    RealTransferServer*     m_pServerProxy;
    //SrvRecvFunc             m_recvFunc;
};

#endif // Session_h__
