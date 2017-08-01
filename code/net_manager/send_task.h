//! @file send_task.h


#ifndef _SEND_TASK_H_
#define _SEND_TASK_H_


#include "types.h"
#include "inet_addr.h"
#include "net_packet.h"
#include "pool_allocator.h"
#include "config.h"


//! @class Send_Task
//! @brief 发送任务
class Send_Task
{
public:
	//! 构造函数
	//! @param id 通道id
	//! @param remote_addr 对端地址
	//! @param packet 要发送的包, 构造完成后packet的生命周期由Send_Task管理
	Send_Task(uint32_t id, const INET_Addr& remote_addr, Net_Packet *packet);

	//! 析构函数
	~Send_Task();

	DECLARE_ALLOCATOR(Send_Task, SEND_TASK_POOL_COUNT)

public:
	//! 通道id
	uint32_t id;

	//! 对端地址
	INET_Addr remote_addr;

	//! 要发送的包
	//! packet由应用层分配并初始化, 由网络层发送后释放
	Net_Packet *packet;
};


#endif // _SEND_TASK_H_
