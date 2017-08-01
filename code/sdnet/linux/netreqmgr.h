#ifndef NETREQMGR_H
#define NETREQMGR_H

#include <list>
#include "netrequest.h"
#include "sddataqueue.h"
#include "simpleloopbuffer.h"

using namespace Crown;

typedef std::list<SNetRequest*> CListNetRequest;

class CNetReqMgr
{
	CNetReqMgr(void);
	virtual ~CNetReqMgr(void);
public:
	
	static bool CreateInstance()
	{
		if(NULL == m_poInstance)
		{
			m_poInstance = CnNew CNetReqMgr;
		}

		return m_poInstance != NULL;
	}
	static CNetReqMgr * Instance(){ return m_poInstance; }
	static void DestoryInstance()
	{
		if(m_poInstance != NULL)
		{
			CnDelete m_poInstance;
			m_poInstance = NULL;
		}
	}

	bool Init(int nSize, CSimpleLoopBuffer* poSendQueue);

	SNetRequest* GetFreeReq();

	//将空闲Req放回池
	inline void ReleaseReq(SNetRequest* pstReq)
	{
		//不可能出现失败情况，稳定后可将判断去掉
		if(!m_oFreeQueue.PushBack(pstReq))
		{
			CRITICAL("Release NetReq fail");
			CnAssert(false);
		}
	}
	//压入Req
	inline void PushBackNetReq(SNetRequest* pstReq)
	{
		//不可能出现失败情况，稳定后可将判断去掉
		if(!m_oReqQueue.PushBack(pstReq))
		{
			CRITICAL("PushBackNetReq fail! ReqQueue full!");
			CnAssert(false);
		}
	}
	
	//弹出Req
	bool PopFrontNetReq(SNetRequest* pstReq);
	//弹出到临时缓冲区，为了解决事件、请求队列可能引发的死锁情况
	void PopToBuffer();

protected:
	static CNetReqMgr*	m_poInstance;
	SNetRequest*		m_pReqArray;
	CSDDataQueue			m_oFreeQueue;
	CSDDataQueue			m_oReqQueue;
	CListNetRequest		m_oBufQueue;
	CSimpleLoopBuffer*		m_poSendQueue;
};

#endif
