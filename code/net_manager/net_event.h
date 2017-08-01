//! @file net_event.h


#ifndef _NET_EVENT_H_
#define _NET_EVENT_H_


#include "inet_addr.h"
#include "net_packet.h"
#include "pool_allocator.h"
#include "config.h"

class Net_Event;
//! 网络事件处理器
typedef void (*pfnNetEventHandler)(Net_Event& net_event);

//! @enum NET_EVENT_TYPE
//! @brief 网络事件类型
enum NET_EVENT_TYPE
{
    TYPE_NULL,              //!< 空事件
    TYPE_DATA,              //!< 数据到达
    TYPE_CONNECT,           //!< tcp连接成功
    TYPE_ACCEPT,            //!< tcp接受连接
    TYPE_CLOSE,             //!< tcp连接关闭
    TYPE_LISTEN_CLOSE,      //!< 侦听socket关闭
    TYPE_EXCEPTION,         //!< 异常
    TYPE_LISTEN_EXCEPTION,  //!< 侦听socket异常
    TYPE_TIMEOUT,           //!< 超时
    TYPE_SYS_ERROR          //!< 系统异常
};


//! @class Net_Event
//! @brief 网络事件类
//!
//! 此类在网络层被分配并初使化, 在应用层被使用并销毁, 为提高效率使用预分配对象池
class Net_Event
{
public:
    Net_Event();
    ~Net_Event();

public:
    DECLARE_ALLOCATOR(Net_Event, NET_EVENT_POOL_COUNT)
public:
    //! 网络事件类型
    NET_EVENT_TYPE net_event_type;

    //! 发生事件的通道id
    uint32_t id;

    //! 产生新通道id, 如果没有产生新通道此处为0
    uint32_t new_id;

    //! 本地地址
    INET_Addr local_addr;

    //! 对端地址
    INET_Addr remote_addr;

    //! 数据包
    Net_Packet packet;

    //! 该事件处理函数指针
    void* UserData;

};

#endif // _NET_EVENT_H_
