#include "SDnetPCH.h"
#include "sdnetconfig.h"
#include "cplistener.h"
#include "iocpctrl.h"
#include "cpsockmgr.h"
#include "sdconnection.h"
#include "connectionmgr.h"
#include "eventmgr.h"
#include "sdlock.h"
#include "conndatamgr.h"

#define POST_ACCEPTEX_COUNT     128

CSDMutex CCpListener::m_soGetAddrMutex; // lcj
CCpListener::CCpListener()
{
	m_stPerHandleData.bListen   = true;
	m_stPerHandleData.ptr       = this;

	m_hListenSock               = INVALID_SOCKET;
	m_lpfnAcceptEx              = NULL;
	m_lpfnGetAcceptExSockaddrs  = NULL;

	m_dwID                      = 0;
	m_poPacketParser            = NULL;
	m_poSessionFactory          = NULL;
	m_dwRecvBufSize             = 0;
	m_dwSendBufSize             = 0;
    m_bStart                    = false;
    m_dwReleaseCount            = 0;    
    
}

CCpListener::~CCpListener()
{
	if(m_hListenSock != INVALID_SOCKET)
	{
		closesocket(m_hListenSock);
	}
	if(m_pPerIoDataArray != NULL)
	{
		CnDelete [] m_pPerIoDataArray;
	}
}

bool CCpListener::Start(const char* pszIP, uint16 wPort, bool bReUseAddr)
{
	//创建套接字
	m_hListenSock = socket(AF_INET, SOCK_STREAM, 0);
	if(-1 == m_hListenSock)
	{
		CRITICAL("%s, create socket error, %s:%u, errno %d", __FUNCTION__, pszIP, wPort, WSAGetLastError());
		return false;
	}

	//是否允许端口复用
	if(bReUseAddr)
	{
		int32 nReuse = 1;
		setsockopt(m_hListenSock, SOL_SOCKET, SO_REUSEADDR, (char*)&nReuse, sizeof(nReuse));
	}

	//绑定IP及端口
	sockaddr_in stAddr = {0};
	stAddr.sin_family		= AF_INET;
	if(0 == strcmp(pszIP, "0"))
	{
		stAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	}
	else
	{
		stAddr.sin_addr.s_addr = inet_addr(pszIP);
	}
	stAddr.sin_port = htons(wPort);

	if(bind(m_hListenSock, (sockaddr*)&stAddr, sizeof(stAddr)) < 0)
	{
		CRITICAL("%s, bind at %s:%d failed, errno %d", __FUNCTION__, pszIP, wPort, WSAGetLastError());
		return false;
	}

	//监听该套接字
	if(listen(m_hListenSock, 128) < 0)
	{
		CRITICAL("%s, listen at %s:%d failed, errno %d", __FUNCTION__, pszIP, wPort, WSAGetLastError());
		return false;
	}

	//将该套接字绑定到IO完成端口上
	if(false == CIocpCtrl::Instance()->AssociateWithIocp(m_hListenSock, &m_stPerHandleData))
	{
		return false;
	}

	if(false == _InitAcceptEx())
	{
		CRITICAL("CCpListener::Start, _InitAcceptEx failed");
		return false;
	}

    m_bStart    = true;
	return true;
}

