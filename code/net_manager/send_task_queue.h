//! @file send_task_queue.h


#ifndef _SEND_TASK_QUEUE_H_
#define _SEND_TASK_QUEUE_H_


#include "cycle_buffer.h"
#include "send_task.h"
#include "config.h"


//! @typedef Net_Send_Task_Queue
//! @brief ��������ķ����������
//!
//! �˶��в����̰߳�ȫ
//! ��������Ϊ100��
typedef Cycle_Buffer_T<Send_Task*, MAX_NET_SEND_TASK_COUNT> Net_Send_Task_Queue;


//! @typedef Socket_Send_Task_Queue
//! @brief ĳ������ͨ���ķ����������
//!
//! �˶��в����̰߳�ȫ
//! ��������Ϊ1��
typedef Cycle_Buffer_T<Send_Task*, MAX_SOCKET_SEND_TASK_COUNT> Socket_Send_Task_Queue;


#endif // _SEND_TASK_QUEUE_H_
