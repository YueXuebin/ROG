#include "SDnetPCH.h"
#include "sdnetconfig.h"
#include "eventmgr.h"
#include "iocpctrl.h"

IMPLEMENT_SINGLETON(CEventMgr)

CEventMgr::CEventMgr()
{
	//m_poRecvQueue	= NULL;
}

CEventMgr::~CEventMgr()
{
}

bool CEventMgr::Init(int32 nSize)
{
	if(false == m_oEvtQueue.Init(nSize+1))
	{
		return false;
	}

	m_threadBufferPool = CIocpCtrl::Instance()->GetThreadBufferPool();
	return true;
}

void CEventMgr::Uninit()
{
	// 释放掉没有处理的NetEvent
	SNetEvent *pstEvent = NULL;
	while (pstEvent = PopFrontNetEvt() )
	{
		ReleaseNetEvt(pstEvent);
	}
}

