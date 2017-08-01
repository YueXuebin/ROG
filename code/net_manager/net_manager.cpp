#include "net_manager_pch.h"
#include "net_manager.h"
#include "sock_dgram.h"
#include "sock_connector.h"
#include "sock_acceptor.h"
#include "log.h"

Net_Manager* Net_Manager::m_instance = 0;
uint32_t Net_Manager::m_reference_count = 0;

// class Net_Manager
Net_Manager::Net_Manager()
{
    m_status = 0;
    m_net_thread = NULL;
    m_net_event_queue = CnNew Net_Event_Queue;
    m_send_packet_queue = CnNew Net_Send_Packet_Queue;
}

Net_Manager::~Net_Manager()
{
    stop();     // 释放前，先结束剩下的事件处理
    CnDelete m_net_event_queue;
    CnDelete m_send_packet_queue;
}

int Net_Manager::putCount = 0;
int Net_Manager::getCount = 0;

int Net_Manager::start()
{
#ifdef _WIN32
    WORD wVersionRequested;
    WSADATA wsaData;
    int err;
    wVersionRequested = MAKEWORD( 2, 2 );
    err = WSAStartup( wVersionRequested, &wsaData );
    if ( err != 0 ) {
        return 1;
    }
    if ( LOBYTE( wsaData.wVersion ) != 2 ||
        HIBYTE( wsaData.wVersion ) != 2 ) {
            WSACleanup( );
            return -1; 
    }
#endif 

    if (0 != m_status)
    {
        CnError("Net_Manager::start error, already started\n");
        return -1;
    }

    m_reactor.initHandlerTable();

    int rc = m_reactor.open_reactor();
    if (0 != rc) 
    {
        CnError("Net_Manager::start error, open reactor error\n");
        return -1;
    }

    m_net_thread = CnNew Net_Thread(this, &m_reactor);

    rc = m_net_thread->start();
    if (0 != rc) 
    {
        CnError("Net_Manager::start error, start thread error\n");
        CnDelete m_net_thread;
        m_net_thread = NULL;
        m_reactor.close_reactor();
        return -1;
    }

    m_status = 1;
    return 0;
}

int Net_Manager::stop()
{
    if (0 == m_status) 
    {
        CnError("Net_Manager::stop error, not started\n");
        return -1;
    }

    int rc = m_net_thread->stop();
    if (0 != rc) 
    {
        CnError("Net_Manager::stop error, stop thread error\n");
        return -1;
    }

    CnDelete m_net_thread;
    m_net_thread = NULL;

    m_reactor.end_reactor_event_loop();
    m_reactor.close_reactor();

    //清除未发送分发完的数据包
    Net_Packet* pNetPacket;
    while(m_send_packet_queue->read(pNetPacket))
    {
        delete pNetPacket;
    }

    //清除未提交给用户的网络事件
    Net_Event* pNetEvent;
    while(m_net_event_queue->read(pNetEvent))
    {
        delete pNetEvent;
    }

    m_reactor.finitHandlerTable();

    m_status = 0;
    return 0;
}

uint32_t Net_Manager::create_udp(const INET_Addr& local_addr, void* pUserData, int netbufsize)
{
    if (1 != m_status)
    {
        CnWarn("Net_Manager::create_udp %s:%u error, not started\n", Crown::GetIpStr(local_addr.get_addr()).c_str(), ntohs(local_addr.get_port()));
        return 0;
    }

    int rc;

    SOCK_Dgram *dgram = CnNew SOCK_Dgram(this, pUserData);
    uint32_t id = dgram->get_id();

    rc = dgram->create_udp(local_addr, netbufsize);
    if (0 != rc) 
    {
        CnWarn("Net_Manager::create_udp %s:%u error, create_udp error\n", Crown::GetIpStr(local_addr.get_addr()).c_str(), ntohs(local_addr.get_port()) );
        CnDelete dgram;
        return 0;
    }

    Net_Packet* pNetPacket = new Net_Packet;
    pNetPacket->id = 0;
    pNetPacket->m_cmd.id = id;
    pNetPacket->m_cmd.cmd = Net_Packet::NET_UDP_REGISTER;
    pNetPacket->m_cmd.mask = MASK_READ;
    pNetPacket->m_cmd.data = dgram;

    bool brc = m_send_packet_queue->write(pNetPacket);
    if (brc != true)	// 队列用尽
    {
        delete pNetPacket;
        CnWarn("Net_Manager::create_udp error, send task queue is full\n");
        return 0;
    }

    return id;
}

