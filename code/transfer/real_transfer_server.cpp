#include "transfer_pch.h"
#include "real_transfer_server.h"
#include "message_encoder.h"
#include "session_manager.h"


RealTransferServer::RealTransferServer() :
m_pRecvFunc(0),
m_pUserData(0)
{
    m_pDecoder = CnNew CMessageEncoder();
    //m_pDecoder->SetTransfer(this);

    SessionManager::Init();
    SessionManager::GetInstance()->SetServerProxy(this);
}

RealTransferServer::~RealTransferServer()
{
    SessionManager::Release();

    CnDelete m_pDecoder;
}

void RealTransferServer::SetRecvFunc( SrvRecvFunc pRecvFunc, void* pUserData )
{
    m_pRecvFunc = pRecvFunc;
    m_pUserData = pUserData;
}

bool RealTransferServer::Init( const char* ip, int32 port )
{
    return SessionManager::GetInstance()->StartListen(ip, port);
}

bool RealTransferServer::Send( uint32 SessionId, uint32 MsgId, void* pData )
{
    BT_PROFILE_FUNC;

    //m_pDecoder->Encode(SessionId, MsgId, pData);
    return true;
}

void RealTransferServer:: SendRawData(uint32 sessionID, char* data, uint32 dataLen)
{
    SessionManager::GetInstance()->Send(sessionID, data, dataLen);
}

void RealTransferServer::Update()
{
    BT_PROFILE_FUNC;
    SessionManager::GetInstance()->Update();
}

void RealTransferServer::DeleteSession(uint32 sessionId)
{
    SessionManager::GetInstance()->ReleaseSession(sessionId);
}

bool RealTransferServer::HasSession(uint32 sessionId) const
{
    return SessionManager::GetInstance()->HasSession(sessionId);
}

const char* RealTransferServer::GetSessionIP(uint32 sessionID) const
{
    Session* session = SessionManager::GetInstance()->GetSession(sessionID);
    if(!session) return nullptr;

    Crown::ISDConnection* connection = session->GetConnection();
    if(!connection) return nullptr;

    return connection->GetRemoteIPStr();
}
