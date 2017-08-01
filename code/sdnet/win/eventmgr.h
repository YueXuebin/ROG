#pragma once

#ifndef EVENTMGR_H
#define EVENTMGR_H

#include "sdsingleton.h"
#include "sddataqueue.h"
#include "netevent.h"
#include "sdmutex.h"
#include "sdloopbuffer.h"
#include "sdobjectpool.h"
#include "thread_buffer_pool.h"

using namespace Crown;

class CSDConnection;

// 网络事件管理器
class CEventMgr
{
	CEventMgr();
	~CEventMgr();

	DECLARE_SINGLETON(CEventMgr)

public:
	/*
	* @brief 初始化网络事件管理器
	* @param nSize : 最大能承载未处理的网络事件的数量
	* @return 是否初始化成功,true为成功,false为不成功
	*/
	bool Init(int32 nSize);

	/*
	* @brief 清理网络事件管理器
	*/
	void Uninit();

	/*
	* @brief 获取一个未处理的网络事件(目前为最先插入的网络事件)
	* @return 返回一个未处理的网络事件.如果处理失败,返回NULL
	* @remark 由于此类只有在主线程中调用,所以,此函数内部并未保证线程安全
	*/
	inline SNetEvent*  PopFrontNetEvt()
	{
		return  (SNetEvent*)m_oEvtQueue.PopFront();
	}

	inline void PushBindEvt(uint32 dwConnectorID, int32	 nSDErrCode, int32 nSysErrCode)
	{
		SNetEvent* pstEvent = _GetFreeEvt();
		CnVerify(pstEvent != NULL);
		pstEvent->nType = NETEVT_BIND_ERR;
		SBindErrEvt* pstBindErr		= &pstEvent->stUn.stBindErr;
		pstBindErr->dwConnectorID	= dwConnectorID;
		pstBindErr->nSDErrCode		= nSDErrCode;
		pstBindErr->nSysErrCode		= nSysErrCode;

		_PushBackNetEvt(pstEvent);
	}

	inline void PushEstablishEvt(CConnData* pConnData, bool bAccepted, uint32 dwParentID)
	{
		SNetEvent* pstEvent = _GetFreeEvt();
		CnVerify(pstEvent != NULL);

		pstEvent->nType					= NETEVT_ESTABLISH;
		SEstablishEvt* pstEstablishEvt	= &pstEvent->stUn.stEstablish;
		pstEstablishEvt->pConnData		= pConnData;
		pstEstablishEvt->bAccepted		= bAccepted;
		pstEstablishEvt->dwParentID		= dwParentID;

		_PushBackNetEvt(pstEvent);
	}
	// 连接事件
	inline void PushAssociateEvt(CConnData* pConnData, uint32 dwConnectionID)
	{
		SNetEvent* pstEvent = _GetFreeEvt();
		CnVerify(pstEvent != NULL);

		pstEvent->nType								= NETEVT_ASSOCIATE;
		pstEvent->stUn.stAssociate.pConnData		= pConnData;
		pstEvent->stUn.stAssociate.dwConnectionID	= dwConnectionID;

		_PushBackNetEvt(pstEvent);		// 事件添加到队列中
	}

	inline void PushConnErrEvt(int32 nSysErrCode, uint32 dwConnectorID)
	{
		SNetEvent* pstEvent = _GetFreeEvt();
		CnVerify(pstEvent != NULL);

		pstEvent->nType				= NETEVT_CONN_ERR;
		SConnErrEvt* pstConnErr		= &pstEvent->stUn.stConnErr;
		pstConnErr->nSysErrCode		= nSysErrCode;
		pstConnErr->dwConnectorID	= dwConnectorID;

		_PushBackNetEvt(pstEvent);
	}

