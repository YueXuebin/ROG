//! @file net_event_queue.h


#ifndef _NET_EVENT_QUEUE_H_
#define _NET_EVENT_QUEUE_H_


#include "cycle_buffer.h"
#include "net_event.h"
#include "config.h"


//! @typedef Net_Event_Queue
//! @brief 网络事件队列
//!
//! 通信层产生事件并放入此队列, 应用层取出事件并释放
//! 此队列部分线程安全
typedef Cycle_Buffer_T<Net_Event*, MAX_NET_EVENT_COUNT> Net_Event_Queue;


#endif // _NET_EVENT_QUEUE_H_
