#pragma once

/*
 *	线程安全的将一组对象映射为一个ID值，ID值的范围从1-maxSize，0为无效ID
 */
#ifndef SDINDEXER_H
#define SDINDEXER_H
#include "sdatomic.h"
#include <algorithm>
#include "sdatomic.h"
#include "sdindexer_impl.h"

/**
* @file sdindexer.h
* @author wangkui
* @brief 将数据映射为连续的索引值
*
**/
namespace Crown 
{

    /**
    * @defgroup groupindexer 索引化类集合
    * @ingroup  Crown
    * @{
    */

#ifndef SDINVALID_INDEX 
#define SDINVALID_INDEX 0  //定义无效的索引为0 
#endif 

	/**
	* @brief 索引化类
	*
	*/
	template <class T, uint32 maxSize = 1024, class ValueEqual = DefaultEqual<T> > 
	class SDIndexer
	{
	public:
		/**
		* @brief
		* 分配一个ID值，
		* @param value : 此ID对应相应的数据
		* @return 分配的ID值，如果无效为SDINVAID_INDEX
		*/
		inline uint32 Alloc( T & data)
		{
			return m_indexerImpl.Alloc(data); 
		}

		/**
		* @brief
		* 根据数据查找对应的ID ，使用情况比较少，但有时候会需要，查找复杂度为O(n)
		* @param value : 此ID对应相应的数据
		* @return 分配的ID值，如果没有此数据，返回无效SDINVAID_INDEX
		*/
		inline uint32 Find(const T & data)
		{
			return m_indexerImpl.Find(data); 
		}

		/**
		* @brief
		* 根据ID值获取对应的数据，如果失败为该类型的默认值
		* @param idx : 分配的ID值
		* @return 该ID值对应的数据
		*/
		inline T & Get(uint32 idx)
		{
			return m_indexerImpl.Get(idx); 
		}

		/**
		* @brief
		* 判断该ID值对应的数据是否有效，是否存在有效数据
		* @param idx : 分配的ID值
		* @return 该ID值是否有效
		*/
		inline bool Exist(uint32 idx)
		{
			return m_indexerImpl.Exist(idx); 
		}

		/**
		* @brief
		* 释放该ID值，返回该ID对应的数据 
		* @param idx : 分配的ID值
		* @return 该ID值对应的数据
		*/
		inline T& Free(uint32 idx )
		{
			return m_indexerImpl.Free(idx); 
		}

	private:
		SDIndexerImpl<T, maxSize, ValueEqual> m_indexerImpl; 
	};

}// namespace Crown 

#endif // 


