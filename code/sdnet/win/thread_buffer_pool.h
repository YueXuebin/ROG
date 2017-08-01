#pragma once

#ifndef __THREAD_BUFFER_POOL_H__
#define __THREAD_BUFFER_POOL_H__

#include "sdthread.h"
#include <Windows.h>
#include "simpleloopbuffer.h"
#include <iostream>

/**
* @brief 实现一个线程安全的，在不同线程代表不同的Buffer池（使用ThreadLocal的方式）
*
*
*/
class ThreadBufferPool
{
public:
	ThreadBufferPool()
	{
		m_tlsIndex = TlsAlloc(); 
	}
 
	/**
	* @brief 创建ThreadLocal的缓存
	*
	*
	*/
	bool CreateThreadBuffer()
	{
		LPVOID pBuf = (LPVOID)LocalAlloc(LPTR, sizeof(CSimpleLoopBuffer));
		CSimpleLoopBuffer * pLoopBuf = new (pBuf)CSimpleLoopBuffer();	// placement new
		pLoopBuf->Init( 4096*1024 );
		return (TlsSetValue(m_tlsIndex, pBuf) ? true : false);
	}

	/**
	* @brief 释放ThreadLocal的缓存
	*
	*
	*/
	void ReleaseThreadBuffer()
	{
		CSimpleLoopBuffer * pBuf = (CSimpleLoopBuffer*)TlsGetValue(m_tlsIndex);
		pBuf->~CSimpleLoopBuffer();
		if (pBuf != NULL)
		{
			LocalFree((HLOCAL) pBuf);
		}
	}

	/**
	* @brief 向ThreadLocal的缓存插入数据
	* @param pData : 插入的数据指针
	* @param nLen : 数据的长度
	* @return ThreadLocal的缓存，需要注意的是，返回的缓存的线程安全性需要由用户保证。
	* 用户需要明确返回的缓存只在当前的线程中使用
	*/
	CSimpleLoopBuffer * PushData(const char * pData, int nLen)
	{
		CSimpleLoopBuffer* pLoopBuf = (CSimpleLoopBuffer*)TlsGetValue(m_tlsIndex);
		if (pLoopBuf != NULL)
		{
			while ( !pLoopBuf->PushBack(pData, nLen) )
			{
				//Sleep(1);
			}
		}
		return pLoopBuf;
	}
private:

	DWORD  m_tlsIndex;
};


#endif // 