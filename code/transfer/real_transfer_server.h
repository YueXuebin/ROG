#pragma once

/*
*   真传输层服务端
*/
#ifndef RealTransferServer_h__
#define RealTransferServer_h__

#include "transfer.h"
#include "protocol_define.h"

class CMessageEncoder;

class RealTransferServer : public TransferServerProxy
{
public:
    RealTransferServer();
    virtual ~RealTransferServer();

    bool                Init( const char* ip = 0, int32 port = GAME_NET_PORT );

    virtual bool        Send(uint32 SessionId, uint32 MsgId, void* pData);

    void                SendRawData(uint32 sessionID, char* data, uint32 dataLen);

    virtual             TransferRecvFunc GetRecvFunc() { return 0; }
    virtual void*       GetUserData() { return 0; }

    virtual void        SetRecvFunc( SrvRecvFunc pRecvFunc, void* pUserData );

    //SrvRecvFunc         GetRecvFunc() { return m_pRecvFunc; }
    //void*               GetUserData() { return m_pUserData; }

    CMessageEncoder*    GetDecoder() { return m_pDecoder; }
    void                SetDecoder(CMessageEncoder* val)
    { 
        m_pDecoder = val; 
    }

    virtual void        DeleteSession(uint32 sessionId);
    virtual bool        HasSession(uint32 sessionId) const;
    const char*         GetSessionIP(uint32 sessionID) const;       ///< 获得连接的IP地址

    virtual void        Update();

private:
    SrvRecvFunc         m_pRecvFunc;
    void*               m_pUserData;
    CMessageEncoder*    m_pDecoder;
};


#endif // RealTransferServer_h__
