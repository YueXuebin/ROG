#include "net_manager_pch.h"

#include "sock_stream.h"
#include "net_manager.h"
#include "log.h"
#include "splitter_factory.h"

// class SOCK_Stream
SOCK_Stream::SOCK_Stream(uint32_t id, Net_Manager *net_manager, SplitterFactory* splitter_factory, SOCKET socket, const INET_Addr& local_addr, 
                         const INET_Addr& remote_addr, void* pUserData, bool useBigQueue )
{
    m_id = id;
    m_socket = socket;
    m_local_addr = local_addr;
    m_remote_addr = remote_addr;
    m_net_manager = net_manager;
    m_splitter_factory = splitter_factory;
    m_packet_splitter = splitter_factory->CreateSplitter();
    m_current_send_packet = NULL;
    m_current_send_length = 0;
    m_UserData = pUserData;
    m_PartDataEvent = NULL;
    m_PartDataLeftLen = 0;

    if( useBigQueue )
    {
        m_big_packet_queue = CnNew Big_Packet_Queue;	
    }
    m_use_big_queue = useBigQueue;

}

SOCK_Stream::~SOCK_Stream()
{
    m_splitter_factory->DestorySplitter(m_packet_splitter);
    m_packet_splitter = NULL;
        
    if (0 != m_id)
    {
        m_net_manager->m_id_manager.release(m_id);
    }

    //清除未发完的数据包
    if (m_current_send_packet != NULL)
    {
        delete m_current_send_packet;
        m_current_send_packet = NULL;
    }

    // 清除队列中的包
    clear_send_queue();
    if( m_use_big_queue )
    {
        delete (Big_Packet_Queue*)m_big_packet_queue;
    }	
}

void SOCK_Stream::close_stream()
{
    if (INVALID_SOCKET != m_socket)
    {
        close(m_socket);
        m_socket = INVALID_SOCKET;
    }
}

uint32_t SOCK_Stream::get_id()
{
    return m_id;
}

SOCKET SOCK_Stream::get_handle()
{
    return m_socket;
}

