#include "net_manager_pch.h"

#include "sock_dgram.h"
#include "net_manager.h"
#include "log.h"


// class SOCK_Dgram
SOCK_Dgram::SOCK_Dgram(Net_Manager *net_manager, void* pUserData)
{
    m_id = net_manager->m_id_manager.acquire(ID_UDP);
    assert(m_id != 0);

    m_socket = INVALID_SOCKET;
    m_net_manager = net_manager;
    m_current_send_packet = NULL;

    m_current_send_length = 0;

    m_UserData = pUserData;
}

SOCK_Dgram::~SOCK_Dgram()
{
    if (0 != m_id) {
        m_net_manager->m_id_manager.release(m_id);
    }

    //清除掉未发送完的数据包
    if (m_current_send_packet != NULL)
    {
        delete m_current_send_packet;
        m_current_send_packet = NULL;
    }

    Net_Packet* pNetPacket = NULL;
    while(m_send_packet_queue.read(pNetPacket))
    {
        delete pNetPacket;
    }

}

int SOCK_Dgram::create_udp(const INET_Addr& local_addr, int netbufsize)
{
    m_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (INVALID_SOCKET == m_socket) {
        LOG(ERROR)("SOCK_Dgram::create_udp error, socket error, errno:%d", error_no());
        return -1;
    }

    int nret = set_socket_nonblocking(m_socket);
    if (nret != 0)
    {
        LOG(INFO)("SOCK_Dgram::set_socket_nonblocking error, socket error, errno:%d", error_no());
        return -1;
    }

    //设置网络底层收发缓冲区长度
    nret = set_socket_bufsize(m_socket, true, netbufsize);
    int nsize = 0;
    nret = get_socket_bufsize(m_socket, true, nsize);
    if (nsize != netbufsize)
    {
        LOG(WARN)("setsockopt so_rcvbuf failed.to set size %d orgin size %d.", netbufsize, nsize);
    }

    nret = set_socket_bufsize(m_socket, false, netbufsize);
    nsize = 0;
    nret = get_socket_bufsize(m_socket, false, nsize);
    if (nsize != netbufsize)
    {
        LOG(WARN)("setsockopt so_sndbuf failed.to set size %d orgin size %d.", netbufsize, nsize);
    }

    sockaddr_in local;
    local.sin_family = AF_INET;
    local.sin_addr.s_addr = local_addr.get_addr();
    local.sin_port = local_addr.get_port();

    int rc = bind(m_socket, (sockaddr*)&local, sizeof(local));
    if (0 != rc) {
        LOG(ERROR)("SOCK_Dgram::create_udp error, bind error, errno:%d", error_no());
        close(m_socket);
        m_socket = INVALID_SOCKET;
        return -1;
    }

    m_local_addr = local_addr;
    return 0;
}

void SOCK_Dgram::close_udp()
{
    if (INVALID_SOCKET != m_socket) {
        close(m_socket);
        m_socket = INVALID_SOCKET;
    }
}

uint32_t SOCK_Dgram::get_id()
{
    return m_id;
}

SOCKET SOCK_Dgram::get_handle()
{
    return m_socket;
}

int SOCK_Dgram::handle_input()
{
    while (true)
    {
        char recv_buf[MAX_PACKET_LENGTH];
        sockaddr_in from;
        socklen_t fromlen = sizeof(from);

        int rc = recvfrom(m_socket, recv_buf, MAX_PACKET_LENGTH, 0, (sockaddr*)&from, &fromlen);
        if (rc > 0)
        {
            Net_Event *net_event = new Net_Event;

            net_event->net_event_type = TYPE_DATA;
            net_event->id = m_id;
            net_event->new_id = 0;
            net_event->local_addr = m_local_addr;
            net_event->UserData = m_UserData;
            net_event->remote_addr.set_addr(from.sin_addr.s_addr);
            net_event->remote_addr.set_port(from.sin_port);

            net_event->packet.write_data(recv_buf, rc);
            net_event->packet.movetobegin();
            m_net_manager->put_event(net_event);
        }
        else if (rc == 0)
        {
            // close
            LOG(WARN)("SOCK_Dgram::handle_input, socket close by peer");
            return 0;
        }
        else
        {
            if (NETMANAGER_EAGAIN == error_no()) {
                return 0;
            }
            else {
                // exception
                LOG(WARN)("SOCK_Dgram::handle_input error, recvfrom error, errno:%d", error_no());
                return 0;
            }
        }
    }

    return 0;
}

