//! @file net_manager.h


#ifndef _NET_MANAGER_H_
#define _NET_MANAGER_H_

#include "config.h"
#include "cycle_buffer.h"
#include "net_thread.h"
#include "id_manager.h"
#include "event_handler.h"
#include "inet_addr.h"
#include "net_packet.h"
#include "net_event.h"
#include "net_event_queue.h"
#include "splitter_factory.h"

#ifdef WIN32	// for windows
#include "select_reactor.h"
//! windows平台下使用select
typedef Select_Reactor Net_Reactor;
#define NETBUF_SIZE 8192
#else			// for linux
//! linux平台下使用epoll
#include "epoll_reactor.h"
typedef Epoll_Reactor Net_Reactor;
#define NETBUF_SIZE 16384
#endif

//! @typedef Net_Send_Task_Queue
//! @brief 网络组件的发送任务队列
//!
//! 此队列部分线程安全
//! 队列容量为100万
typedef Cycle_Buffer_T<Net_Packet*, MAX_NET_SEND_TASK_COUNT> Net_Send_Packet_Queue;


//! @typedef Socket_Send_Task_Queue
//! @brief 某个网络通道的发送任务队列
//!
//! 此队列部分线程安全
//! 队列容量为1万
typedef Cycle_Buffer_T<Net_Packet*, MAX_SOCKET_SEND_TASK_COUNT> Socket_Send_Packet_Queue;

// for test
typedef Cycle_Buffer_T<Net_Packet*, SMALL_QUEUE_PACKET_COUNT> Small_Packet_Queue;
typedef Cycle_Buffer_T<Net_Packet*, BIG_QUEUE_PACKET_COUNT> Big_Packet_Queue;

//! @class Net_Manager
//! @brief 网络管理器
class Net_Manager
{
public:
    Net_Manager();
    ~Net_Manager();

    static void CreateInstance()
    {
        m_reference_count++;    // 计数应该只在应用层调用，无锁
        if(m_instance) return;
        m_instance = CnNew Net_Manager;
        m_instance->start();
    }

    static Net_Manager* Instance() { return m_instance; }

    static void DestroyInstance()
    {
        m_reference_count--;    // 计数应该只在应用层调用，无锁
        if(m_reference_count) return;
        if(!m_instance) return;
        CnDelete m_instance;
        m_instance = 0;
    }

public:
    //! 启动网络管理器线程
    //! @return 0:成功, -1失败
    int start();

    //! 停止网络管理器线程
    //! @return 0:成功, -1失败
    int stop();

    //! 创建一个upd通道
    //! @param local_addr upd本地绑定地址
    //! @param pUserData 用户定义的数据，在netevent中返回给用户，一般会包含该连接的事件处理函数指针
    //! @param netbufsize 网络底层缓冲区大小
    //! @return 通道id, >0:成功, 0失败
    uint32_t create_udp(const INET_Addr& local_addr, void* pUserData, int netbufsize = NETBUF_SIZE);

    //! 创建一个upd通道
    //! @param local_ip upd本地绑定ip
    //! @param local_port upd本地绑定port
    //! @param pUserData 用户定义的数据，在netevent中返回给用户，一般会包含该连接的事件处理函数指针
    //! @param netbufsize 网络底层缓冲区大小
    //! @return 通道id, >0:成功, 0失败
    uint32_t create_udp(const char *local_ip, int local_port, void* pUserData, int netbufsize = NETBUF_SIZE);

    //! 创建一个tcp client通道
    //! @param remote_addr tcp对端地址
    //! @param packet_splitter tcp拆包器
    //! @param pUserData 用户定义的数据，在netevent中返回给用户，一般会包含该连接的事件处理函数指针
    //! @param timeout 连接超时时间(秒)
    //! @param netbufsize 网络底层缓冲区大小
    //! @return 通道id, >0:成功, 0失败
    uint32_t create_tcp_client(const INET_Addr& remote_addr, SplitterFactory *splitter_factory, 
        void* pUserData, int timeout, bool useBigQueue, int netbufsize = NETBUF_SIZE);

    //! 创建一个tcp client通道
    //! @param remote_ip 对端ip
    //! @param remote_port 对端port
    //! @param packet_splitter tcp拆包器
    //! @param pUserData 用户定义的数据，在netevent中返回给用户，一般会包含该连接的事件处理函数指针
    //! @param timeout 连接超时时间(秒)
    //! @param netbufsize 网络底层缓冲区大小
    //! @return 通道id, >0:成功, 0失败
    uint32_t create_tcp_client(const char *remote_ip, int remote_port, SplitterFactory *splitter_factory, 
        void* pUserData, int timeout, bool useBigQueue, int netbufsize = NETBUF_SIZE);

