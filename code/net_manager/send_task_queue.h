//! @file send_task_queue.h


#ifndef _SEND_TASK_QUEUE_H_
#define _SEND_TASK_QUEUE_H_


#include "cycle_buffer.h"
#include "send_task.h"
#include "config.h"


//! @typedef Net_Send_Task_Queue
//! @brief 网络组件的发送任务队列
//!
//! 此队列部分线程安全
//! 队列容量为100万
typedef Cycle_Buffer_T<Send_Task*, MAX_NET_SEND_TASK_COUNT> Net_Send_Task_Queue;


//! @typedef Socket_Send_Task_Queue
//! @brief 某个网络通道的发送任务队列
//!
//! 此队列部分线程安全
//! 队列容量为1万
typedef Cycle_Buffer_T<Send_Task*, MAX_SOCKET_SEND_TASK_COUNT> Socket_Send_Task_Queue;


#endif // _SEND_TASK_QUEUE_H_
