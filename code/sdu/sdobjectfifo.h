#pragma once

/*
 *	针对对象的FIFO, 用PoolAllocator实现
 */
#ifndef sdobjectfifo_h__
#define sdobjectfifo_h__

#include "sdpool_allocator.h"

namespace Crown
{
	// 针对对象的FIFO
	template <typename _Tp,typename MT = CSDMutex>
	class CSDObjectFifo : public PoolAllocator<_Tp, CSDMutex>
	{
	public:
		CSDObjectFifo(uint32 dwInitCount = 0, uint32 dwGrouCount = 1)
			: PoolAllocator<_Tp, MT>(dwInitCount, dwGrouCount),
			m_BusyListTailor(nullptr)
		{

		}
		~CSDObjectFifo()
		{

		}

		void Push(const _Tp& pT)
		{
			CSDLock<MT> lock(m_mutex);

			if( !this->m_FreeListHeader )
			{	// FreeList中没有元素了
				this->Grow(this->m_Grow);
			}

			_PoolMemLinkNode<_Tp>* pCurNode = this->m_FreeListHeader;
			// 把该元素从FreeList中移出
			this->m_FreeListHeader = this->m_FreeListHeader->pNext;

			pCurNode->unlink();

			// 把该元素加入到BusyList头部
			pCurNode->pNext = this->m_BusyListHeader;
			if(this->m_BusyListHeader)
			{
				this->m_BusyListHeader->pPrev = pCurNode;
			}
			else
			{
				CnVerify(!m_BusyListTailor);
				m_BusyListTailor = pCurNode;
			}
			this->m_BusyListHeader = pCurNode;

			// 赋值
			pCurNode->_val = pT;
			this->m_Size++;
		}

		bool Pop(_Tp& pT)
		{
			CSDLock<MT> lock(m_mutex);

			if( !m_BusyListTailor )
				return false;

			// 取值
			pT = m_BusyListTailor->_val;		

			// 该Node从BusyList的末端取走
			_PoolMemLinkNode<_Tp>* pCurNode = m_BusyListTailor;
			if(pCurNode == this->m_BusyListHeader)
			{
				CnVerify(m_BusyListTailor->pNext == nullptr && m_BusyListTailor->pPrev == nullptr);
				this->m_BusyListHeader = nullptr;
			}
			m_BusyListTailor = pCurNode->pPrev;	

			pCurNode->unlink();

			// 把该Node加入到FreeList中
			pCurNode->pNext = this->m_FreeListHeader;
			if(this->m_FreeListHeader)
				this->m_FreeListHeader->pPrev = pCurNode;
			this->m_FreeListHeader = pCurNode;
			
			this->m_Size--;
			return true;
		}

	protected:
		MT		m_mutex;
		_PoolMemLinkNode<_Tp>*	m_BusyListTailor;			// 标记BusyList尾部元素的指针

	};	// CSDObjectFifo

}	// namespace Crown

#endif // sdobjectfifo_h__
