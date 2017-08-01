#include "SDnetPCH.h"
#include "sdnetconfig.h"
#include "netreqmgr.h"
#include "netevtmgr.h"

CNetReqMgr * CNetReqMgr::m_poInstance = NULL;

CNetReqMgr::CNetReqMgr(void)
{
	m_pReqArray		= NULL;
	m_poSendQueue	= NULL;
}

CNetReqMgr::~CNetReqMgr(void)
{
	if(m_pReqArray != NULL)
		CnDelete m_pReqArray;
}

bool CNetReqMgr::Init(int nSize, CSimpleLoopBuffer* poSendQueue)
{
	m_pReqArray = CnNew SNetRequest[nSize];
	if(NULL == m_pReqArray)
		return false;

	if(false == m_oFreeQueue.Init(nSize+1))
		return false;

	if(false == m_oReqQueue.Init(nSize+1))
		return false;

	for(int32 i = 0; i < nSize; i++)
	{
		m_oFreeQueue.PushBack(&m_pReqArray[i]);
	}

	m_poSendQueue = poSendQueue;
	return true;
}

//获取空闲Req
SNetRequest* CNetReqMgr::GetFreeReq()
{
	SNetRequest* pstReq;
	while( (pstReq = (SNetRequest*)m_oFreeQueue.PopFront()) == NULL)
	{
		WARN("No free NetReq! usleep and try again");
		CNetEvtMgr::Instance()->PopToBuffer();
		usleep(10);
	}
	return pstReq;
}

//弹出Req
bool CNetReqMgr::PopFrontNetReq(SNetRequest* pstReq)
{
	SNetRequest* pstTmp;

	if(m_oBufQueue.size() != 0)
	{
		pstTmp = m_oBufQueue.front();
		m_oBufQueue.pop_front();
		*pstReq = *pstTmp;
		CnDelete pstTmp;
		return true;
	}

	pstTmp = (SNetRequest*)m_oReqQueue.PopFront();
	if(NULL == pstTmp)
		return false;

	*pstReq = *pstTmp;
	ReleaseReq(pstTmp);
	return true;
}

void CNetReqMgr::PopToBuffer()
{
	for(int32 i = 0; i < POP_TO_BUF_COUNT; i++)
	{
		SNetRequest* pstTmp = (SNetRequest*)m_oReqQueue.PopFront();
		if(NULL == pstTmp)
			return;

		SNetRequest* pstRequest = CnNew SNetRequest;
		if(NULL == pstRequest)
		{
			CRITICAL("CNetReqMgr::PopToBuffer, new SNetRequest fail! exit program");
			if(NETREQ_SEND == pstTmp->nType)
			{
				m_poSendQueue->DiscardFront(pstTmp->stUn.stSend.nLen);
			}
			ReleaseReq(pstTmp);
			return;
		}
		*pstRequest = *pstTmp;
		ReleaseReq(pstTmp);

		if(NETREQ_SEND == pstRequest->nType)
		{
			pstRequest->stUn.stSend.pData = CnNew char[pstRequest->stUn.stSend.nLen];
			if(NULL == pstRequest->stUn.stSend.pData)
			{
				CRITICAL("CNetReqMgr::PopToBuffer, new char[%d] fail", pstRequest->stUn.stSend.nLen);
				m_poSendQueue->DiscardFront(pstRequest->stUn.stSend.nLen);
				CnDelete pstRequest;
				continue;
			}
			if(false == m_poSendQueue->PopFront(pstRequest->stUn.stSend.pData, pstRequest->stUn.stSend.nLen))
			{
				CRITICAL("CNetReqMgr::PopToBuffer, PopFront SendData Len %d failed", pstRequest->stUn.stSend.nLen);
				CnDelete pstRequest->stUn.stSend.pData;
				CnDelete pstRequest;
				continue;
			}
		}

		m_oBufQueue.push_back(pstRequest);
	}
}
