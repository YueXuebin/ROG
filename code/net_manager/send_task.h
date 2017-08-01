//! @file send_task.h


#ifndef _SEND_TASK_H_
#define _SEND_TASK_H_


#include "types.h"
#include "inet_addr.h"
#include "net_packet.h"
#include "pool_allocator.h"
#include "config.h"


//! @class Send_Task
//! @brief ��������
class Send_Task
{
public:
	//! ���캯��
	//! @param id ͨ��id
	//! @param remote_addr �Զ˵�ַ
	//! @param packet Ҫ���͵İ�, ������ɺ�packet������������Send_Task����
	Send_Task(uint32_t id, const INET_Addr& remote_addr, Net_Packet *packet);

	//! ��������
	~Send_Task();

	DECLARE_ALLOCATOR(Send_Task, SEND_TASK_POOL_COUNT)

public:
	//! ͨ��id
	uint32_t id;

	//! �Զ˵�ַ
	INET_Addr remote_addr;

	//! Ҫ���͵İ�
	//! packet��Ӧ�ò���䲢��ʼ��, ������㷢�ͺ��ͷ�
	Net_Packet *packet;
};


#endif // _SEND_TASK_H_
