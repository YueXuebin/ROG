#pragma once

/******************************************************************************
Copyright (C) Shanda Corporation. All rights reserved.


******************************************************************************/

#ifndef SDMEMOBJECTPOOLALLOCATOR_H
#define SDMEMOBJECTPOOLALLOCATOR_H

#include "sdmutex.h"
#include "sdlock.h"
#include "sdmemdefines.h"
#include "sdmemallocatormgr.h"
#include <deque>
#include <vector>

namespace Crown
{
	#ifdef _USEMEMOBJ

	template<typename T, typename MT = CSDNonMutex>
	class MemObjectAllocator
	{
		enum
		{
			POOL_INFLATE_SIZE= 100,
		};
	public:
		typedef size_t   size_type;
		typedef T*       pointer;
		typedef T&       reference;
		typedef const T* const_pointer;
		typedef const T& const_reference;
		typedef T        value_type;

		typedef std::deque<pointer> ObjectQueue;

		typedef typename ObjectQueue::iterator ObjectQueueItr;

		MemObjectAllocator(int32 initSize = 0, int32 growSize = 1 ) throw()
		{
			m_totalSize =0;
			m_growSize = growSize;
			CSDLock<MT> lock(m_mutex);
			inflate(initSize );
		}


		~MemObjectAllocator() throw()
		{
			CSDLock<MT> lock(m_mutex);

			for(typename ObjectQueue::iterator itr = m_objects.begin(); itr != m_objects.end(); ++itr)
			{
				CSDMemAllocatorMgr::Instance()->GetAllocator()->Deallocate(*itr, SD_OPER_DELETE, sizeof(value_type));
			}
			m_objects.clear();
		}

		pointer address(reference __x) const
		{
			return &__x;
		}

		const_pointer address(const_reference __x) const
		{
			return &__x;
		}

		T* allocate()
		{
			m_mutex.Lock();
			if (m_objects.empty())
			{
				this->inflate(m_growSize);
			}

			T * ret = m_objects.front();
			m_objects.pop_front();
			m_mutex.Unlock();
			return ret;
		}

		// __p is not permitted to be a null pointer.
		void deallocate(pointer __p)
		{
			m_mutex.Lock();
			m_objects.push_back(__p);
			m_mutex.Unlock();
		}

		inline size_type read_size()
		{
			CSDLock<MT> lock(m_mutex);
			return m_objects.size();
		}

		inline size_type write_size() const throw()
		{
			CSDLock<MT> lock(m_mutex);
			return m_objects.size() ;
		}


	private:
		void inflate(int32 count = POOL_INFLATE_SIZE)
		{
			//NOTICE: locked outside
			size_t	stSize = sizeof(value_type);
			size_t	stAlign = SD_MEM_ALIGNMENT_DEFAULT;
			for (int32 i = 0; i < count ; i++)
			{
				pointer  _p = static_cast<pointer>(CSDMemAllocatorMgr::Instance()->GetAllocator()->Allocate(stSize
					, stAlign
					, SD_OPER_NEW));
				m_objects.push_back(_p);
			}
			m_totalSize += count;
		}

		MemObjectAllocator(const MemObjectAllocator&) throw()
		{
		}

		// not allow copy
		template<typename T1>
		MemObjectAllocator(const MemObjectAllocator<T1>&) throw()
		{
		}

		template<typename T1>
		void operator = (const MemObjectAllocator<T1>&) throw()
		{

		}

		ObjectQueue m_objects;
		MT m_mutex;
		int32 m_totalSize ;
		int32 m_growSize;
	};

	#endif //#ifdef _USEMEMOBJ
}
#endif // SDMEMOBJECTPOOLALLOCATOR_H


