#include "transfer_pch.h"
#include "real_transfer_client.h"
#include "transfer_socket.h"
#include "protocol_define.h"
#include "real_transfer_server.h"

#ifdef WIN32

int32 g_ClientCount = 0;	// client connect count

class FakeServer : public RealTransferServer
{
public:

    virtual void SendRawData(uint32 sessionID, char* data, uint32 dataLen)
    {
        pClient->SendRawData(data, dataLen);
    }

    RealTransferClient* pClient;

};

void FakeRecvFunc(uint32 SessionId, uint32 MsgId, void* pData, void* pUserData)
{

    ((RealTransferClient*)pUserData)->OnReceive(MsgId, pData);
}

RealTransferClient::RealTransferClient() :
m_pSdSock(NULL),
m_pRecvFunc(NULL),
m_pUserDate(NULL)
{
    m_pDecoder = CnNew CMessageEncoder();

    g_ClientCount++;
    if(!Crown::CSDAsyncSocketManager::Instance())
    {
        WSADATA data;
        int err = WSAStartup(MAKEWORD(2, 2), &data);
        CnVerify(!err);

        Crown::CSDAsyncSocketManager::CreateInstance();
        bool ret = false;
        ret = Crown::CSDAsyncSocketManager::Instance()->Init();
        CnVerify(ret);
    }
}

RealTransferClient::~RealTransferClient()
{
    if(m_pSdSock)
        CnDelete m_pSdSock;

    g_ClientCount--;
    if(g_ClientCount==0)
    {
        Crown::CSDAsyncSocketManager::Instance()->Uninit();
        Crown::CSDAsyncSocketManager::DestroyInstance();

        int err = WSACleanup();
        CnVerify(!err);
    }

}

void RealTransferClient::SetRecvFunc( CliRecvFunc pRecvFunc, void* pUserData )
{
    CnVerify(!m_pRecvFunc);
    m_pRecvFunc = pRecvFunc;
    m_pUserDate = pUserData;

    bool ret = m_pSdSock->Connect(m_pConnectIpStr.c_str(), m_Port);
    CnVerify(ret);
}

bool RealTransferClient::Init( const char* ip, int32 port )
{
    if(ip)
        m_pConnectIpStr = ip;
    else
        m_pConnectIpStr = "127.0.0.1";

    if(port == 0)
        m_Port = GAME_NET_PORT;
    else
        m_Port = port;

    m_pSdSock = CnNew TransferSocket(this);
    bool ret = m_pSdSock->Create();
    CnVerify(ret);
    if(!ret)
        return false;

    return true;
}

void RealTransferClient::Update()
{
    CnVerify(Crown::CSDAsyncSocketManager::Instance()->GetHWnd());
    MSG kMsg;
    while (PeekMessage(&kMsg, Crown::CSDAsyncSocketManager::Instance()->GetHWnd(), 0, 0, PM_REMOVE))
    {
        TranslateMessage(&kMsg);
        DispatchMessage(&kMsg);
    }
}

bool RealTransferClient::Send( uint32 MsgId, void* pData )
{
    return true;
}

void RealTransferClient::SendRawData(char* data, uint32 dataLen)
{
    m_pSdSock->Send(data, dataLen);
//=======
//    m_pEncoder->Encode(0, MsgId, pData);
//	//return m_pSdSock->SendMsg( MsgId, pData );
//    return true;
//>>>>>>> master
}

void RealTransferClient::OnReceive( uint32 MsgId, void* pData )
{
    if( m_pRecvFunc )
        m_pRecvFunc( MsgId, pData, m_pUserDate );
    else
        CnVerify(false);
}

#endif // WIN32

