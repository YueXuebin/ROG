#pragma once

#ifndef SDINDEXER_IMPL_H
#define SDINDEXER_IMPL_H

namespace Crown
{

#ifndef SDINVALID_INDEX
#define SDINVALID_INDEX 0  //定义无效的索引为0 
#endif

	template <class T>
	struct Container
	{
		Container()
		{
			bUsed = false;
			mask = 0;
		}
		uint32 mask;
		volatile uint32 bUsed;
		T data;
	};

	template <class T> 
	struct DefaultValue
	{
		void operator() (T & data)
		{
			data = T();
		}
	};

	template <class T> 
	struct DefaultValue<T * > 
	{
		void operator() (T & data)
		{
			data = nullptr;
		}
	};

	template <class T>
	struct DefaultEqual
	{
		bool operator ()( const T& left, const T& right)
		{
			return left == right;
		}
	};


	template < class T, uint32 maxSize = 1024, class ValueEqual = DefaultEqual<T> >
	class SDIndexerImpl
	{
	public:
		SDIndexerImpl()
		{
			DefaultValue<T> defValue;
			for (uint32 i = 0; i < maxSize; i ++)
			{
				defValue(m_array[i].data);
				m_array[i].bUsed = false;
			}
			m_curIndex = maxSize >> 1;
			m_freeCount = maxSize - 1;
		}

		uint32 Alloc(T & value)
		{
			while (m_freeCount > 0)
			{
				for (uint32 curIdx = m_curIndex; curIdx < maxSize; curIdx++)
				{
					//当此Index未被使用时
					if ( !SDAtomicCas32((volatile int32*)&(m_array[curIdx].bUsed), true, false) )
					{
						m_array[curIdx].data = value;
						//减一操作,如果多线程同时减,会将此操作数置0
						SDAtomicDec32((volatile int32*)&m_freeCount);
						SDAtomicSet32((volatile int32 *)&m_curIndex, (m_curIndex)%(maxSize-2) + 1);
						return curIdx;
					}
				}

				for (uint32 curIdx = 1; curIdx < m_curIndex; curIdx++)
				{
					//当此Index未被使用时
					if ( !SDAtomicCas32((volatile int32*)&(m_array[curIdx].bUsed), true, false) )
					{
						m_array[curIdx].data = value;
						//减一操作,如果多线程同时减,会将此操作数置0
						SDAtomicDec32((volatile int32*)&m_freeCount);
						SDAtomicSet32((volatile int32 *)&m_curIndex, (m_curIndex)%(maxSize - 2) + 1);
						return curIdx;
					}
				}
			}
			return SDINVALID_INDEX;
		}

		//根据数据查找对应的ID 
		uint32 Find(const T & data)
		{
			for (uint32 i = 1; i < maxSize; i++)
			{
				if (m_equaler(m_array[i].data, data))
				{
					return i;
				}
			}
			return SDINVALID_INDEX;
		}

		bool Exist(uint32 idx)
		{
			if (idx < maxSize)
			{
				return m_array[idx].bUsed;
			}
			return false;
		}

		T & Get(uint32 idx)
		{
			static T defValue;
			if (idx < maxSize && m_array[idx].bUsed)
			{
				return m_array[idx].data;
			}
			return defValue;
		}

		T& Free(uint32 idx)
		{
			static T defValue;
			if (idx < maxSize && idx > 0)
			{
				SDAtomicInc32((volatile int32 *)&m_freeCount);
				SDAtomicCas32((volatile int32*)&(m_array[idx].bUsed), false, true);
				return m_array[idx].data;
			}
			return defValue;
		}

	private:
		ValueEqual  m_equaler;
		uint32 m_curIndex;
		uint32 m_indexMask;
		uint32 m_freeCount;
		Container<T> m_array[maxSize];
	};

}

#endif // 