	inline void PushErrorEvt(CConnData* pConnData, uint32 dwConnectionID, int32 nSDErrCode, int32 nSysErrCode)
	{
		SNetEvent* pstEvent = _GetFreeEvt();
		CnVerify(pstEvent != NULL);

		pstEvent->nType				= NETEVT_ERROR;
		SErrorEvt* pstError			= &pstEvent->stUn.stError;
		pstError->pConnData			= pConnData;
		pstError->dwConnectionID	= dwConnectionID;
		pstError->nSDErrCode		= nSDErrCode;
		pstError->nSysErrCode		= nSysErrCode;

		_PushBackNetEvt(pstEvent);
	}

	inline void PushTerminateEvt(CConnData* pConnData, uint32 dwConnectionID)
	{
		SNetEvent* pstEvent = _GetFreeEvt();
		CnVerify(pstEvent != NULL);

		pstEvent->nType					= NETEVT_TERMINATE;
		STerminateEvt* pstTerminate		= &pstEvent->stUn.stTerminate;
		pstTerminate->pConnData			= pConnData;
		pstTerminate->dwConnectionID	= dwConnectionID;

		_PushBackNetEvt(pstEvent);
	}

	inline void PushRecvEvt(CConnData* pConnData, uint32 dwConnectionID, const char* pData, int32 nLen)
	{
		SNetEvent* pstEvent = _GetFreeEvt();
		CnVerify(pstEvent != NULL);
		
		pstEvent->nType							= NETEVT_RECV;
		pstEvent->stUn.stRecv.pConnData			= pConnData;
		pstEvent->stUn.stRecv.dwConnectionID	= dwConnectionID;
		pstEvent->stUn.stRecv.nLen				= nLen;

		pstEvent->stUn.stRecv.pLoopBuf = m_threadBufferPool->PushData(pData, nLen);
		if (pstEvent->stUn.stRecv.pLoopBuf != NULL)
		{
			this->_PushBackNetEvt(pstEvent);
		}
	}

	inline void PushSendEvt(CConnData * pConnData, const char * pBuf, int32 nLen)
	{
		SNetEvent* pstEvent = _GetFreeEvt();
		CnVerify(pstEvent != NULL);

		pstEvent->nType					= NETEVT_SEND;
		pstEvent->stUn.stSend.pConnData = pConnData;
		pstEvent->stUn.stSend.pBuf		= pBuf;
		pstEvent->stUn.stSend.dwLen		= nLen;
		_PushBackNetEvt(pstEvent);
	}

	/*
	* @brief 释放一个网络事件到事件池中
	* @param pstEvt : 释放的网络事件
	*/
	inline void ReleaseNetEvt(SNetEvent* pstEvt)
	{
		m_evtPool.Free(pstEvt);
	}

protected:

	/*
	* @brief 从事件池中获取一个空闲的网络事件结构体
	* @return 返回的空闲的网络事件结构体
	*/
	inline SNetEvent* _GetFreeEvt()
	{
		return m_evtPool.Alloc(); 
	}

	/*
	* @brief 插入一个网络事件供处理
	* @param pstEvt : 插入的网络事件
	*/
	inline void _PushBackNetEvt(SNetEvent* pstEvt)
	{
		m_oQueueLock.Lock();
		if(!m_oEvtQueue.PushBack(pstEvt))
		{
			CRITICAL("CEventMgr::_PushBackNetEvt, fail, Event Queue Full");
			CnVerify(false);
		}
		m_oQueueLock.Unlock();
	}

	///*
	//* @brief 释放一个网络事件到事件池中
	//* @param pstEvt : 释放的网络事件
	//*/
	//inline void _ReleaseEvt(SNetEvent* pstEvt)
	//{
	//	m_evtPool.Free(pstEvt); 
	//}

protected:
	CSDMutex m_oQueueLock;

	CSDObjectPool<SNetEvent, CSDMutex> m_evtPool;

	CSDDataQueue		m_oEvtQueue;
	
	ThreadBufferPool *  m_threadBufferPool;
};

#endif
