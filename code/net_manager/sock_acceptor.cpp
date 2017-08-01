#include "net_manager_pch.h"
#include "sock_acceptor.h"
#include "sock_stream.h"
#include "net_manager.h"
#include "splitter_factory.h"
#include "log.h"



// class SOCK_Acceptor
SOCK_Acceptor::SOCK_Acceptor(Net_Manager *net_manager, SplitterFactory *splitter_factory, void* pUserData, bool useBigQueue)
{
    m_id = net_manager->m_id_manager.acquire(ID_TCP_ACCEPTOR);
    assert(m_id != 0);

    m_socket = INVALID_SOCKET;
    m_net_manager = net_manager;
    m_splitter_factory = splitter_factory;
    m_packet_splitter = m_splitter_factory->CreateSplitter();
    m_UserData = pUserData;
    m_use_big_queue = useBigQueue;
}

SOCK_Acceptor::~SOCK_Acceptor()
{
    m_splitter_factory->DestorySplitter(m_packet_splitter);
    m_packet_splitter = NULL;

    if (0 != m_id) 
    {
        m_net_manager->m_id_manager.release(m_id);
    }
}

int SOCK_Acceptor::create_tcp_server(const INET_Addr& local_addr, int netbufsize)
{
    m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (INVALID_SOCKET == m_socket) 
    {
        CnError("SOCK_Acceptor::create_tcp_server %s:%u error, socket error, errno:%d\n", Crown::GetIpStr(local_addr.get_addr()).c_str(), local_addr.get_port(), error_no());
        return -1;
    }

    int nret = set_socket_nonblocking(m_socket);
    if (nret != 0)
    {
        CnError("SOCK_Acceptor::set_socket_nonblocking %s:%u error, socket error, errno:%d\n", Crown::GetIpStr(local_addr.get_addr()).c_str(), local_addr.get_port(), error_no());
        return -1;
    }

    //设置网络底层收发缓冲区长度
    if(false)
    {
        nret = set_socket_bufsize(m_socket, true, netbufsize);
        int nsize = 0;
        nret = get_socket_bufsize(m_socket, true, nsize);

        if (nsize != netbufsize)
        {
            CnWarn("setsockopt so_rcvbuf failed to set size %d orgin size %d\n", netbufsize, nsize);
        }

        nret = set_socket_bufsize(m_socket, false, netbufsize);
        nsize = 0;
        nret = get_socket_bufsize(m_socket, false, nsize);
        if (nsize != netbufsize)
        {
            CnDbg("setsockopt so_sndbuf failed nto set size %d orgin size %d\n", netbufsize, nsize);
        }
    }

    int val = 1;
    setsockopt(m_socket, SOL_SOCKET, SO_REUSEADDR, (char*)&val, sizeof(val));

    sockaddr_in local;
    local.sin_family = AF_INET;
    local.sin_addr.s_addr = local_addr.get_addr();
    local.sin_port = local_addr.get_port();

    int rc = bind(m_socket, (sockaddr*)&local, sizeof(local));
    if (0 != rc) 
    {
        CnError("SOCK_Acceptor::create_tcp_server %s:%u error, bind error, errno:%d\n", Crown::GetIpStr(local_addr.get_addr()).c_str(), ntohs(local_addr.get_port()), error_no());
        close(m_socket);
        m_socket = INVALID_SOCKET;
        return -1;
    }

    rc = listen(m_socket, 10240);
    if (0 != rc) 
    {
        CnError("SOCK_Acceptor::create_tcp_server %s:%u error, listen error, errno:%d\n", Crown::GetIpStr(local_addr.get_addr()).c_str(), local_addr.get_port(), error_no());
        close(m_socket);
        m_socket = INVALID_SOCKET;
        return -1;
    }

    m_local_addr = local_addr;
    return 0;
}

void SOCK_Acceptor::close_tcp_server()
{
    if (INVALID_SOCKET != m_socket)
    {
        close(m_socket);
        m_socket = INVALID_SOCKET;
    }
}

uint32_t SOCK_Acceptor::get_id()
{
    return m_id;
}

SOCKET SOCK_Acceptor::get_handle()
{
    return m_socket;
}

int SOCK_Acceptor::handle_input()
{
    while (true)
    {
        sockaddr_in addr;
        socklen_t addrlen = sizeof(addr);

        SOCKET accept_socket = accept(m_socket, (sockaddr*)&addr, &addrlen);
        if (-1 == accept_socket)
        {
            if (NETMANAGER_EAGAIN == error_no() || NETMANAGER_EWOULDBLOCK == error_no()) 
            {
                return 0;
            }
            else 
            {
                // exception
                CnWarn("SOCK_Acceptor::handle_input error, accept error, errno:%d\n", error_no());
                return -2;
            }
        }

        // got a new connection
        int rc;

        set_socket_nonblocking(accept_socket);
        if(USE_TCP_NODELAY)
            set_socket_tcp_nodelay(accept_socket);      // 设置tcp_nodelay

        uint32_t new_id = m_net_manager->m_id_manager.acquire(ID_TCP_STREAM);
        assert(new_id != 0);

        INET_Addr remote_addr;
        remote_addr.set_addr(addr.sin_addr.s_addr);
        remote_addr.set_port(addr.sin_port);


        SOCK_Stream *stream = new SOCK_Stream(new_id, m_net_manager, m_splitter_factory, accept_socket, m_local_addr, remote_addr, m_UserData, m_use_big_queue );

        //注册读写事件
        rc = reactor()->register_handler(stream, MASK_READ | MASK_WRITE);
        if (0 != rc) 
        {
            CnError("SOCK_Acceptor::handle_input error, register_handler MASK_READ error\n");
            stream->close_stream();
            delete stream;
            return 0;
        }

        // throw a net event
        Net_Event *net_event = new Net_Event;

        net_event->net_event_type = TYPE_ACCEPT;
        net_event->id = m_id;
        net_event->new_id = new_id;
        net_event->local_addr = m_local_addr;
        net_event->remote_addr = remote_addr;
        net_event->packet.remote_addr = remote_addr;
        net_event->UserData = m_UserData;

        m_net_manager->put_event(net_event);
    }

    return 0;
}

int SOCK_Acceptor::handle_output()
{
    //assert(true == false);
    return -1;
}

int SOCK_Acceptor::handle_exception()
{
    if (INVALID_SOCKET != m_socket) {
        close(m_socket);
        m_socket = INVALID_SOCKET;
    }

    Net_Event *net_event = new Net_Event;

    net_event->net_event_type = TYPE_LISTEN_EXCEPTION;
    net_event->id = m_id;
    net_event->new_id = 0;
    net_event->local_addr = m_local_addr;
    net_event->UserData = m_UserData;

    m_net_manager->put_event(net_event);

    return 0;
}

int SOCK_Acceptor::handle_close()
{
    if (INVALID_SOCKET != m_socket) {
        close(m_socket);
        m_socket = INVALID_SOCKET;
    }

    if (notify_close)
    {
        return 0;
    }

    Net_Event *net_event = new Net_Event;

    net_event->net_event_type = TYPE_LISTEN_CLOSE;
    net_event->id = m_id;
    net_event->new_id = 0;
    net_event->local_addr = m_local_addr;
    net_event->UserData = m_UserData;

    m_net_manager->put_event(net_event);

    return 0;
}

int SOCK_Acceptor::handle_timeout()
{
    return -1;
}

int SOCK_Acceptor::post_packet(Net_Packet *send_packet)
{
    CnAssert(false);
    CnError("SOCK_Acceptor::send_data error, can't send at this socket");
    delete send_packet;
    return 0;
}