uint32_t Net_Manager::create_udp(const char *local_ip, int local_port, void* pUserData, int netbufsize)
{
    INET_Addr local_addr;
    local_addr.set_addr(inet_addr(local_ip));
    local_addr.set_port(htons(local_port));

    return create_udp(local_addr, pUserData, netbufsize);
}

uint32_t Net_Manager::create_tcp_client(const INET_Addr& remote_addr, SplitterFactory* splitter_factory, void* pUserData, int timeout, bool useBigQueue, int netbufsize)
{
    if (1 != m_status) 
    {
        CnWarn("Net_Manager::create_tcp_client error, not started\n");
        return 0;
    }

    int rc;

    SOCK_Connector *connector = CnNew SOCK_Connector(this, splitter_factory, pUserData, useBigQueue );
    uint32_t id = connector->get_id();

    rc = connector->create_tcp_client(netbufsize);
    if (0 != rc) 
    {
        CnWarn("Net_Manager::create_tcp_client error, create_tcp_client error\n");
        CnDelete connector;
        return 0;
    }

    rc = connector->connect_server(remote_addr, timeout);
    if (0 != rc) 
    {
        CnWarn("Net_Manager::create_tcp_client error, connect_server error\n");
        connector->close_tcp_client();
        CnDelete connector;
        return 0;
    }

    Net_Packet* pNetPacket = new Net_Packet;
    pNetPacket->id = 0;
    pNetPacket->m_cmd.id = id;
    pNetPacket->m_cmd.cmd = Net_Packet::NET_TCP_CONNECTOR_REGISTER;
    pNetPacket->m_cmd.mask = MASK_WRITE;
    pNetPacket->m_cmd.data = connector;
    pNetPacket->m_cmd.timeout = timeout;

    bool brc = m_send_packet_queue->write(pNetPacket);
    if (brc != true)	// 队列用尽
    {
        delete pNetPacket;
        CnWarn("Net_Manager::create_tcp_client error, send task queue is full\n");
        return 0;
    }

    return id;
}

uint32_t Net_Manager::create_tcp_client(const char *remote_ip, int remote_port, SplitterFactory *splitter_factory, 
                                        void* pUserData, int timeout, bool useBigQueue,  int netbufsize)
{
    INET_Addr remote_addr;
    remote_addr.set_addr(inet_addr(remote_ip));
    remote_addr.set_port(htons(remote_port));

    return create_tcp_client(remote_addr, splitter_factory, pUserData, timeout, useBigQueue, netbufsize);
}

uint32_t Net_Manager::create_tcp_server(const INET_Addr& local_addr, SplitterFactory *splitter_factory, 
                                        void* pUserData, bool useBigQueue, int netbufsize)
{
    if (1 != m_status) 
    {
        CnWarn("Net_Manager::create_tcp_server error, not started\n");
        return 0;
    }

    int rc;

    SOCK_Acceptor *acceptor = CnNew SOCK_Acceptor(this, splitter_factory, pUserData, useBigQueue );
    uint32_t id = acceptor->get_id();

    rc = acceptor->create_tcp_server(local_addr, netbufsize);
    if (0 != rc) 
    {
        CnWarn("Net_Manager::create_tcp_server %s:%u error, create_tcp_server error\n", Crown::GetIpStr(local_addr.get_addr()).c_str(), ntohs(local_addr.get_port()));
        CnDelete acceptor;
        return 0;
    }

    Net_Packet* pNetPacket = new Net_Packet;
    pNetPacket->id = 0;
    pNetPacket->m_cmd.id = id;
    pNetPacket->m_cmd.cmd = Net_Packet::NET_TCP_ACCEPTOR_REGISTER;
    pNetPacket->m_cmd.mask = MASK_READ;
    pNetPacket->m_cmd.data = acceptor;

    bool brc = m_send_packet_queue->write(pNetPacket);
    if (brc != true)	// 队列用尽
    {
        delete pNetPacket;
        CnWarn("Net_Manager::create_tcpserver error, send task queue is full\n");
        return 0;
    }

    return id;
}

