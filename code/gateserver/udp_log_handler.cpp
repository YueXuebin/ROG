#include "gateserver_pch.h"
#include "udp_log_handler.h"
#include "net_manager.h"
#include "message_encoder.h"
#include "center_handler.h"

IMPLEMENT_SINGLETON(UdpLogHandler)

UdpLogHandler::UdpLogHandler()
{
    m_serverID = 0;
    m_NeedReconnect = false;
    m_ClosedTime = 0;
    memset(m_ip, 0, sizeof(m_ip));
    m_port = 0;
}

UdpLogHandler::~UdpLogHandler()
{
    
}

bool UdpLogHandler::Init(const char* ip, uint32 port)
{
    CnInfo("listen udp log %s:%d\n", ip, port);
    // 创建网络连接
    Crown::SDStrncpy(m_ip, ip, 20);
    m_port = port;
    Connect();
    
    return true;
}

void UdpLogHandler::Connect()
{
    m_serverID = Net_Manager::Instance()->create_udp(m_ip, m_port, (void*)&RecvNetPacket);
    if (m_serverID == 0)
    {
        m_ClosedTime = Crown::SDGetTickCount();
        m_NeedReconnect = true;
    }
    else
    {
        CnInfo("listen udp ok %s:%d\n", m_ip, m_port);
    }
}

void UdpLogHandler::RecvNetPacket(uint32 eventType, uint32 sessionID, Net_Packet* packet, bool& delpacket)
{
    switch(eventType)
    {
    case TYPE_DATA:
        RecvMsg(sessionID, packet, delpacket);
        break;
    default:
        break;
    }
}

void UdpLogHandler::RecvMsg(uint32 sessionID, Net_Packet* packet, bool& delpacket)
{
    Instance()->RecvClientLog(sessionID, packet);
}

void UdpLogHandler::RecvClientLog(uint32 sessionID, Net_Packet* packet)
{
    LOG_REQ sendMsg;
    
    //CnDbg("recv log\n");

    // 检查有效缓存长度
    if(packet->getdatalen() > VALID_BUFF_LEN)
    {
        CnDbg("recv log too long\n");
        return;
    }

    packet->read_string(sendMsg.log, packet->getdatalen());

    if(sendMsg.log.length() > 120)
        sendMsg.log = sendMsg.log.substr(0, 120);

    sendMsg.idflag = LOG_CLIENT_UDP;

    CenterHandler::Instance()->Send(MSG_LOG_REQ, &sendMsg);
}

void UdpLogHandler::Update()
{
    if(m_NeedReconnect && (Crown::SDGetTickCount() - m_ClosedTime) > 10000)
    {
        m_NeedReconnect = false;
        Connect();
    }
}