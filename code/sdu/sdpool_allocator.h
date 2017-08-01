#pragma once

/*
 *	用链表实现的对象池分配器,而且会自动释放未deallocate的对象
 */
#ifndef sdpool_allocator_h__
#define sdpool_allocator_h__

#include "sdmutex.h"
#include "sdlock.h"
#include "buildconfig.h"
#include <assert.h>			// for just include this file

namespace Crown
{
	template<typename T>
	struct _PoolMemLinkNode {
		_PoolMemLinkNode* pPrev;
		_PoolMemLinkNode* pNext;
	#ifdef _DEBUG
		uint32 dbgCheckFlag; // 内存检查
	#endif
        char _val[sizeof(T)];

		_PoolMemLinkNode() :
            pPrev(nullptr), pNext(nullptr)
	#ifdef _DEBUG
		, dbgCheckFlag(0xdeaddead)
	#endif
        {}

        T& Val() { return *reinterpret_cast<T*>(this->_val); }

		void unlink() {
			if (pPrev)
				pPrev->pNext = pNext;
			if (pNext)
				pNext->pPrev = pPrev;
			pPrev = pNext = nullptr;
		}
	};

	template<typename T, typename MT = CSDNonMutex>
	class PoolAllocator
	{
		enum
		{
			POOL_INFLATE_SIZE= 100,
            VAL_OFFSET = offsetof(_PoolMemLinkNode<T>, _val)
		};
	protected:
		_PoolMemLinkNode<T>* m_FreeListHeader;		// 标记FreeList头部元素的指针
		_PoolMemLinkNode<T>* m_BusyListHeader;		// 标记BusyList头部元素的指针

		int		m_Grow;
		int		m_Size;					// 当前元素个数
		int		m_MaxSize;				// 分配的元素个数
		MT		m_mutex;

	public:
		PoolAllocator(int32 initSize = 0, int32 growSize = 1) :
			m_FreeListHeader(nullptr), m_BusyListHeader(nullptr),
            m_Grow(growSize), m_Size(0), m_MaxSize(0)
		{
			CSDLock<MT> lock(m_mutex);
			Grow(initSize);
			m_Grow = growSize;
		}
		~PoolAllocator();	

		T* allocate();
		void deallocate(T* pVal);

		int Size() {return m_Size;}

		bool IsEmpty()
		{
			CnVerify( (m_BusyListHeader && (m_Size > 0) ) || (!m_BusyListHeader && (m_Size == 0) ) );
			return (m_BusyListHeader == nullptr);
		}

	protected:
		void Grow(int32 count = POOL_INFLATE_SIZE);

	private:
		// not allow copy
		PoolAllocator(const PoolAllocator&) throw() {}

		template<typename T1>
		PoolAllocator(const PoolAllocator<T1>&) throw()	{}

		template<typename T1>
		void operator = (const PoolAllocator<T1>&) throw() {}


	};		// class PoolAllocator

	template<typename T, typename MT>
	PoolAllocator<T, MT>::~PoolAllocator() 
	{
		int checkSize = 0;
		if(m_BusyListHeader)
		{
			_PoolMemLinkNode<T>* pCurNode = m_BusyListHeader;
			_PoolMemLinkNode<T>* pDelNode = pCurNode;
			while(pCurNode)
			{
				pCurNode = pCurNode->pNext;
                pDelNode->Val().~T();			// 没有释放，在此强制调用析构函数，垃圾回收器
				CnFree(pDelNode);
				pDelNode = pCurNode;
				checkSize++;
				m_Size--;
			}
		}

		_PoolMemLinkNode<T>* pCurNode = m_FreeListHeader;
		_PoolMemLinkNode<T>* pDelNode = pCurNode;
		while(pCurNode)
		{
			pCurNode = pCurNode->pNext;
			CnFree(pDelNode);
			pDelNode = pCurNode;
			checkSize++;
		}
		CnVerify(checkSize == m_MaxSize);
		CnVerify(m_Size == 0);

	};

	template<typename T, typename MT>
	void PoolAllocator<T, MT>::Grow(int32 count)
	{
		if( count == 0 )
			return;

		m_Grow = count;
		CnVerify(!m_FreeListHeader);		// FreeList中还有元素，就不要增长
		for(int i = 0; i < m_Grow ; i++)
		{
			_PoolMemLinkNode<T>* pNewNode = static_cast<_PoolMemLinkNode<T>*>( CnMalloc( sizeof(_PoolMemLinkNode<T>) ) );
			// Node初始化
            pNewNode->pPrev = pNewNode->pNext = nullptr;
#ifdef _DEBUG
			pNewNode->dbgCheckFlag = 0xdeaddead;
#endif
			if( m_FreeListHeader )
			{
				pNewNode->pNext = m_FreeListHeader;
				m_FreeListHeader->pPrev = pNewNode;

				m_FreeListHeader = pNewNode;
			}
			else
			{
				m_FreeListHeader = pNewNode;
			}
		}		
		m_MaxSize += m_Grow;
		m_Grow *= 2;		// 下次再生成元素是这次的2倍
	}

	template<typename T, typename MT>
	T* PoolAllocator<T, MT>::allocate()
	{
		CSDLock<MT> lock(m_mutex);

		if( !m_FreeListHeader )
		{	// FreeList中没有元素了
			Grow(m_Grow);
		}

		_PoolMemLinkNode<T>* pCurNode = m_FreeListHeader;
		// 把该元素从FreeList中移出
        T* pVal = &m_FreeListHeader->Val();
		m_FreeListHeader = m_FreeListHeader->pNext;

		pCurNode->unlink();

		// 把该元素加入到BusyList中
		pCurNode->pNext = m_BusyListHeader;
		if(m_BusyListHeader)
			m_BusyListHeader->pPrev = pCurNode;
		m_BusyListHeader = pCurNode;
		
		m_Size++;
		return pVal;
	}

	template<typename T, typename MT>
	void PoolAllocator<T, MT>::deallocate(T* pVal)
	{
		CSDLock<MT> lock(m_mutex);
		
        // 编译期常量计算
        const std::ptrdiff_t offset = VAL_OFFSET;

        _PoolMemLinkNode<T>* pCurNode = reinterpret_cast<_PoolMemLinkNode<T>*>(reinterpret_cast<char*>(pVal) - offset);

#ifdef _DEBUG
		CnVerify(pCurNode->dbgCheckFlag == 0xdeaddead);			// 还回来的内存，不是从这里Pool中分配出来的
#endif
		// 把该元素从BusyList中移出
		if(pCurNode == m_BusyListHeader)
			m_BusyListHeader = pCurNode->pNext;
		pCurNode->unlink();

		// 把该元素加入到FreeList中
		pCurNode->pNext = m_FreeListHeader;
		if(m_FreeListHeader)
			m_FreeListHeader->pPrev = pCurNode;
		m_FreeListHeader = pCurNode; 
		
		m_Size--;
	}
	
}	// namespace Crown

#endif // sdpool_allocator_h__
