//! @file sock_connector.h


#ifndef _SOCK_CONNECTOR_H_
#define _SOCK_CONNECTOR_H_

#include "net_event.h"
#include "event_handler.h"
#include "inet_addr.h"

class Net_Manager;
class Packet_Splitter;
class SplitterFactory;


//! @class SOCK_Connector
//! @brief tcp连接处理类
class SOCK_Connector : public Event_Handler
{
public:
    //! 构造函数
    //! @param net_manager 网络管理器
    //! @param packet_splitter 拆包器
    SOCK_Connector(Net_Manager *net_manager, SplitterFactory *splitter_factory, void* pUserData, bool useBigQueue );

    //! 析构函数
    virtual ~SOCK_Connector();

public:
    //! 创建tcp连接器
    //! @return 结果, 0:成功, -1失败
    int create_tcp_client(uint16_t netbufsize);

    //! 连接服务器(异步连接)
    //! @param remote_addr 服务端地址
    //! @param timeout 连接超时
    //! @return 结果, 0:成功, -1失败
    int connect_server(const INET_Addr& remote_addr, int timeout);

    //! 关闭通道
    void close_tcp_client();

    virtual uint32 get_type() {return CONNECTOR_HANDLER;}

    //! 获取通道id
    //! @return 通道id
    virtual uint32_t get_id();

    //! 获取socket句柄
    //! @return socket句柄
    virtual SOCKET get_handle();

    //! 处理读
    //! @return 处理结果 0:处理正常, -1: 连接被关闭, -2:连接异常
    virtual int handle_input();

    //! 处理写
    //! @return 处理结果 0:处理正常, -1: 连接被关闭, -2:连接异常
    virtual int handle_output();

    //! 连接异常
    virtual int handle_exception();

    //! 连接关闭
    virtual int handle_close();

    //! 超时
    virtual int handle_timeout();

    //! 提交发送任务
    //! @param send_task 待发送的任务
    virtual int post_packet(Net_Packet *send_packet);

private:
    //! 通道id
    uint32_t m_id;

    //! socket句柄
    SOCKET m_socket;

    //! 服务端地址
    INET_Addr m_remote_addr;

    //! 网络管理器
    Net_Manager *m_net_manager;

    //! 拆包器
    Packet_Splitter*    m_packet_splitter;
    SplitterFactory*    m_splitter_factory;

    //! 用户自定义数据
    void* m_UserData;

    bool	m_use_big_queue;
};


#endif // _SOCK_CONNECTOR_H_
