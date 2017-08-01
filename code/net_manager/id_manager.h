//! @file id_manager.h


#ifndef _ID_MANAGER_H_
#define _ID_MANAGER_H_

#include "types.h"
#include "cycle_buffer.h"


//! @enum ID_TYPE
//! @brief 网络通道id分类
enum ID_TYPE
{
	ID_UDP,
	ID_TCP_CONNECTOR,
	ID_TCP_ACCEPTOR,
	ID_TCP_STREAM
};


//! @class Id_Manager
//! @brief 网络通道id管理器
//!
//! 此类维护数量足够的网络通道id, 实际运行过程中不可能出现id用尽的情况
class Id_Manager
{
public:
	Id_Manager();
	~Id_Manager();

	//! 分配一个网络通道id
	//! @param type 要分配的通道类型
	//! @return 通道id 0:失败,id用尽,  >0:成功
	uint32_t acquire(ID_TYPE type);

	//! 释放网络通道id
	//! @param id 网络通道id
	void release(uint32_t id);

private:
	static const uint32_t Q1_B = 1;
	static const uint32_t Q1_E = 100000;

	static const uint32_t Q2_B = 100001;
	static const uint32_t Q2_E = 200000;

	static const uint32_t Q3_B = 200001;
	static const uint32_t Q3_E = 300000;

	static const uint32_t Q4_B = 300001;
	static const uint32_t Q4_E = 1000000;

	//! 网络通道id队列1
	typedef Cycle_Buffer_T<uint32_t, Q1_E - Q1_B + 1> Id_Queue1;
	Id_Queue1 *m_id_queue1;

	//! 网络通道id队列2
	typedef Cycle_Buffer_T<uint32_t, Q2_E - Q2_B + 1> Id_Queue2;
	Id_Queue2 *m_id_queue2;

	//! 网络通道id队列3
	typedef Cycle_Buffer_T<uint32_t, Q3_E - Q3_B + 1> Id_Queue3;
	Id_Queue3 *m_id_queue3;

	//! 网络通道id队列1
	typedef Cycle_Buffer_T<uint32_t, Q4_E - Q4_B + 1> Id_Queue4;
	Id_Queue4 *m_id_queue4;
};


#endif // _ID_MANAGER_H_
