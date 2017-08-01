#pragma once

/******************************************************************************
Copyright (C) Shanda Corporation. All rights reserved.

sdfifo.h - 先进先出对象队列。

******************************************************************************/

#ifndef SDFIFO_H
#define SDFIFO_H
/**
* @file sdfifo.h
* @author lw
* @brief 先进先出队列
*
**/
/**
* @file sdfifo.h
* @author wk
* @brief 先进先出对象队列。
*
**/
#include "sdtype.h"
#include "sdfifodqueue.h"
#include "sdfifoqueue.h"
namespace Crown 
{


	/**
	* @brief 先进先出对象队列, 支持多线程和单线程, 是否为多线程,由Fifo确定
	*
	*/
	template <typename T, typename Fifo = CSDFifoQueue<T, CSDMutex> >
	class CSDFifo
	{

	public:
		/**
		* @brief
		* 从消息队列获取消息
		* @param ms : 获取消息的超时时间
		* 当ms等于零时，检查一次，无论是否能获取结果，立即返回
		* 当ms大于零时，等待相应的时间
		* @return 获取的消息指针，如果超时，返回NULL
		*/
		inline  T* SDAPI Pop(uint32 ms= 0)
		{
			return m_fifo.Pop(ms);
		}

		/**
		* @brief
		* 向消息队列写入消息
		* @param pItem : 写入的消息指针
		* @return void
		*/
		inline void SDAPI Push(T * pItem)
		{
			m_fifo.Push(pItem);
		}

		/**
		* @brief
		* 当前在读队列中的可读的消息数
		* @return void
		*/
		inline size_t SDAPI ReadSize()
		{
			return m_fifo.ReadSize();
		}

		/**
		* @brief
		* 当前写队列中的消息个数
		* @return 已经写的消息个数
		*/
		inline size_t SDAPI WriteSize()
		{
			return m_fifo.WriteSize();
		}
	private:
		Fifo  m_fifo;
	};
}

#endif // 
