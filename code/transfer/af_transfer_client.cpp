#include "transfer_pch.h"
#include "af_transfer_client.h"
#include "net_manager.h"
#include "message_encoder.h"
#include "af_encrypt.h"

AFTransferClientProxy::AFTransferClientProxy(const char* ip, uint32 port, TransferRecvFunc func, void* userdata, bool useBigQueue)
{
    m_Encryptor = NULL;

    CMessageEncoder::CreateInstance();
    Net_Manager::CreateInstance();
    Crown::SDStrncpy(m_ip, ip, 20);
    m_port = port;
    m_clientID = Net_Manager::Instance()->create_tcp_client(ip, port, &m_splitterFactory, this, 10, useBigQueue);
    SetRecvFunc(func, userdata);

    m_AutoReconnect = false;
    m_NeedReconnect = false;
    m_CloseTime = 0;
}

AFTransferClientProxy::~AFTransferClientProxy()
{
    m_clientID = 0;
    m_recvfunc = 0;
    m_userdata = 0;
    Net_Manager::DestroyInstance();
    CMessageEncoder::DestroyInstance();
}

void AFTransferClientProxy::SetRecvFunc(TransferRecvFunc func, void* userdata)
{
    m_recvfunc = func;
    m_userdata = userdata;
}

void AFTransferClientProxy::Recv(uint64 playerID, uint32 sessionID, uint32 msgID, void* data)
{
    switch(msgID)
    {
    case MSG_NET_CONNECT:
        break;
    case MSG_NET_CLOSE:
    case MSG_NET_ERROR:
        Net_Manager::Instance()->delete_net(m_clientID);
        m_clientID = 0;
        if(m_AutoReconnect)
        {
            m_CloseTime = Crown::SDGetTickCount();
            m_NeedReconnect = true;
        }
        break;
    default:
        break;
    }
    m_recvfunc(playerID, sessionID, msgID, data, m_userdata);
}

bool AFTransferClientProxy::Send(uint32 msgID, void* data, uint64 playerID)
{
    return TransferSend(m_clientID, msgID, data, playerID, this);
}

bool AFTransferClientProxy::SendPacket(Net_Packet* packet)
{
    return TransferSendPacket(m_clientID, packet, this);
}

void AFTransferClientProxy::Connect(bool AutoReconnect, uint32 ReconnectTime)
{
    m_AutoReconnect = AutoReconnect;
    m_ReconnectTime = ReconnectTime;
    if(m_clientID) return;

    Reconnect();
}

void AFTransferClientProxy::Reconnect()
{
    m_clientID = Net_Manager::Instance()->create_tcp_client(m_ip, m_port, &m_splitterFactory, this, 10, true);
    if (0 == m_clientID)
    {
        // 在主循环中尝试重新连接
        //printf("create_tcp_client error \n");
    }
}

void AFTransferClientProxy::Close()
{
    if(m_clientID)
    {
        Net_Manager::Instance()->delete_net(m_clientID);
        m_clientID = 0;
    }

    m_AutoReconnect = false;
    m_NeedReconnect = false;
}

void AFTransferClientProxy::Update()
{
    // 自动重连处理
    if(m_NeedReconnect && m_clientID == 0 && m_AutoReconnect)
    {
        uint32 now = Crown::SDGetTickCount();
        if(now - m_CloseTime >= m_ReconnectTime)
        {
            // 重连
            Reconnect();
            m_NeedReconnect = false;
        }
    }
    
}

void AFTransferClientProxy::SetEncrpty(Encrypt* pEncryptor)
{
    m_Encryptor = pEncryptor;
}

void AFTransferClientProxy::BeforeDecode(Net_Packet* packet)
{
    if(m_Encryptor)
        m_Encryptor->DecodeByPubKey(packet);
}

void AFTransferClientProxy::AfterEncode(Net_Packet* packet)
{
    if(m_Encryptor)
        m_Encryptor->EncodeByPriKey(packet);
}

