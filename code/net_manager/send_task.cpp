#include "net_manager_pch.h"
#include "send_task.h"


// class Send_Task
Send_Task::Send_Task(uint32_t id, const INET_Addr& remote_addr, Net_Packet *packet)
{
	this->id = id;
	this->remote_addr = remote_addr;
	this->packet = packet;
}

Send_Task::~Send_Task()
{
	if (NULL != packet)
	{
		delete packet;
	}
}

IMPLEMENT_ALLOCATOR(Send_Task)

