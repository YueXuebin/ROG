#include "SDnetPCH.h"
#include "sdnetconfig.h"
#include "epollthrd.h"
#include "netreqmgr.h"
#include "netevtmgr.h"
#include "epollsockmgr.h"
#include "sdconnection.h"
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
using namespace	Crown;

CEpollThrd::CEpollThrd(void)
{
	m_bTerminate	= false;
	m_poRecvQueue	= NULL;
	m_poSendQueue	= NULL;
	m_pSendBuf		= NULL;
}

CEpollThrd::~CEpollThrd(void)
{
}

void CEpollThrd::ThrdProc()
{
	_MainLoop();

	_Uninit();
}

void CEpollThrd::Terminate()
{
	m_bTerminate = true;
}

bool CEpollThrd::Start(CSimpleLoopBuffer* poRecvQueue, CSimpleLoopBuffer* poSendQueue)
{
	CnVerify(poRecvQueue != NULL && poSendQueue != NULL);
	m_poRecvQueue = poRecvQueue;
	m_poSendQueue = poSendQueue;

	if(false == _Init())
		return false;

	if(false == m_oThrdCtrl.Start(RunThreadFunc,this))
	{
		CRITICAL("%s, SGSYS_CreateThreadCtrl failed", __FUNCTION__);
		return false;
	}

	return true;
}

SDTHREAD_DECLARE(CEpollThrd::RunThreadFunc) (void * arg)
{
	CEpollThrd* pEpollThrd = (CEpollThrd*)arg ;
	if(pEpollThrd)
	{
		pEpollThrd->ThrdProc() ;
	}
	return pEpollThrd;
}


void CEpollThrd::Stop()
{
	m_oThrdCtrl.Terminate();
	m_oThrdCtrl.Wait();
}

bool CEpollThrd::_Init()
{
	if(false == m_oEpollSockMgr.Init(MAX_CONNECTION))
	{
		CRITICAL("CEpollThrd::_Init, EpollSockMgr Init failed");
		return false;
	}
	//// 2009-03-25 cwy add for interface expanding
	_SetEpollSockMgrToConnection();
	if(false == m_oEpollCtrl.Init(MAX_CONNECTION))
	{
		CRITICAL("%s, EpollCtrl Init failed, size=%d", __FUNCTION__, MAX_CONNECTION);
		return false;
	}

	m_pSendBuf = CnNew char[MAX_PKG_LEN];
	if(NULL == m_pSendBuf)
	{
		CRITICAL("%s, alloc SendBuf failed", __FUNCTION__);
		return false;
	}

	CEpollSock::SetRecvQueue(m_poRecvQueue);
	CEpollSock::SetEpollCtrl(&m_oEpollCtrl);

	return true;
}

void CEpollThrd::_Uninit()
{
	if(m_pSendBuf != NULL)
	{
		CnDelete m_pSendBuf;
		m_pSendBuf = NULL;
	}
	m_oEpollSockMgr.Uninit();
	m_oEpollCtrl.Uninit();
}

void CEpollThrd::_MainLoop()
{
	while(!m_bTerminate)
	{
		if(false == _ProcEpoll())
			break;

		_ProcRequest();
	}
}

