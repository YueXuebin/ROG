#pragma once

#ifndef SD_IDPOOL_H
#define SD_IDPOOL_H
#include <list>
#include "sdtype.h"
#include "sdutil.h"

namespace Crown
{
    template <typename T>
	class CSDIDPool
	{
		typedef struct Node
		{
			T obj;
			uint32 next;
		}Ele;
	public:
		bool Init(uint32 dwSize)
		{			
			m_aObject = SDSAFE_NEW Ele[dwSize];
			memset(m_aObject,0,dwSize*sizeof(Ele));
			for ( uint32 i = 0; i < dwSize; i++ )
				m_aObject[i].next = i+1;
			m_dwSize = m_freeSize = dwSize;
			m_dwFreeHead = 0;
			m_dwFreeTail = dwSize-1;
			return true;
		}

		CSDIDPool(): m_aObject(nullptr), m_dwSize(0) {}

		~CSDIDPool()
		{
			SDSAFE_DELETE [] m_aObject;
		}

		uint32 Add(T object)
		{
			if ( 0 == m_freeSize) return (uint32)(-1);
			uint32 dwID = m_dwFreeHead;
			m_aObject[m_dwFreeHead].obj = object;   //从头拿一个
			m_dwFreeHead = m_aObject[m_dwFreeHead].next; // 更新队头
			m_aObject[dwID].next = (uint32)-1;//next是－1表示在使用中
			--m_freeSize;		
			return dwID;
		}

		bool Find(uint32 dwID, T& pObject)
		{
			if ((dwID >= m_dwSize) )
			{
				return false;
			}

			if (m_aObject[dwID].next != (uint32)(-1)) //不是在使用中
				 return false;

			pObject = m_aObject[dwID].obj;
			return true;
		}

		void Del(uint32 dwID)
		{
			if ((dwID >= m_dwSize) )
			{
				return;
			}

			if (m_aObject[dwID].next != (uint32)(-1))  //不是在使用中
				return ;
			++m_freeSize;
			if ( m_freeSize == 1) //还回来的第一个节点，是头又是尾
			{
				m_dwFreeHead = m_dwFreeTail = dwID;
				m_aObject[m_dwFreeHead].next = m_dwSize;
				return;
			}
			m_aObject[dwID].next = m_dwSize;  //这个表示无后继了
			m_aObject[m_dwFreeTail].next = dwID; //放到队尾
			m_dwFreeTail = dwID;  //更新队尾		
		}

        template<class CFunc>
        bool For_Each(CFunc oFunc)
        {
            if(IsEmpty())
                return true;

            uint32 dwSize = 0;
            uint32 dwCurrentSize = Capacity() - GetFreeCount();
            for(uint32 i = 0; i < m_dwSize; ++i)
            {
                if(m_aObject[i].next == (uint32)(-1))
                {
                    oFunc(m_aObject[i].obj);
                    dwSize++;

                    if(dwSize == dwCurrentSize)
                        break;
                }
            }
            return true;
        }

		std::list<uint32>::size_type GetFreeCount() const
		{
			return m_freeSize;
		}

		bool IsEmpty() const
		{
			return ( m_freeSize == m_dwSize );
		}

		bool IsFull() const
		{
			return ( m_freeSize == 0 );
		}

		std::list<uint32>::size_type Capacity() const
		{
			return m_dwSize;
		}

		//  获取当前使用率
		uint32 GetUSG()
		{
			return 100 - m_freeSize*100/m_dwSize;
		}
		
	private:
		uint32			m_dwSize;
		uint32			m_freeSize;
		Ele *			m_aObject;
		uint32			m_dwFreeHead;
		uint32			m_dwFreeTail;	
	};
}
#endif
