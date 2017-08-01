//! @file config.h
//! @brief 系统各项参数定义


#ifndef _CONFIG_H_
#define _CONFIG_H_


//! @def MAX_PACKET_LENGTH
//! @brief 网络包最大长度
//!
//! 目前系统使用的包大小都不超过2048, 此处包数据缓存大小设为2048,
//! 开发新协议时注意包长度不要超过此处限制
#ifndef MAX_PACKET_LENGTH
#define MAX_PACKET_LENGTH 2048
#endif


//! @def MAX_NET_EVENT_COUNT
//! @brief 最大网络事件个数
//!
//! 实际未处理网络事件超过MAX_NET_EVENT_COUNT时会出现丢失网络事件的情况
#ifndef MAX_NET_EVENT_COUNT
#define MAX_NET_EVENT_COUNT 1024000
#endif


//! @def MAX_NET_SEND_TASK_COUNT
//! @brief 最大全局待发送任务个数
//!
//! 实际待发送任务个数超过时会出现丢失发送包的情况
#ifndef MAX_NET_SEND_TASK_COUNT
#define MAX_NET_SEND_TASK_COUNT 1024000
#endif


//! @def MAX_SOCKET_SEND_TASK_COUNT
//! @brief 最大单个socket待发送任务个数
//!
//! 实际待发送任务个数超过时会出现丢失发送包的情况
#define MAX_SOCKET_SEND_TASK_COUNT 10240


//! @def RECV_BUFFER_LENGTH
//! @brief 接受缓存长度
#define RECV_BUFFER_LENGTH 20480


//! @def NET_EVENT_POOL_COUNT
//! @brief Net_Event对象池中缓存的对象个数
#define NET_EVENT_POOL_COUNT 20000


//! @def NET_PACKET_POOL_COUNT
//! @brief Net_Packet对象池中缓存的对象个数
#define NET_PACKET_POOL_COUNT 20000


//! @def SEND_TASK_POOL_COUNT
//! @brief Send_Task对象池中缓存的对象个数
#define SEND_TASK_POOL_COUNT 20000


//! @def SOCK_STREAM_POOL_COUNT
//! @brief SOCK_Stream对象池中缓存的对象个数
#define SOCK_STREAM_POOL_COUNT 1000


//! @def SMALL_QUEUE_PACKET_COUNT
//! @bried SOCK_Stream 使用小循环队列的包个数
#define SMALL_QUEUE_PACKET_COUNT 10240

//! @def SMALL_QUEUE_PACKET_COUNT
//! @bried SOCK_Stream 使用大循环队列的包个数
#define BIG_QUEUE_PACKET_COUNT  5000000


const bool USE_TCP_NODELAY = true;

#endif // _CONFIG_H_