int SOCK_Stream::handle_input()
{
    while (true)
    {
        int rc = recv(m_socket, m_recv_buffer.wr_ptr(), m_recv_buffer.space_length(), 0);
        if (rc > 0)
        {
            m_recv_buffer.wr_ptr(rc);

            if (m_PartDataEvent != NULL)
            {
                if (m_recv_buffer.data_length() < m_PartDataLeftLen)
                {
                    m_PartDataEvent->packet.write_data(m_recv_buffer.rd_ptr(), m_recv_buffer.data_length());
                    m_PartDataLeftLen -= m_recv_buffer.data_length();
                    m_recv_buffer.rd_ptr(m_recv_buffer.data_length());
                    m_recv_buffer.recycle();
                    continue;
                }

                m_PartDataEvent->packet.write_data(m_recv_buffer.rd_ptr(), m_PartDataLeftLen);
                m_PartDataEvent->packet.movetobegin();

                m_net_manager->put_event(m_PartDataEvent);

                m_PartDataEvent = NULL;
                m_recv_buffer.rd_ptr(m_PartDataLeftLen);
                m_PartDataLeftLen = 0;
            }

            int packet_begin = 0;
            int packet_len = 0;

            // 循环拆包
            while (true)
            {
                packet_len = 0;
                int split_result = m_packet_splitter->split(m_recv_buffer.rd_ptr(), m_recv_buffer.data_length(), packet_begin, packet_len);
                if (1 == split_result)
                {
                    // 获取到一个数据包 got a packet
                    Net_Event *net_event = new Net_Event;

                    net_event->net_event_type = TYPE_DATA;
                    net_event->id = m_id;
                    net_event->new_id = 0;
                    net_event->local_addr = m_local_addr;
                    net_event->remote_addr = m_remote_addr;
                    net_event->UserData = m_UserData;
                    net_event->packet.write_data(m_recv_buffer.rd_ptr() + packet_begin, packet_len);
                    net_event->packet.movetobegin();
                    m_net_manager->put_event(net_event);

                    m_recv_buffer.rd_ptr(packet_begin + packet_len);
                }
                else if (0 == split_result)
                {
                    // no packet in buf
                    if(packet_len > m_recv_buffer.length())
                    {
                        //重新生成一个较大的缓冲区,并将当前缓冲区数据转移过去
                        //m_recv_buffer.extend(packet_len);
                        m_PartDataEvent = new Net_Event;
                        m_PartDataEvent->net_event_type = TYPE_DATA;
                        m_PartDataEvent->id = m_id;
                        m_PartDataEvent->new_id = 0;
                        m_PartDataEvent->local_addr = m_local_addr;
                        m_PartDataEvent->remote_addr = m_remote_addr;
                        m_PartDataEvent->UserData = m_UserData;
                        m_PartDataLeftLen = packet_len - m_recv_buffer.data_length();
                        m_PartDataEvent->packet.write_data(m_recv_buffer.rd_ptr() + packet_begin, m_recv_buffer.data_length());
                        m_recv_buffer.rd_ptr(m_recv_buffer.data_length());
                    }

                    break;
                }
                else
                {
                    // 协议错
                    CnWarn("SOCK_Stream::handle_input error, split error, split_result:%d\n", split_result);
                    return -2;  // 异常
                }

                if (0 == m_recv_buffer.data_length())
                {
                    // buf is empty, no packet in buf
                    break;
                }
            }

            if ((m_recv_buffer.data_length() == 0) || (m_recv_buffer.space_length() == 0)
                || (m_recv_buffer.data_length()+m_recv_buffer.space_length())<packet_len)
            {
                m_recv_buffer.recycle();
            }

        }
        else if (rc == 0)
        {
            // close
            CnDbg("SOCK_Stream::handle_input, socket close by peer\n");    // 对方断开
            return -1;      // 关闭
        }
        else
        {
            if (NETMANAGER_EAGAIN == error_no())
            {
                return 0;       // 继续
            }
            else 
            {
                // exception
                CnDbg("SOCK_Stream::handle_input error, recv error, errno:%d\n", error_no());
                return -2;      // 异常
            }
        }
    }

    return 0;
}

int SOCK_Stream::handle_output()
{
#ifdef WIN32
    int size = get_queue_size();
    if (m_current_send_packet == NULL && /*m_send_packet_queue.size()*/ size == 0)
    {
        m_net_manager->m_reactor.remove_handler(this, MASK_WRITE);
    }
#endif

    if (m_current_send_packet != NULL)
    {
        int rc = realSend();
        if (rc != 0)
        {
            return rc;
        }
    }

    assert(m_current_send_packet == NULL);
    assert(m_current_send_length == 0);

    while (true)
    {
        // bool brc = false /*m_send_packet_queue.read(m_current_send_packet);*/;
        bool brc = read_queue();
        if (false == brc)
        {   // 发送队列已空
            break;
        }
        int rc = realSend();
        if (rc != 0)
        {
            return rc;
        }
    }

    assert(m_current_send_packet == NULL);
    assert(m_current_send_length == 0);

    return 0;
}

int SOCK_Stream::handle_exception()
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
    net_event->remote_addr = m_remote_addr;
    net_event->UserData = m_UserData;

    m_net_manager->put_event(net_event);

    return 0;
}

