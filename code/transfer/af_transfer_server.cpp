#include "transfer_pch.h"
#include "af_transfer_server.h"
#include "message_encoder.h"
#include "net_manager.h"

AFTransferServerProxy::AFTransferServerProxy(const char* ip, uint32 port, TransferRecvFunc func, void* userdata)
{
    CMessageEncoder::CreateInstance();
    Net_Manager::CreateInstance();
    m_serverID = Net_Manager::Instance()->create_tcp_server(ip, port, &m_splitter_factory, this, true);
    if(IsValid())
        SetRecvFunc(func, userdata);
}

AFTransferServerProxy::~AFTransferServerProxy()
{
    m_serverID = 0;
    m_recvfunc = 0;
    m_userdata = 0;
    Net_Manager::DestroyInstance();
    CMessageEncoder::DestroyInstance();
}

void AFTransferServerProxy::SetRecvFunc(TransferRecvFunc func, void* userdata)
{
    m_recvfunc = func;
    m_userdata = userdata;
}

void AFTransferServerProxy::Recv(uint64 playerID, uint32 sessionID, uint32 msgID, void* data)
{
    switch(msgID)
    {
    case MSG_NET_ACCEPT:
    case MSG_NET_CONNECT:
        break;
    case MSG_NET_CLOSE:
    case MSG_NET_ERROR:
        break;
    }
    m_recvfunc(playerID, sessionID, msgID, data, m_userdata);
}

bool AFTransferServerProxy::Send(uint32 sessionID, uint32 msgID, void* data, uint64 playerID)
{
    return TransferSend(sessionID, msgID, data, playerID, this);
}

void AFTransferServerProxy::Close(uint32 sessionID)
{
    Net_Manager::Instance()->delete_net(sessionID);
}
