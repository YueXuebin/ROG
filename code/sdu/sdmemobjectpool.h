#pragma once

/******************************************************************************
Copyright (C) Shanda Corporation. All rights reserved.

sdobjectpool.h - 对象池类，分配和管理对象，可以支持多线程模式

******************************************************************************/


#ifndef SDMEMOBJECTPOOL_H
#define SDMEMOBJECTPOOL_H
/**
* @file sdobjectpool.h
* @author lw
* @brief 对象池类
*
**/
#include "sdmemobjectallocator.h"
#include "sdtype.h"

#if SDU_WITH_BOOST
#include <boost/preprocessor/repetition.hpp>
#endif

namespace Crown
{
	#ifdef _USEMEMOBJ
	/**
	* @defgroup groupmemorypool 内存池(包括对象池)
	* @ingroup  Crown
	* @{
	*/

	/**
	*@brief 对象池类
	*/
	template <typename _Tp,typename MT = CSDNonMutex,
		typename _Alloc=MemObjectAllocator<_Tp,MT> >
	class CSDMemObjectPool
	{
	public:
		CSDMemObjectPool(uint32 dwInitCount = 0, uint32 dwGrouCount = 1)
			:m_allocator(dwInitCount,dwGrouCount)
		{
		}


#if SDU_WITH_BOOST

		/**
		* @brief
		* 分配一个对象, 使用boost库，可以将支持的构造函数的参数扩展为20个
		* @return 返回对象的指针，如果失败返回NULL
		*/
#define DECL(z,n,text) T##n t##n
#define MALLOC_DECL(z,n,text)  \
	BOOST_PP_IF(n,template < , ) BOOST_PP_ENUM_PARAMS(n,typename T) BOOST_PP_IF(n,>, ) \
	_Tp* SDAPI Alloc(BOOST_PP_ENUM(n,DECL,~))   \
		{ \
		return new (m_allocator.allocate())_Tp(BOOST_PP_ENUM_PARAMS(n,t)); \
		}

		BOOST_PP_REPEAT (20,MALLOC_DECL,~)



#else
		/**
		* @brief
		* 分配一个对象, 如果对象构造函数没有参数，使用此接口
		* @return 返回对象的指针，如果失败返回NULL
		*/
		inline _Tp* SDAPI  Alloc()
		{
			return new (m_allocator.allocate())_Tp;
		}

		/**
		* @brief
		* 分配一个对象, 对象的构造函数包含一个输入参数，以下一次为多个参数
		* @return 返回对象的指针，如果失败返回NULL
		*/
		template<typename P1>
		_Tp * SDAPI Alloc(P1 p1)
		{
			return new (m_allocator.allocate())_Tp(p1);
		}

		template<typename P1,typename P2>
		_Tp* SDAPI Alloc(P1 p1,P2 p2)
		{
			return new (m_allocator.allocate())_Tp(p1,p2);
		}


		template<typename P1,typename P2,typename P3>
		_Tp* SDAPI Alloc(P1 p1,P2 p2,P3 p3)
		{
			return new (m_allocator.allocate())_Tp(p1,p2,p3);
		}

		template<typename P1,typename P2,typename P3,typename P4>
		_Tp* SDAPI Alloc(P1 p1,P2 p2,P3 p3,P4 p4)
		{
			return new (m_allocator.allocate())_Tp(p1,p2,p3,p4);
		}

		template<typename P1,typename P2,typename P3,typename P4,typename P5>
		_Tp* SDAPI Alloc(P1 p1,P2 p2,P3 p3,P4 p4,P5 p5)
		{
			return new (m_allocator.allocate())_Tp(p1,p2,p3,p4,p5);
		}
#endif // 
		/**
		* @brief
		* 归还一个对象
		* @param pObj : 对象指针
		* @return void
		*/
		void SDAPI Free(_Tp * p)
		{
			p->~_Tp();
			m_allocator.deallocate(p);
		}

		_Alloc & SDAPI GetAllocator()
		{
			return m_allocator;
		}

	private:
		_Alloc m_allocator;

	};

	#endif //#ifdef _USEMEMOBJ
}

#endif //SDMEMOBJECTPOOL_H