bool CEpollThrd::_ProcEpoll()
{
	int32 nCount = m_oEpollCtrl.Wait(1);
	if(nCount < 0)
	{
		CRITICAL("%s, EpollWait failed", __FUNCTION__);
		return false;
	}

	for(int32 i = 0; i < nCount; i++)
	{
		epoll_event* pstEpollEvt = m_oEpollCtrl.GetEvent(i);
		CnVerify(pstEpollEvt != NULL);
		SEpollData* pstEpollData = (SEpollData*)pstEpollEvt->data.ptr;
		CnVerify(pstEpollData != NULL);
		CEpollSock* poSock = NULL;

		if(EPOLL_SOCK == pstEpollData->dwType)
		{
			CEpollSock* poSock = (CEpollSock*)pstEpollData->ptr;
			CnVerify(poSock != NULL);

			if(pstEpollEvt->events & EPOLLOUT)
			{
				if(poSock->IsConnected())
				{
					if(false == poSock->OnSend())
					{
						poSock->Close();
						m_oEpollSockMgr.Release(poSock);
						continue;
					}
				}
				else
				{
					if(false == poSock->OnInitiativeConn())
					{
						poSock->Close();
						m_oEpollSockMgr.Release(poSock);
						continue;
					}
				}
			}

			if(pstEpollEvt->events & EPOLLIN)
			{
				if(false == poSock->OnRecv())
				{
					poSock->Close();
					m_oEpollSockMgr.Release(poSock);
					continue;
				}
			}

			if(pstEpollEvt->events & EPOLLERR)
			{
				INFO("%s, epoll error, errno %d, %s", __FUNCTION__, errno, strerror(errno));
				poSock->OnError(NET_SYSTEM_ERROR, errno);
				poSock->Close();
				m_oEpollSockMgr.Release(poSock);
				continue;
			}
		}
		else if(EPOLL_LISTEN == pstEpollData->dwType)
		{
			CEpollListener* poListener = (CEpollListener*)pstEpollData->ptr;
			CnVerify(poListener != NULL);

			if(pstEpollEvt->events & EPOLLERR)
			{
				CRITICAL("%s, epoll listen sock failed", __FUNCTION__);
				m_oEpollCtrl.Del(poListener->GetListenSock());
				m_oListenerMgr.Release(poListener);
				continue;
			}

			if(pstEpollEvt->events & EPOLLIN)
			{
				poSock = poListener->Accept();
				if(NULL == poSock)
				{
					WARN("%s, accept failed", __FUNCTION__);
				}
				else
				{
					poSock->OnPassiveConn();
				}
			}
		}
	}

	return true;
}

void CEpollThrd::_ProcRequest()
{
	SNetRequest stReq;
	while( CNetReqMgr::Instance()->PopFrontNetReq(&stReq))
	{
		switch(stReq.nType)
		{
		case NETREQ_SEND:
			_ProcSendReq(&stReq.stUn.stSend);
			break;
		case NETREQ_DISCONN:
			_ProcDisconnReq(&stReq.stUn.stDisConn);
			break;
		case NETREQ_CONNECT:
			_ProcConnReq(&stReq.stUn.stConn);
			break;
		case NETREQ_SOCKOPT:
			_ProcSockOptReq(&stReq.stUn.stSockOpt);
			break;
		case NETREQ_START_LISTEN:
			_ProcStartListenReq(&stReq.stUn.stStartListen);
			break;
		case NETREQ_STOP_LISTEN:
			_ProcStopListenReq(&stReq.stUn.stStopListen);
			break;
		default:
			CnVerify(false);
			break;
		}
	}
}

void CEpollThrd::_ProcStartListenReq(SStartListenReq* pstReq)
{
	CnVerify(pstReq->dwListenerID != 0 && pstReq->hListenSock != -1);
	CnVerify(pstReq->poPacketParser != NULL && pstReq->poSessionFactory != NULL);
	CnVerify(pstReq->dwRecvBufSize != 0 && pstReq->dwSendBufSize != 0);

	CEpollListener* poListener = m_oListenerMgr.Create(pstReq->dwListenerID);
	if(NULL == poListener)
	{
		CRITICAL("%s, Create CEpollListener failed", __FUNCTION__);
		return;
	}

	poListener->SetListenSock(pstReq->hListenSock);
	poListener->SetBufferSize(pstReq->dwRecvBufSize, pstReq->dwSendBufSize);
	poListener->SetSessionFactory(pstReq->poSessionFactory);
	poListener->SetPacketParser(pstReq->poPacketParser);
	poListener->SetEpollCtrl(&m_oEpollCtrl);
	poListener->SetEpollSockMgr(&m_oEpollSockMgr);
	
	if(false == m_oEpollCtrl.Add(poListener->GetListenSock(), EPOLLIN, poListener->GetEpollData()))
	{
		CRITICAL("%s, Epoll Add failed", __FUNCTION__);
		return;
	}

	DBG("%s, ok!, hSock %d, id %u", __FUNCTION__, pstReq->hListenSock, pstReq->dwListenerID);
}

