#pragma once

#ifndef CONNECTIONMGR_H
#define CONNECTIONMGR_H

#include "sdsingleton.h"
#include "sdconnection.h"
#include "sdmutex.h"
#include <deque>

/**
* @brief 连接管理器
*
*/
class CConnectionMgr
{
	CConnectionMgr();
	~CConnectionMgr();

	DECLARE_SINGLETON(CConnectionMgr)

	typedef std::deque<CSDConnection*> CConnectionPool;
public:

	/**
	* @brief 初始化连接管理器
	* @param nMax 连接管理器中管理的最大连接数量
	* @return 是否初始化成功,true为成功,false为失败
	*/
	bool Init(int32 nMax);

	/**
	* @brief 清理连接管理器
	* @return void
	*/
	void Uninit();

	/**
	* @brief 创建一个连接
	* @return 返回创建的连接
	*/
	CSDConnection* Create();

	/**
	* @brief 回收一个连接
	* @return void
	*/
	void Release(CSDConnection* poConnection);

protected:
	uint32					m_dwNextID;
	int32					m_nMaxConnection;
	CSDConnection*			m_pConnectionArray;
	CConnectionPool			m_oFreeList;

    CSDMutex                   m_mutex;
};

#endif
