#pragma once

#ifndef SDDATAQUEUE_H
#define SDDATAQUEUE_H

#include "sdtype.h"

namespace Crown
{
	/**
	* @defgroup grouploopbuffer 定长的循环数据队列
	* @ingroup  Crown
	* @{
	*/

	/**
	*@brief 循环指针队列，存取为缓冲区指针，支持单线程读写
	*/
	class CSDDataQueue
	{
	public:
		CSDDataQueue(void);
		~CSDDataQueue(void);

		/**
		* @brief
		* 初始化循环缓冲区
		* @param nSize ：分配的长度，实际长度为分配的长度加1
		* @return 成功返回true，失败返回false
		*/
		bool SDAPI Init(int32 nSize);

		/**
		* @brief
		* 将使用完毕的Buffer插入循环缓冲区的结尾
		* @param ptr : 指向Buffer的地址
		* @return 如果循环缓冲区拥有的大小大于nLen，返回true，否则返回false
		* @remark 此函数不是线程安全的
		*/
		bool SDAPI PushBack(void *ptr);

		/**
		* @brief
		* 获取新的Buffer
		* @return 指向Buffer的地址，如果失败，返回NULL
		* @remark 此函数不是线程安全的
		*/
		void* SDAPI PopFront();

	protected:
		void            **m_pArray;
		int32			m_nHead;
		int32			m_nTail;
		int32			m_nSize;
	};

}	// namespace Crown

#endif // SDDATAQUEUE_H