bool CCpListener::_InitAcceptEx()
{
	DWORD dwbytes = 0;

	GUID m_GuidAcceptEx = WSAID_ACCEPTEX;

	int iRc = ::WSAIoctl(
		m_hListenSock,
		SIO_GET_EXTENSION_FUNCTION_POINTER,
		&m_GuidAcceptEx,
		sizeof(m_GuidAcceptEx),
		&m_lpfnAcceptEx,
		sizeof(LPFN_ACCEPTEX),
		&dwbytes,
		NULL,
		NULL);

	GUID m_GuidGetAcceptExSockaddrs = WSAID_GETACCEPTEXSOCKADDRS;

	dwbytes = 0;

	iRc = ::WSAIoctl(
		m_hListenSock,
		SIO_GET_EXTENSION_FUNCTION_POINTER,
		&m_GuidGetAcceptExSockaddrs,
		sizeof(m_GuidGetAcceptExSockaddrs),
		&m_lpfnGetAcceptExSockaddrs,
		sizeof(LPFN_GETACCEPTEXSOCKADDRS),
		&dwbytes,
		NULL,
		NULL);

	m_pPerIoDataArray = CnNew SPerIoData[POST_ACCEPTEX_COUNT];
	if(NULL == m_pPerIoDataArray)
	{
		return false;
	}

	for(int32 i = 0; i < POST_ACCEPTEX_COUNT; i++)
	{
		if(false == PostAcceptEx(&m_pPerIoDataArray[i]))
		{
			return false;
		}
	}

	return true;
}

bool CCpListener::PostAcceptEx(SPerIoData* pstPerIoData)
{

	SOCKET hNewSock = WSASocket(
						AF_INET,
						SOCK_STREAM,
						0,
						NULL,
						0,
						WSA_FLAG_OVERLAPPED);

	if(INVALID_SOCKET == hNewSock)
	{
		CRITICAL("CCpListener::PostAcceptEx, WSASocket failed, errno %d", ::WSAGetLastError());
		return false;
	}
	//SOCKET hNewSock = SysSockMgr::Instance()->Create(); 

	memset(&pstPerIoData->stOverlapped, 0, sizeof(pstPerIoData->stOverlapped));
	pstPerIoData->hSock		= hNewSock;
	pstPerIoData->nOp		= IOCP_ACCEPT;

	DWORD dwBytes;
	int bRet = m_lpfnAcceptEx(
		m_hListenSock,
		hNewSock,
		pstPerIoData->Buf,
		0,
		sizeof(SOCKADDR_IN) + 16,
		sizeof(SOCKADDR_IN) + 16,
		&dwBytes,
		&pstPerIoData->stOverlapped);

	if (S_OK != bRet)
	{
		if(WSA_IO_PENDING != ::WSAGetLastError())
		{
			CRITICAL("CCpListener::PostAcceptEx, AcceptEx failed, errno %d", WSAGetLastError());
			closesocket(hNewSock);
			return false;
		}
	}

	return true;
}

void CCpListener::Stop()
{
	if(INVALID_SOCKET == m_hListenSock)
	{
		return;
	}

    m_bStart = false;

	CancelIo((HANDLE)m_hListenSock);
	closesocket(m_hListenSock);
	m_hListenSock = INVALID_SOCKET;
	uint32 dwLastReleaseCount = 0;
    while (true)
    {
        //static uint32 dwLastReleaseCount = 0;//Modify by hexi. 2009-08-21
        if (dwLastReleaseCount == m_dwReleaseCount)
        {
            Sleep(500);
            if (dwLastReleaseCount == m_dwReleaseCount)
            {
                Sleep(500);
                break;
            }
        }
        dwLastReleaseCount = m_dwReleaseCount;
    }
}


