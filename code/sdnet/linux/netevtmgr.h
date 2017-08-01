#ifndef NETEVTMGR_H
#define NETEVTMGR_H

#include <list>
#include "netevent.h"
#include "sddataqueue.h"
#include "simpleloopbuffer.h"

using namespace Crown;

typedef std::list<SNetEvent*> CListNetEvent;


class CNetEvtMgr
{
	CNetEvtMgr(void);
	virtual ~CNetEvtMgr(void);
public:

	static bool CreateInstance()
	{
		if(NULL == m_poInstance)
		{
			m_poInstance = CnNew CNetEvtMgr;
		}

		return m_poInstance != NULL;
	}
	static CNetEvtMgr * Instance(){ return m_poInstance; }
	static void DestoryInstance()
	{
		if(m_poInstance != NULL)
		{
			CnDelete m_poInstance;
			m_poInstance = NULL;
		}
	}

	bool Init(int nSize, CSimpleLoopBuffer* poRecvQueue);

	//获取空闲Evt
	SNetEvent* GetFreeEvt();
	
	//将空闲Evt放回池
	inline void ReleaseEvt(SNetEvent* pstEvt)
	{
		if(!m_oFreeQueue.PushBack(pstEvt))
		{
			CRITICAL("Release event fail! FreeQueue full!");
			CnAssert(false);
		}
	}
	
	//弹出Evt
	bool PopFrontNetEvt(SNetEvent* pstEvent);
	//弹出到临时缓冲区，为了解决事件、请求队列可能引发的死锁情况
	void PopToBuffer();

	inline void PushEstablishEvt(uint32 dwSockID, uint32 dwConnID,	bool bAccepted,	uint32 dwParentID)
	{
		SNetEvent* pstEvent = GetFreeEvt();
		CnAssert(pstEvent != NULL);

		pstEvent->nType = NETEVT_ESTABLISH;
		SEstablishEvt* pstEstablishEvt	= &pstEvent->stUn.stEstablish;
		pstEstablishEvt->dwSockID		= dwSockID;
		pstEstablishEvt->dwConnID		= dwConnID;
		pstEstablishEvt->bAccepted		= bAccepted;
		pstEstablishEvt->dwParentID		= dwParentID;

		PushBackNetEvt(pstEvent);
	}
	inline void PushConnErrEvt(int32 nSysErrCode, uint32 dwConnectorID)
	{
		SNetEvent* pstEvent = GetFreeEvt();
		CnAssert(pstEvent != NULL);

		pstEvent->nType = NETEVT_CONN_ERR;
		SConnErrEvt* pstConnErr = &pstEvent->stUn.stConnErr;
		pstConnErr->nSysErrCode = nSysErrCode;
		pstConnErr->dwConnectorID = dwConnectorID;

		PushBackNetEvt(pstEvent);
	}
	inline void PushErrorEvt(uint32 dwConnectionID, int32 nSDErrCode, int32 nSysErrCode)
	{
		SNetEvent* pstEvent = GetFreeEvt();
		CnAssert(pstEvent != NULL);

		pstEvent->nType = NETEVT_ERROR;
		SErrorEvt* pstError = &pstEvent->stUn.stError;
		pstError->dwConnectionID = dwConnectionID;
		pstError->nSDErrCode = nSDErrCode;
		pstError->nSysErrCode = nSysErrCode;

		PushBackNetEvt(pstEvent);
	}
	inline void PushTerminateEvt(uint32 dwConnectionID, bool bAccepted, uint32 dwParentID)
	{
		SNetEvent* pstEvent = GetFreeEvt();
		CnAssert(pstEvent != NULL);

		pstEvent->nType = NETEVT_TERMINATE;
		STerminateEvt* pstTerminate = &pstEvent->stUn.stTerminate;
		pstTerminate->dwConnectionID = dwConnectionID;
		pstTerminate->bAccepted = bAccepted;
		pstTerminate->dwParentID = dwParentID;

		PushBackNetEvt(pstEvent);
	}
	inline void PushRecvEvt(uint32 dwConnectionID, int32 nLen)
	{
		SNetEvent* pstEvent = GetFreeEvt();
		CnAssert(pstEvent != NULL);

		pstEvent->nType = NETEVT_RECV;
		pstEvent->stUn.stRecv.dwConnectionID = dwConnectionID;
		pstEvent->stUn.stRecv.nLen = nLen;
		pstEvent->stUn.stRecv.pData = NULL;

		PushBackNetEvt(pstEvent);
	}

	inline void PushSendEvt(uint32 dwConnectionID,const char * pBuf,int32 nLen)
	{
		SNetEvent* pstEvent = GetFreeEvt();
		CnAssert(pstEvent != NULL);
		pstEvent->nType = NETEVT_SEND; 
		pstEvent->stUn.stSend.dwConnectionID = dwConnectionID;
		pstEvent->stUn.stSend.pBuf = pBuf; 
		pstEvent->stUn.stSend.dwLen = nLen; 
		PushBackNetEvt(pstEvent);
	}
	inline void PushBindErrEvt(int32 nSysErrCode, uint32 dwConnectorID)
	{
		SNetEvent* pstEvent = GetFreeEvt();
		CnAssert(pstEvent != NULL);

		pstEvent->nType = NETEVT_BINDERR;
		SBindErrEvt* pstBindErr = &pstEvent->stUn.stBindErr;
		pstBindErr->nSysErrCode = nSysErrCode;
		pstBindErr->dwConnectorID = dwConnectorID;

		PushBackNetEvt(pstEvent);
	}
protected:
	//压入Evt
	void PushBackNetEvt(SNetEvent* pstEvt)
	{
		if(!m_oEvtQueue.PushBack(pstEvt))
		{
			CRITICAL("PushBackNetEvt fail, Event Queue Full");
			CnAssert(false);
		}
	}

protected:
	static CNetEvtMgr*	m_poInstance;
	SNetEvent*			m_pEvtArray;
	CSDDataQueue			m_oFreeQueue;
	CSDDataQueue			m_oEvtQueue;
	CListNetEvent		m_oBufQueue;
	CSimpleLoopBuffer*		m_poRecvQueue;
};

#endif
