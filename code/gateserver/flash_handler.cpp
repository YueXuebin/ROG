#include "gateserver_pch.h"
#include "flash_handler.h"
#include "net_manager.h"

IMPLEMENT_SINGLETON(FlashHandler)

FlashHandler::FlashHandler()
{
    m_serverID = 0;
    m_NeedReconnect = false;
    m_closedtime = 0;
    memset(m_ip, 0, sizeof(m_ip));
    m_port = 0;
}

FlashHandler::~FlashHandler()
{

}

bool FlashHandler::Init(const char* ip, uint32 port)
{
    CnInfo("listen flash %s:%d\n", ip, port);
    // 创建网络连接
    Crown::SDStrncpy(m_ip, ip, 20);
    m_port = port;
    Connect();
    return true;
}

void FlashHandler::RecvNetPacket(uint32 eventType, uint32 sessionID, Net_Packet* packet, bool& delpacket)
{
    switch(eventType)
    {
    case TYPE_ACCEPT:
        CnInfo("FlashHandler OnAccept %s:%u\n", Crown::GetIpStr(packet->remote_addr.get_addr()).c_str(), packet->remote_addr.get_port());
        FlashHandler::Instance()->OnAccept(sessionID);
        break;
    case TYPE_CLOSE:
        CnInfo("FlashHandler OnClose %s:%u\n", Crown::GetIpStr(packet->remote_addr.get_addr()).c_str(), packet->remote_addr.get_port());
        FlashHandler::Instance()->OnClose(sessionID);
        break;
    case TYPE_EXCEPTION:
        FlashHandler::Instance()->OnError(sessionID);
        break;
    default:
        RecvMsg(sessionID, packet, delpacket);
        break;
    }
}

void FlashHandler::RecvMsg(uint32 sessionID, Net_Packet* packet, bool& delpacket)
{
    Instance()->SendFlashVerify(sessionID);
    CnInfo("flash verify ip=%s port=%d\n", Crown::GetIpStr(packet->remote_addr.get_addr()).c_str(), packet->remote_addr.get_port());
}

void FlashHandler::OnAccept(uint32 sessionID)
{
    
}

void FlashHandler::OnClose(uint32 sessionID)
{

}

void FlashHandler::OnError(uint32 sessionID)
{

}

void FlashHandler::SendFlashVerify(uint32 sessionID)
{
    const std::string verifyCode = "<?xml version=\"1.0\"?>"
                                   "<cross-domain-policy>"
                                   "<allow-access-from domain=\"*\" to-ports=\"*\" />"
                                   "</cross-domain-policy>";

    Net_Packet* packet = new Net_Packet;

    packet->write_data(verifyCode.c_str(), verifyCode.length());

    // 发送策略信息
    if(Net_Manager::Instance()->send_packet(sessionID, packet) != 0)
    {
        delete packet;
    }

    // 主动关闭网络连接
    Net_Manager::Instance()->delete_net(sessionID);
}

void FlashHandler::Update()
{
    if(m_NeedReconnect && (Crown::SDGetTickCount() - m_closedtime) > 10000)
    {
        m_NeedReconnect = false;
        Connect();
    }
}

void FlashHandler::Connect()
{
    m_serverID = Net_Manager::Instance()->create_tcp_server(m_ip, m_port, &m_splitterFactory, (void*)&RecvNetPacket, true);
    if(m_serverID == 0)
    {
        CnInfo("listen flash %s:%d failed\n", m_ip, m_port);
        m_closedtime = Crown::SDGetTickCount();
        m_NeedReconnect = true;
    }
    else
    {
        CnInfo("listen flash %s:%d ok\n", m_ip, m_port);
    }
}