    //! 创建一个tcp server通道
    //! @param local_addr tcp本地监听地址
    //! @param packet_splitter tcp拆包器
    //! @param pUserData 用户定义的数据，在netevent中返回给用户，一般会包含该连接的事件处理函数指针
    //! @param netbufsize 网络底层缓冲区大小
    //! @return 通道id, >0:成功, 0失败
    uint32_t create_tcp_server(const INET_Addr& local_addr, SplitterFactory *splitter_factory, void* pUserData, bool useBigQueue, int netbufsize = NETBUF_SIZE);

    //! 创建一个tcp server通道
    //! @param local_ip tcp本地监听ip
    //! @param local_port tcp本地监听port
    //! @param packet_splitter tcp拆包器
    //! @param pUserData 用户定义的数据，在netevent中返回给用户，一般会包含该连接的事件处理函数指针
    //! @param netbufsize 网络底层缓冲区大小
    //! @return 通道id, >0:成功, 0失败
    uint32_t create_tcp_server(const char *local_ip, int local_port, SplitterFactory *splitter_factory, void* pUserData, bool useBigQueue, int netbufsize = NETBUF_SIZE);

    //! 通知删除网络通道
    //! @param id 通道id
    //! @return 0:成功 <0:失败
    int delete_net(uint32_t id);

    //! 发送数据包
    //! @warning 如果此调用成功, packet类由网络组件负责释放; 如果此调用失败, packet类由调用方负责释放
    //! @param id 通道id
    //! @param packet 数据包
    //! @param remote_addr 对端地址(udp需要使用此地址, tcp忽略此地址)
    //! @return 发送结果, 0:成功, -1:失败(通道不存在), -2:失败(缓冲区满)
    int send_packet(uint32_t id, Net_Packet* packet, const INET_Addr& remote_addr);

    //! 发送数据包
    //! @warning 如果此调用成功, packet类由网络组件负责释放; 如果此调用失败, packet类由调用方负责释放
    //! @param id 通道id
    //! @param packet 数据包
    //! @param remote_ip 对端ip
    //! @param remote_port 对端port
    //! @return 发送结果, 0:成功, -1:失败(通道不存在), -2:失败(缓冲区满)
    int send_packet(uint32_t id, Net_Packet* packet, const char *remote_ip, int remote_port);

    //! 发送数据包(只能用于tcp)
    //! @warning 如果此调用成功, packet类由网络组件负责释放; 如果此调用失败, packet类由调用方负责释放
    //! @param id 通道id
    //! @param packet 数据包
    //! @return 发送结果, 0:成功, -1:失败(通道不存在), -2:失败(缓冲区满)
    int send_packet(uint32_t id, Net_Packet* packet);

    //! 取网络事件
    //! @warning 客户端应循环调用此函数获取网络事件, 并在处理完成每个网络事件后将其释放
    //! @return 网络事件, 没有网络事件时返回NULL
    Net_Event* get_event();

    //! 获取未处理的发送事件的数量
    //! @return 发送事件的数量
    int get_send_size();

    //! 获取未处理的接收事件的数量
    //! @return 接收事件的数量
    int get_recv_size();

    static	int putCount;
    static	int getCount;
public:
    friend class Net_Thread;
    friend class SOCK_Dgram;
    friend class SOCK_Acceptor;
    friend class SOCK_Connector;
    friend class SOCK_Stream;

private:
    //! 放入一个网络事件
    //! @param net_event 网络事件
    void put_event(Net_Event* net_event);

    //! 取发送任务
    //! @return 发送任务, 没有发送任务时返回NULL
    Net_Packet* get_send_packet();

    //! 网络故障(reactor故障)
    void on_net_error();

private:
    //! 运行状态 0:停止, 1:运行, 2:异常
    int m_status;

    //! 网络管理器线程
    Net_Thread* m_net_thread;

    //! 通道id管理器
    Id_Manager m_id_manager;

    //! 反应器
    Net_Reactor m_reactor;

    //! 网络事件队列
    Net_Event_Queue *m_net_event_queue;

    //! 待发送任务队列
    Net_Send_Packet_Queue *m_send_packet_queue;

    static Net_Manager* m_instance;
    static uint32_t m_reference_count;
};


#endif // _NET_MANAGER_H_
