#include "SDnetPCH.h"
#include "sdnetconfig.h"
#include "sdconnection.h"
#include "sdconnector.h"
#include "netrequest.h"
#include "netreqmgr.h"
#include "netevtmgr.h"
#include "sdnetlinux.h"
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
using namespace Crown;

CSimpleLoopBuffer* CSDConnection::m_poSendQueue = NULL;
CEpollSockMgr* CSDConnection::m_poEpollSockMgr = NULL;
CSDConnection::CSDConnection(void)
{
	Reset();
}

CSDConnection::~CSDConnection(void)
{
}

void CSDConnection::Reset()
{
	m_bConnected	= false;
	m_dwLocalIP		= 0;
	m_wLocalPort	= 0;
	m_dwRemoteIP	= 0;
	m_wRemotePort	= 0;
	m_szLocalIP[0]	= '\0';
	m_szRemoteIP[0]	= '\0';

	m_dwID			= 0;
	m_bAccept		= false;
	m_dwParentID	= 0;
	m_dwEpollSockID	= 0;
	
	m_poSession		= NULL;
}

void CSDConnection::SetRemoteIP(uint32 dwIP)
{
	m_dwRemoteIP = dwIP;
	strcpy(m_szRemoteIP, inet_ntoa( (in_addr&)m_dwRemoteIP));
}

void CSDConnection::SetLocalIP(uint32 dwIP)
{
	m_dwLocalIP = dwIP;
	strcpy(m_szLocalIP, inet_ntoa( (in_addr&)m_dwLocalIP));
}

bool CSDConnection::IsConnected(void)
{
	return m_bConnected;
}

void CSDConnection::Send(const char* pBuf,uint32 dwLen)
{
	if(false == m_bConnected)
		return;
	if(0 == dwLen || dwLen > MAX_PKG_LEN)
		return;

	SNetRequest* pstReq = CNetReqMgr::Instance()->GetFreeReq();
	CnVerify(pstReq != NULL);
	
	while(!m_poSendQueue->PushBack(pBuf, dwLen))
	{
		WARN("CSDConnection::Send, PushBack SendQueue failed! usleep and try again");
		CNetEvtMgr::Instance()->PopToBuffer();
		usleep(10);
	}

	pstReq->nType = NETREQ_SEND;
	pstReq->stUn.stSend.dwEpollSockID = m_dwEpollSockID;
	pstReq->stUn.stSend.nLen = dwLen;
	pstReq->stUn.stSend.pData = NULL;

	CNetReqMgr::Instance()->PushBackNetReq(pstReq);
}

void CSDConnection::SetOpt(uint32 dwType, void* pOpt)
{
	if(NULL == pOpt)
		return;
	if(false == m_bConnected)
		return;

	switch(dwType)
	{
	case CONNECTION_OPT_SOCKOPT:
		{
			SConnectionOptSockopt* pstOpt = (SConnectionOptSockopt*)pOpt;
			if(NULL == pstOpt->pOptVal || pstOpt->nOptLen > 8)
				return;

			SNetRequest* pstReq = CNetReqMgr::Instance()->GetFreeReq();
			CnVerify(pstReq != NULL);

			pstReq->nType = NETREQ_SOCKOPT;
			SSockOptReq* pstSockOpt = &pstReq->stUn.stSockOpt;
			pstSockOpt->dwEpollSockID	= m_dwEpollSockID;
			pstSockOpt->nLevel			= pstOpt->nLevel;
			pstSockOpt->nOptName		= pstOpt->nOptName;
			memcpy(pstSockOpt->OptVal, pstOpt->pOptVal, pstOpt->nOptLen);
			pstSockOpt->nOptLen			= pstOpt->nOptLen;

			CNetReqMgr::Instance()->PushBackNetReq(pstReq);
		}
		break;
	default:
		break;
	}
}

void CSDConnection::Disconnect(void)
{
	if(false == m_bConnected)
		return;

	m_bConnected = false;

	SNetRequest* pstReq = CNetReqMgr::Instance()->GetFreeReq();
	CnVerify(pstReq != NULL);

	pstReq->nType = NETREQ_DISCONN;
	pstReq->stUn.stDisConn.dwEpollSockID = m_dwEpollSockID;

	CNetReqMgr::Instance()->PushBackNetReq(pstReq);
}

void CSDConnection::OnConnect()
{
	m_bConnected = true;

	//如果是主动连接对方，那么要通知Connector连接事件
	if(false == m_bAccept)
	{
		CSDConnector* poConnector = CSDNetLinux::Instance()->FindConnector(m_dwParentID);
		if(NULL == poConnector)
			return;
		poConnector->OnConnect();
	}

	CnVerify(m_poSession != NULL);
	m_poSession->SetConnection(this);
	m_poSession->OnEstablish();
}

void CSDConnection::OnSend(const char * pBuf, uint32 dwLen)
{
#if SDNET_HAS_SEND_REPORT   
	if (m_poSession != NULL )
	{
		m_poSession->OnSend(pBuf,dwLen); 
	}
#endif 
}

void CSDConnection::OnClose()
{
	m_bConnected = false;

	//如果是主动连接对方，要通知Connector断线事件
	if(false == m_bAccept)
	{
		CSDConnector* poConnector = CSDNetLinux::Instance()->FindConnector(m_dwParentID);
		if(NULL == poConnector)
			return;
		poConnector->OnClose();
	}

	CnVerify(m_poSession != NULL);
	m_poSession->OnTerminate();
	m_poSession->Release();
	m_poSession = NULL;
}

uint32 SDAPI CSDConnection::GetSendBufFree( void )
{
	if (NULL == m_poEpollSockMgr)
	{
		return UNKNOWN_SIZE;
	}
	CEpollSock* pSock = m_poEpollSockMgr->Find(m_dwEpollSockID);
	if (pSock != NULL)
	{
		return pSock->GetSendBufFree();
	}
	return UNKNOWN_SIZE;
}