int SOCK_Dgram::handle_output()
{
    // 如果当前正在发送的任务非空则先发送
    if (m_current_send_packet != NULL)
    {
        int rc = realSend();
        if (rc != 0)
        {
            return 0;
        }

    }

    assert(m_current_send_packet == NULL);

    while (true)
    {
        bool brc = m_send_packet_queue.read(m_current_send_packet);
        if (false == brc) {
            // 发送队列已空
            break;
        }

        int rc = realSend();
        if (rc != 0)
        {
            return 0;
        }

    }

    assert(m_current_send_packet == NULL);

    int rc = reactor()->remove_handler(this, MASK_WRITE);
    if (0 != rc) {
        // 设置reactor失败,认为socket异常
        LOG(ERROR)("SOCK_Dgram::handle_output error, remove_handler error");
        return -2;
    }

    return 0;
}

int SOCK_Dgram::handle_exception()
{
    if (INVALID_SOCKET != m_socket) {
        close(m_socket);
        m_socket = INVALID_SOCKET;
    }

    Net_Event *net_event = new Net_Event;

    net_event->net_event_type = TYPE_EXCEPTION;
    net_event->id = m_id;
    net_event->new_id = 0;
    net_event->local_addr = m_local_addr;
    net_event->UserData = m_UserData;

    m_net_manager->put_event(net_event);

    return 0;
}

int SOCK_Dgram::handle_close()
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

    net_event->net_event_type = TYPE_CLOSE;
    net_event->id = m_id;
    net_event->new_id = 0;
    net_event->local_addr = m_local_addr;
    net_event->UserData = m_UserData;

    m_net_manager->put_event(net_event);

    return 0;
}

int SOCK_Dgram::handle_timeout()
{
    return -1;
}

int SOCK_Dgram::post_packet(Net_Packet *send_packet)
{
    // 当前没有待发任务, 直接发送
    if (NULL == m_current_send_packet)
    {
        // assert send_task_queue is empty
        assert(false == m_send_packet_queue.read(m_current_send_packet));
        int rc = realSend();
        if (rc != NETMANAGER_EAGAIN)
        {

        }
    }

    // 网络忙, 入队列待发
    if (NULL != m_current_send_packet)
    {
        bool brc = m_send_packet_queue.write(send_packet);
        if (true != brc)
        {
            // 队列用尽
            LOG(ERROR)("SOCK_Dgram::send_data error, send task queue is full, packet lost");
            delete send_packet;
            return 0;
        }
    }

    assert(NULL != m_current_send_packet);

    // 监控可写事件
    int rc = reactor()->register_handler(this, MASK_WRITE);
    if (0 != rc) {
        // 设置reactor失败, 认为socket异常
        LOG(ERROR)("SOCK_Dgram::send_data error, register_handler error");
        reactor()->notify_close(m_id);
        return 0;
    }
    return 0;
}

int SOCK_Dgram::realSend()
{
    sockaddr_in to;
    to.sin_family = AF_INET;
    to.sin_addr.s_addr = m_current_send_packet->remote_addr.get_addr();
    to.sin_port = m_current_send_packet->remote_addr.get_port();
    socklen_t tolen = sizeof(to);

    Net_Packet* pNetPacket = m_current_send_packet;
    int nDataLen = pNetPacket->getdatalen() - m_current_send_length;
    int rc = 0;

    char* pBuf;
    int curdatalen = 0;
    while(nDataLen > 0)
    {
        pBuf = pNetPacket->get_rptr(curdatalen);
        rc = sendto(m_socket, pBuf, curdatalen, 0, (sockaddr*)&to, tolen);
        if (rc > 0) {
            nDataLen -= rc;
            m_current_send_length += rc;
            pNetPacket->move_rptr(rc);
        }
        else {
            if (NETMANAGER_EAGAIN == error_no()) {
                return NETMANAGER_EAGAIN;
            }
            else {
                // exception
                int nErrCode = error_no();
                LOG(WARN)("SOCK_Dgram::handle_output error, sendto error, errno:%d", nErrCode);
                return nErrCode;
            }
        }
    }

    delete m_current_send_packet;
    m_current_send_packet = NULL;
    m_current_send_length = 0;

    return 0;
}


