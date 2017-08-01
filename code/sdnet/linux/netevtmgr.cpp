#include "SDnetPCH.h"
#include "sdnetconfig.h"
#include "netevtmgr.h"
#include "netreqmgr.h"

CNetEvtMgr* CNetEvtMgr::m_poInstance = NULL;

CNetEvtMgr::CNetEvtMgr(void)
{
	m_pEvtArray		= NULL;
	m_poRecvQueue	= NULL;
}

CNetEvtMgr::~CNetEvtMgr(void)
{
	if(m_pEvtArray != NULL)
		CnDelete m_pEvtArray;
}

bool CNetEvtMgr::Init(int nSize, CSimpleLoopBuffer* poRecvQueue)
{
	m_pEvtArray = CnNew SNetEvent[nSize];
	if(NULL == m_pEvtArray)
		return false;

	if(false == m_oFreeQueue.Init(nSize+1))
		return false;

	if(false == m_oEvtQueue.Init(nSize+1))
		return false;

	for(int32 i = 0; i < nSize; i++)
	{
		m_oFreeQueue.PushBack(&m_pEvtArray[i]);
	}

	m_poRecvQueue = poRecvQueue;

	return true;
}

SNetEvent* CNetEvtMgr::GetFreeEvt()
{
	SNetEvent* pstEvent;
	while( (pstEvent = (SNetEvent*)m_oFreeQueue.PopFront()) == NULL)
	{
		WARN("No FreeEvt! usleep and try again");
		CNetReqMgr::Instance()->PopToBuffer();
		usleep(10);
	}

	return pstEvent;
}

//弹出Evt
bool CNetEvtMgr::PopFrontNetEvt(SNetEvent* pstEvent)
{
	SNetEvent* pstTmp;
	if(m_oBufQueue.size() != 0)
	{
		pstTmp = m_oBufQueue.front();
		m_oBufQueue.pop_front();
		*pstEvent = *pstTmp;
		CnDelete pstTmp;
		return true;
	}

	pstTmp = (SNetEvent*)m_oEvtQueue.PopFront();
	if(NULL == pstTmp)
		return false;

	*pstEvent = *pstTmp;
	ReleaseEvt(pstTmp);
	return true;
}

void CNetEvtMgr::PopToBuffer()
{
	for(int32 i = 0; i < POP_TO_BUF_COUNT; i++)
	{
		SNetEvent* pstTmp = (SNetEvent*)m_oEvtQueue.PopFront();
		if(NULL == pstTmp)
			return;

		SNetEvent* pstEvent = CnNew SNetEvent;
		if(NULL == pstEvent)
		{
			CRITICAL("CNetEvtMgr::PopToBuffer, new SNetEvent fail! exit program");
			if(NETEVT_RECV == pstTmp->nType)
			{
				m_poRecvQueue->DiscardFront(pstTmp->stUn.stRecv.nLen);
			}
			ReleaseEvt(pstTmp);
			return;
		}
		*pstEvent = *pstTmp;
		ReleaseEvt(pstTmp);

		if(NETEVT_RECV == pstEvent->nType)
		{
			pstEvent->stUn.stRecv.pData = CnNew char[pstEvent->stUn.stRecv.nLen];
			if(NULL == pstEvent->stUn.stRecv.pData)
			{
				CRITICAL("CNetEvtMgr::PopToBuffer, new char[%d] fail", pstEvent->stUn.stRecv.nLen);
				m_poRecvQueue->DiscardFront(pstEvent->stUn.stRecv.nLen);
				CnDelete pstEvent;
				continue;
			}
			if(false == m_poRecvQueue->PopFront(pstEvent->stUn.stRecv.pData, pstEvent->stUn.stRecv.nLen))
			{
				CRITICAL("CNetEvtMgr::PopToBuffer, PopFront RecvData Len %d failed", pstEvent->stUn.stRecv.nLen);
				CnDelete pstEvent->stUn.stRecv.pData;
				CnDelete pstEvent;
				continue;
			}
		}

		m_oBufQueue.push_back(pstEvent);
	}
}
