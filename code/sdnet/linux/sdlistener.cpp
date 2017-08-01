#include "SDnetPCH.h"
#include "sdnetconfig.h"
#include "sdlistener.h"
#include "netreqmgr.h"
#include "sdnetlinux.h"

//DECLARE_LEAK_REF;

CSDListener::CSDListener(void)
{
	//INC_LEAK_REF;
	m_poPacketParser	= NULL;
	m_poSessionFactory	= NULL;
	m_dwRecvBufSize		= DEFAULT_SENDBUF_SIZE;
	m_dwSendBufSize		= DEFAULT_RECVBUF_SIZE;
	m_dwID				= 0;

	m_hListenSock		= -1;
	m_bStart			= false;
}

CSDListener::~CSDListener(void)
{
	//DEC_LEAK_REF;
}

void CSDListener::SetPacketParser(ISDPacketParser* poPacketParser)
{
	m_poPacketParser = poPacketParser;
}

void CSDListener::SetSessionFactory(ISDSessionFactory* poSessionFactory)
{
	m_poSessionFactory = poSessionFactory;
}

void CSDListener::SetBufferSize(uint32 dwRecvBufSize, uint32 dwSendBufSize)
{
	m_dwRecvBufSize		= dwRecvBufSize;
	m_dwSendBufSize		= dwSendBufSize;
}

void CSDListener::SetOpt(uint32 dwType, void* pOpt)
{
}

bool CSDListener::Start(const char* pszIP, uint16 wPort, bool bReUseAddr)
{
	if(m_bStart)
	{
		return true;
	}

	if( NULL == m_poSessionFactory || NULL == m_poPacketParser)
	{
		return false;
	}

	m_hListenSock = socket(AF_INET, SOCK_STREAM, 0);
	if(-1 == m_hListenSock)
	{
		CRITICAL("%s, create socket error, %s:%u, errno %d", __FUNCTION__, pszIP, wPort, errno);
		return false;
	}

	if(bReUseAddr)
	{
		int32 nReuse = 1;
		setsockopt(m_hListenSock, SOL_SOCKET, SO_REUSEADDR, (char*)&nReuse, sizeof(nReuse));
	}

	SNetRequest* pstReq;
	SStartListenReq* pstLsnReq;

	sockaddr_in stAddr = {0};
	stAddr.sin_family		= AF_INET;
	if(0 == strcmp(pszIP, "0"))
		stAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	else
		stAddr.sin_addr.s_addr = inet_addr(pszIP);
	stAddr.sin_port = htons(wPort);

    if(::bind(m_hListenSock, (sockaddr*)&stAddr, sizeof(stAddr)) < 0)
	{
		CRITICAL("%s, bind at %s:%d failed, errno %d, errmsg %s", __FUNCTION__, pszIP, wPort, errno, strerror(errno));
		goto fail_ret;
	}
	if(listen(m_hListenSock, 128) < 0)
	{
		CRITICAL("%s, listen at %s:%d failed, errno %d, errmsg %s", __FUNCTION__, pszIP, wPort, errno, strerror(errno));
		goto fail_ret;
	}

	pstReq = CNetReqMgr::Instance()->GetFreeReq();
	CnVerify(pstReq != NULL);

	static uint32	s_dwNextListenID = 0;
	m_dwID = ++s_dwNextListenID;

	pstReq->nType = NETREQ_START_LISTEN;
	pstLsnReq	= &pstReq->stUn.stStartListen;
	pstLsnReq->dwListenerID		= m_dwID;
	pstLsnReq->hListenSock		= m_hListenSock;
	pstLsnReq->poPacketParser	= m_poPacketParser;
	pstLsnReq->poSessionFactory	= m_poSessionFactory;
	pstLsnReq->dwRecvBufSize	= m_dwRecvBufSize;
	pstLsnReq->dwSendBufSize	= m_dwSendBufSize;

	CNetReqMgr::Instance()->PushBackNetReq(pstReq);

	m_bStart = true;

	INFO("%s, listen at %s:%u success", __FUNCTION__, pszIP, wPort);
	return true;

fail_ret:
	if(m_hListenSock != -1)
	{
		close(m_hListenSock);
		m_hListenSock = -1;
	}

	return false;
}

bool CSDListener::Stop(void)
{
	if(false == m_bStart)
		return true;

	SNetRequest* pstReq = CNetReqMgr::Instance()->GetFreeReq();
	CnVerify(pstReq != NULL);

	pstReq->nType = NETREQ_STOP_LISTEN;
	pstReq->stUn.stStopListen.dwListenerID = m_dwID;

	CNetReqMgr::Instance()->PushBackNetReq(pstReq);
	INFO("%s, stop listen ok", __FUNCTION__);

	m_bStart = false;
	return true;
}

void CSDListener::Release(void)
{
	CSDNetLinux::Instance()->ReleaseListener(this);
}