uint32_t Net_Manager::create_tcp_server(const char *local_ip, int local_port, SplitterFactory *splitter_factory, 
                                        void* pUserData, bool useBigQueue, int netbufsize)
{
    INET_Addr local_addr;
    local_addr.set_addr(inet_addr(local_ip));
    local_addr.set_port(htons(local_port));

    return create_tcp_server(local_addr, splitter_factory, pUserData, useBigQueue, netbufsize);
}

int Net_Manager::delete_net(uint32_t id)
{
    Net_Packet* pNetPacket = new Net_Packet;
    pNetPacket->id = 0;
    pNetPacket->m_cmd.id = id;
    pNetPacket->m_cmd.cmd = Net_Packet::NET_DELETE;

    bool brc = m_send_packet_queue->write(pNetPacket);
    if (brc != true)	// 队列用尽
    {
        delete pNetPacket;
        CnWarn("Net_Manager::delete_net error, send task queue is full\n");
        return 0;
    }

    return 0;
}

int Net_Manager::send_packet(uint32_t id, Net_Packet* packet, const INET_Addr& remote_addr)
{
    if (1 != m_status) 
    {
        CnDbg("Net_Manager::send_packet error, not started\n");
        return -1;
    }

    if (0 == packet->getdatalen()) 
    {
        CnDbg("Net_Manager::send_packet error, length is 0\n");
        return -1;
    }

    Event_Handler* event_handler = m_reactor.get_event_handler(id);
    if (event_handler == NULL)
    {
        // 通道不存在
        //CnDbg("Net_Manager::send_packet error, net no exist\n");
        return -1;
    }

    packet->id = id;
    packet->remote_addr = remote_addr;

    bool rc = m_send_packet_queue->write(packet);
    if (rc != true)	// 队列用尽
    {
        CnDbg("Net_Manager::send_packet error, send task queue is full\n");
        return -2;
    }

    return 0;
}

int Net_Manager::send_packet(uint32_t id, Net_Packet* packet, const char *remote_ip, int remote_port)
{
    INET_Addr remote_addr;
    remote_addr.set_addr(inet_addr(remote_ip));
    remote_addr.set_port(htons(remote_port));

    return send_packet(id, packet, remote_addr);
}

int Net_Manager::send_packet(uint32_t id, Net_Packet* packet)
{
    INET_Addr remote_addr;
    return send_packet(id, packet, remote_addr);
}

Net_Event* Net_Manager::get_event()
{
    Net_Event *net_event = NULL;

    bool rc = m_net_event_queue->read(net_event);
    if (true != rc) {
        return NULL;
    }
    ++getCount;
    return net_event;
}

void Net_Manager::put_event(Net_Event* net_event)
{
    bool rc = m_net_event_queue->write(net_event);
    if (true != rc)
    {
        // 队列用尽
        CnError("Net_Manager::put_event error, net event queue is full, event_type:%d, netid:%d\n", net_event->net_event_type, net_event->id);
        delete net_event;
    }
    ++putCount;
}

int Net_Manager::get_send_size()
{
    return m_send_packet_queue->size();
}

int Net_Manager::get_recv_size()
{
    return m_net_event_queue->size();
}

Net_Packet* Net_Manager::get_send_packet()
{
    Net_Packet *send_packet = NULL;

    bool rc = m_send_packet_queue->read(send_packet);
    if (true != rc) {
        return NULL;
    }

    return send_packet;
}

void Net_Manager::on_net_error()
{
    m_status = 2;

    Net_Event *net_event = new Net_Event;

    net_event->net_event_type = TYPE_SYS_ERROR;
    net_event->id = 0;
    net_event->new_id = 0;

    put_event(net_event);
}