void CEpollThrd::_ProcStopListenReq(SStopListenReq* pstReq)
{
	CEpollListener* poListener = m_oListenerMgr.Find(pstReq->dwListenerID);
	if(NULL == poListener)
	{
		CRITICAL("%s, can not get listener, id %u", __FUNCTION__, pstReq->dwListenerID);
		return;
	}

	m_oEpollCtrl.Del(poListener->GetListenSock());
	m_oListenerMgr.Release(poListener);
}

void CEpollThrd::_ProcConnReq(SConnReq* pstReq)
{
	CEpollSock* poSock	= m_oEpollSockMgr.Create(pstReq->dwRecvBufSize, pstReq->dwSendBufSize);
	if(NULL == poSock)
	{
		CRITICAL("%s, create CEpollSock failed! dwRecvBufSize %u", __FUNCTION__, pstReq->dwRecvBufSize);
		CNetEvtMgr::Instance()->PushConnErrEvt(0, pstReq->dwConnectorID);
		return;
	}

	poSock->SetAccepted(false);
	poSock->SetParentID(pstReq->dwConnectorID);
	poSock->SetPacketParser(pstReq->poPacketParser);
	poSock->SetSession(pstReq->poSession);
	if(false == poSock->Connect(pstReq->dwIP, pstReq->wPort))
	{
		m_oEpollSockMgr.Release(poSock);
		return;
	}
}

void CEpollThrd::_ProcDisconnReq(SDisconnReq* pstReq)
{
	CEpollSock* poSock = m_oEpollSockMgr.Find(pstReq->dwEpollSockID);
	if(NULL == poSock)
	{
		INFO("%s, can not find CEpollSock, id %u", __FUNCTION__, pstReq->dwEpollSockID);
		return;
	}
	poSock->Close();
	m_oEpollSockMgr.Release(poSock);
}

void CEpollThrd::_ProcSendReq(SSendReq* pstReq)
{
	//即使没找到Sock也要将发送缓冲Pop出来，发送缓冲和ReqQueue中的Send请求是一一匹配的
	CnVerify(pstReq->nLen <= MAX_PKG_LEN);
	if(pstReq->pData != NULL)
	{
		memcpy(m_pSendBuf, pstReq->pData, pstReq->nLen);
		CnDelete pstReq->pData;
	}
	else
	{
		if(false == m_poSendQueue->PopFront(m_pSendBuf, pstReq->nLen))
		{
			CnVerify(false);
			CRITICAL("%s, can not pop data", __FUNCTION__);
			return;
		}
	}

	CEpollSock* poSock = m_oEpollSockMgr.Find(pstReq->dwEpollSockID);
	if(NULL == poSock)
	{
		INFO("%s, can not find CEpollSock, id %u", __FUNCTION__, pstReq->dwEpollSockID);
		return;
	}
	
	if(false == poSock->Send(m_pSendBuf, pstReq->nLen))
	{
		poSock->Close();
		m_oEpollSockMgr.Release(poSock);
	}
}

void CEpollThrd::_ProcSockOptReq(SSockOptReq* pstReq)
{
	CEpollSock* poSock = m_oEpollSockMgr.Find(pstReq->dwEpollSockID);
	if(NULL == poSock)
	{
		INFO("CEpollThrd::_ProcSockOptReq, can not find CEpollSock, id %u", pstReq->dwEpollSockID);
		return;
	}

	setsockopt(poSock->GetSock(), pstReq->nLevel, pstReq->nOptName, pstReq->OptVal, pstReq->nOptLen);
}

void CEpollThrd::_SetEpollSockMgrToConnection()
{
	CSDConnection::SetEpollSockMgr(&m_oEpollSockMgr);
}