int SOCK_Stream::handle_close()
{
    if (INVALID_SOCKET != m_socket) 
    {
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
    net_event->remote_addr = m_remote_addr;
    net_event->UserData = m_UserData;

    m_net_manager->put_event(net_event);

    return 0;
}

int SOCK_Stream::handle_timeout()
{
    if (INVALID_SOCKET != m_socket) {
        close(m_socket);
        m_socket = INVALID_SOCKET;
    }

    Net_Event *net_event = new Net_Event;

    net_event->net_event_type = TYPE_TIMEOUT;
    net_event->id = m_id;
    net_event->new_id = 0;
    net_event->local_addr = m_local_addr;
    net_event->remote_addr = m_remote_addr;
    net_event->UserData = m_UserData;

    m_net_manager->put_event(net_event);

    return 0;
}

int SOCK_Stream::post_packet(Net_Packet* send_packet)
{
    if (m_current_send_packet != NULL)
    {
        bool brc = write_send_queue( send_packet );

        if (true != brc)
        {
            // 队列用尽
            CnDbg("SOCK_Stream::send_data error, send task queue is full, packet lost, net_id%d, ip:%s, port:%u\n",
                m_id, Crown::GetIpStr(m_remote_addr.get_addr()).c_str(), ntohl(m_remote_addr.get_port()));
            // 连接关闭
            reactor()->delete_handler(this);
            handle_close();
            return -5;
        }
    }
    else
    {
        m_current_send_packet = send_packet;
        int rc = realSend();
        if (-1 == rc)
        {
            // 连接关闭
            reactor()->delete_handler(this);
            handle_close();
            return -1;
        }
        else if (-2 == rc)
        {
            // 连接异常
            reactor()->delete_handler(this);
            handle_exception();
            return -2;
        }
        else if(-3 == rc)
        {	

        }
    }
    return 0;
}

int SOCK_Stream::realSend()
{
    Net_Packet* pNetPacket = m_current_send_packet;
    int nDataLen = pNetPacket->getdatalen() - m_current_send_length;
    int rc = 0;

    char* pBuf;
    int curdatalen = 0;
    while(nDataLen > 0)
    {
        pBuf = pNetPacket->get_rptr(curdatalen);
        rc = send(m_socket, pBuf, curdatalen, 0);
        if (rc > 0) 
        {
            // 发送成功
            m_current_send_length += rc;
            pNetPacket->move_rptr(rc);
            nDataLen -= rc;
        }
        else 
        {
            if (NETMANAGER_EAGAIN == error_no())
            {
#ifdef WIN32
                m_net_manager->m_reactor.register_handler(this, MASK_WRITE);
#endif
                // 该socket上次操作未完成
                //CnWarn("SOCK_Stream::handle_output error, net_id:%d, ip:%s, port:%u\n", m_id, Crown::GetIpStr(m_remote_addr.get_addr()).c_str(), m_remote_addr.get_port());
                return -3;	// 数据未写完，需要等待后,再次写入
            }
            else 
            {
                CnDbg("SOCK_Stream::handle_output error, send error, errno:%d\n", error_no());
                return -2;      // 异常
            }
        }
    }

    assert(m_current_send_length == pNetPacket->getdatalen());
    delete m_current_send_packet;
    m_current_send_packet = NULL;
    m_current_send_length = 0;
    return 0;
}

bool SOCK_Stream::read_queue()
{
    bool res = true;
    if( m_use_big_queue )
    {
        res = ((Big_Packet_Queue*)m_big_packet_queue)->read(m_current_send_packet);
    }
    else
    {
        res = m_small_packet_queue.read(m_current_send_packet);
    }
    return res;
}

bool SOCK_Stream::write_send_queue( Net_Packet *packet )
{
    bool res = true;
    if( m_use_big_queue )
    {
        res = ((Big_Packet_Queue*)m_big_packet_queue)->write(packet);
    }
    else
    {
        res = m_small_packet_queue.write(packet);
    }
    return res;
}

void SOCK_Stream::clear_send_queue()
{
    if( m_use_big_queue )
    {
        while(((Big_Packet_Queue*)m_big_packet_queue)->read(m_current_send_packet))
        {
            delete m_current_send_packet;
        }
    }
    else
    {
        while( m_small_packet_queue.read(m_current_send_packet))
        {
            delete m_current_send_packet;
        }
    }
}

int SOCK_Stream::get_queue_size()
{
    int size = 0;
    if( m_use_big_queue )
    {
        size = ((Big_Packet_Queue*)m_big_packet_queue)->size();
    }
    else
    {
        size = m_small_packet_queue.size();
    }
    return size;
}

IMPLEMENT_ALLOCATOR(SOCK_Stream)

