//! @file event_handler.h


#ifndef _EVENT_HANDLER_H_
#define _EVENT_HANDLER_H_


#include "os.h"
#include "List.h"

//! @enum EVENT_MASK
//! @brief 事件类型
enum EVENT_MASK
{
    MASK_READ = (1 << 0),		//!< 可读事件
    MASK_WRITE = (1 << 1),		//!< 可写事件
    MASK_TIMEOUT = (1 << 2),	//!< 超时事件
};

// 四种事件处理器类型
enum
{
    ACCEPT_HANDLER = 1,
    CONNECTOR_HANDLER,
    DGRAM_HANDLER,
    STREAM_HANDLER,
};


class Reactor;
class Net_Packet;


//! @class Event_Handler
//! @brief 事件处理器
class Event_Handler
{
public:
    Event_Handler();
    virtual ~Event_Handler();

public:
    //! 获取反应器
    //! @return 反应器指针
    Reactor* reactor();

    //! 设置反应器
    //! @param reactor 反应器指针
    void reactor(Reactor* reactor);

    // 类型
    virtual uint32 get_type() = 0;

    //! 获取通道id
    //! @return 通道id
    virtual uint32_t get_id() = 0;

    //! 获取socket句柄
    //! @return socket句柄
    virtual SOCKET get_handle() = 0;

    //! 处理读
    //! @return 处理结果 0:处理正常, -1: 连接被关闭, -2:连接异常
    virtual int handle_input() = 0;

    //! 处理写
    //! @return 处理结果 0:处理正常, -1: 连接被关闭, -2:连接异常
    virtual int handle_output() = 0;

    //! 连接异常
    virtual int handle_exception() = 0;

    //! 连接关闭
    virtual int handle_close() = 0;

    //! 超时
    virtual int handle_timeout() = 0;

    //! 提交发送任务
    //! @param send_task 待发送的任务
    virtual int post_packet(Net_Packet *send_packet) = 0;

public:
    list_head hashitem;

    bool	read;
    bool	write;
    bool	notify_close;
    time_t	timeout;		//<! 超时的时间, 0表示没有设置超时
private:
    //! 反应器
    Reactor *m_reactor;
};


#endif // _EVENT_HANDLER_H_
