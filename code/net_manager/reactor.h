//! @file reactor.h


#ifndef _REACTOR_H_
#define _REACTOR_H_

#include "List.h"

class Event_Handler;

class Reactor;

//! @class Reactor
//! @brief 反应器模型
class Reactor
{
	friend class HandlerIterator;
public:
	virtual ~Reactor();

	//! 事件主循环
	//! @return >=0:处理事件的个数, <0:反应器错误
	virtual int run_reactor_event_loop() = 0;

	//! 停止事件主循环并清理所有注册的处理器
	virtual int end_reactor_event_loop();

	//! 注册一个事件监听
	//! @param event_handler 事件处理器
	//! @param masks 要监听的事件
	//! @return 0:成功, <0:失败
	virtual int register_handler(Event_Handler *event_handler, uint32_t masks) = 0;

	//! 删除一个事件监听
	//! @param event_handler 事件处理器
	//! @param masks 要删除的事件
	//! @return 0:成功, <0:失败
	virtual int remove_handler(Event_Handler *event_handler, uint32_t masks) = 0;

	//! 删除一个sock上的所有监听
	//! @param event_handler 事件处理器
	//! @return 0:成功, <0:失败
	virtual int delete_handler(Event_Handler *event_handler) = 0;

	//! 设置超时
	//! @param id 网络通道
	//! @param timeout 超时时间(秒)
	//! @return 0:成功, <0:失败
	virtual int set_timeout(uint32_t id, int timeout) = 0;

	//! 取消超时
	//! @param id 网络通道
	//! @return 0:成功, <0:失败
	virtual int cancel_timeout(uint32_t id) = 0;

	//! 通知关闭通道
	//! @param id 通道id
	//! @return 0:成功, <0:失败
	virtual int notify_close(uint32_t id) = 0;

	//! 取通道事件处理器
	//! @param id 通道id
	//! @return 事件处理器
	virtual Event_Handler* get_event_handler(uint32_t id) = 0;
public:
	static void initHandlerTable();
	static void finitHandlerTable();
protected:

	static Event_Handler* getHandler(uint32_t id);
	static void putHandler(Event_Handler* pHandler, uint32_t id);
	static void removeHandler(uint32_t id);
	static void removeHandler(Event_Handler* pHandler);
	static int m_HandlerCount;

#ifdef _DEBUG
#define HANDLER_TABLE_SIZE		1027
#else
#define HANDLER_TABLE_SIZE		100003
#endif
	static list_head HandlerTable[HANDLER_TABLE_SIZE];
};


#endif // _REACTOR_H_