void CCpListener::GetSockAddress(SPerIoData * pstPerIoData, sockaddr_in & RemoteAddr, sockaddr_in & LocalAddr)
{
	sockaddr_in* pstRemoteAddr	= NULL;
	sockaddr_in* pstLocalAddr	= NULL;

	int32 nRemoteAddrLen		= sizeof(sockaddr_in);
	int32 nLocalAddrLen			= sizeof(sockaddr_in);
	int32 nAddrLen				= sizeof(sockaddr_in) + 16;

#ifndef  HAS_WINDOWS2000_SERVER

	{
		CSDMutexLock lock(m_soGetAddrMutex); 

		m_lpfnGetAcceptExSockaddrs(pstPerIoData->Buf,
			0,
			nAddrLen,
			nAddrLen,
			(SOCKADDR**)&pstLocalAddr,
			&nLocalAddrLen,
			(SOCKADDR**)&pstRemoteAddr,
			&nRemoteAddrLen);
	}

	RemoteAddr = *pstRemoteAddr;
	LocalAddr = *pstLocalAddr;
#else

	if (0 != getpeername( pstPerIoData->hSock, (struct sockaddr*)&RemoteAddr, &nRemoteAddrLen))
		if (0 != getpeername( pstPerIoData->hSock, (struct sockaddr*)&RemoteAddr, &nRemoteAddrLen))
		{
			DBG("getpeername failed, errno=%d", WSAGetLastError());
		}
		DBG("getpeername succ, sockfd=%u", (uint32) pstPerIoData->hSock);

		DBG("RemoteIP:%u, RemotePort:%d",  RemoteAddr.sin_addr.s_addr, ntohs(RemoteAddr.sin_port));

		if (0 != getsockname( pstPerIoData->hSock, (struct sockaddr*)&LocalAddr, &nLocalAddrLen))
		{
			DBG("getsockname failed, errno=%d", WSAGetLastError());
		}
		DBG("getsockname succ, sockfd=%u", (uint32) pstPerIoData->hSock);

		pstRemoteAddr = &RemoteAddr;
		pstLocalAddr = &LocalAddr;

#endif 
}

void CCpListener::OnAccept(bool bSucc, SPerIoData* pstPerIoData)
{
    SOCKET hSock = pstPerIoData->hSock;

    if(false == m_bStart)
    {
		InterlockedIncrement((volatile long*)&m_dwReleaseCount);
        closesocket(hSock);
        return;
    }

	this->PostAcceptEx(pstPerIoData);
	
    if(false == bSucc)
    {
		WARN("CCpListener::OnAccept, accept failed, errno %d", ::WSAGetLastError());
        closesocket(hSock);
    }
    else
    {
		CConnData* pConnData = CConnDataMgr::Instance()->Alloc(m_dwRecvBufSize, m_dwSendBufSize);
		CCPSock * poSock = &pConnData->sock;		
		CSDConnection* poConnection = & pConnData->connection;

//+lcj
        if (0 != ::setsockopt(hSock, SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT, (char *)&m_hListenSock, sizeof(SOCKET)))
        {
			WARN("setsockopt for new socket on UpdateConetext failed, errno=%d", ::WSAGetLastError());
        }

        poSock->SetSock(hSock);
        poSock->SetPacketParser(m_poPacketParser);

        poConnection->SetAccept(true);
        poConnection->SetParentID(0);

		sockaddr_in RemoteAddr, LocalAddr;

        GetSockAddress(pstPerIoData, RemoteAddr, LocalAddr);


        poConnection->SetLocalIP(LocalAddr.sin_addr.s_addr);
        poConnection->SetLocalPort(ntohs(LocalAddr.sin_port));

        poConnection->SetRemoteIP(RemoteAddr.sin_addr.s_addr);
        poConnection->SetRemotePort(ntohs(RemoteAddr.sin_port));

        //DBG("AfterSet RemoteIP:%s, RemotePort:%d",  inet_ntoa(RemoteAddr.sin_addr), ntohs(RemoteAddr.sin_port));
        ISDSession* poSession = m_poSessionFactory->CreateSession(poConnection);
        if(NULL == poSession)
        {
            DBG("CCpListener::OnAccept, CreateSession failed");
            closesocket(hSock);
	        return;
        }
        poConnection->SetSession(poSession);
        poSock->SetConnect(true);
#ifndef SDENT_HAS_RECV_QUEUE
		poConnection->OnAssociate();
#endif
        //
        // 应该先投递连接事件再关联套接口，否则可能出现第一个Recv事件先于连接事件入队列
        //
        //CEventMgr::Instance()->PushEstablishEvt(poConnection, true, m_dwID);

        if(false == poSock->AssociateWithIocp())
        {
            poSock->Close();
        }
        else
        {
            if(false == poSock->PostRecv())
            {
                poSock->Close();
            }
        }
    }


}
