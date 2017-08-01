//! @file net_thread.h


#ifndef _NET_THREAD_H_
#define _NET_THREAD_H_


#include "task_base.h"


class Net_Manager;
class Reactor;


//! @class Net_Thread
//! @brief 网络处理线程
//!
//! 此线程负责网络数据的收发, 网络通道超时检查, 将待发送数据放入各网络通道的待发送队列
class Net_Thread : public utils::Task_Base
{
public:
	//! 构造函数
	//! @param net_manager 网络管理器
	//! @param reactor 反应器
	Net_Thread(Net_Manager *net_manager, Reactor *reactor);
	virtual ~Net_Thread();

public:
	//! 启动线程
	int start();

	//! 停止线程
	int stop();

	//! 线程函数
	virtual int svc();

public:
	//! 运行状态
	bool m_is_run;

	//! 停止通知
	volatile bool m_notify_stop;

	//! 网络管理器
	Net_Manager *m_net_manager;

	//! 反应器
	Reactor *m_reactor;
};


#endif // _NET_THREAD_H_
