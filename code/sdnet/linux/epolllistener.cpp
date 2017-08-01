#include "SDnetPCH.h"
#include "sdnetconfig.h"
#include "epolllistener.h"
#include "epollsockmgr.h"
#include "connectionmgr.h"
#include "epollctrl.h"
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

//DECLARE_LEAK_REF;

CEpollListener::CEpollListener(void)
{
	//INC_LEAK_REF;
	m_stEpollData.dwType	= EPOLL_LISTEN;
	m_stEpollData.ptr		= this;
	m_dwID					= 0;
	m_hListenSock			= -1;
	m_poEpollCtrl			= NULL;
	m_poEpollSockMgr		= NULL;
	m_poPacketParser		= NULL;
	m_poSessionFactory		= NULL;
	m_dwRecvBufSize			= DEFAULT_RECVBUF_SIZE;
	m_dwSendBufSize			= DEFAULT_SENDBUF_SIZE;
}

CEpollListener::~CEpollListener(void)
{
	//DEC_LEAK_REF;
	if(m_hListenSock != -1)
		close(m_hListenSock);
}

CEpollSock* CEpollListener::Accept()
{
	sockaddr_in stLocalAddr;
	sockaddr_in stRemoteAddr;
	uint32 dwSendBufSize;
	uint32 dwAddrLen = sizeof(stRemoteAddr);
	int32 hAcceptSock = accept(m_hListenSock, (sockaddr*)&stRemoteAddr, &dwAddrLen);
	if(-1 == hAcceptSock)
	{
		WARN("CEpollListener::Accept, Accept failed, errno %d, %s", errno, strerror(errno));
		return NULL;
	}

	CEpollSock* poSock = NULL;
	CSDConnection* poConnection = NULL;
	ISDSession* poSession = NULL;

	poSock = m_poEpollSockMgr->Create(m_dwRecvBufSize, m_dwSendBufSize);
	if(NULL == poSock)
	{
		CRITICAL("%s, accept failed, can not create CEpollSock", __FUNCTION__);
		goto fail_ret;
	}

	poConnection = CConnectionMgr::Instance()->Create();
	if(NULL == poConnection)
	{
		CRITICAL("%s, accept failed, can not create CSDConnection", __FUNCTION__);
		goto fail_ret;
	}

	dwSendBufSize = m_dwSendBufSize;
	if(dwSendBufSize > MAX_SYS_SEND_BUF)
	{
		dwSendBufSize = MAX_SYS_SEND_BUF;
		setsockopt(hAcceptSock, SOL_SOCKET, SO_SNDLOWAT, &VAL_SO_SNDLOWAT, sizeof(VAL_SO_SNDLOWAT));
	}

	setsockopt(hAcceptSock, SOL_SOCKET, SO_SNDBUF, (char*)&dwSendBufSize, sizeof(dwSendBufSize));
	if(false == m_poEpollCtrl->Add(hAcceptSock, EPOLLIN, poSock->GetEpollData()))
	{
		CRITICAL("%s, Epoll add failed", __FUNCTION__);
		goto fail_ret;
	}

	dwAddrLen = sizeof(stLocalAddr);
	getsockname(hAcceptSock, (sockaddr*)&stLocalAddr, &dwAddrLen);

	poConnection->SetLocalIP(stLocalAddr.sin_addr.s_addr);
	poConnection->SetLocalPort(ntohs(stLocalAddr.sin_port));
	poConnection->SetRemoteIP(stRemoteAddr.sin_addr.s_addr);
	poConnection->SetRemotePort(ntohs(stRemoteAddr.sin_port));

	poSession = m_poSessionFactory->CreateSession(poConnection);
	if(NULL == poSession)
	{
		m_poEpollCtrl->Del(hAcceptSock);
		CRITICAL("%s, accept failed, can not create ISDSession", __FUNCTION__);
		goto fail_ret;
	}
    
	poConnection->SetAccept(true);
	poConnection->SetParentID(m_dwID);
	poConnection->SetSession(poSession);
	poConnection->SetEpollSockID(poSock->GetID());

	poSock->SetAccepted(true);
	poSock->SetParentID(m_dwID);
	poSock->SetSock(hAcceptSock);
	poSock->SetConnectionID(poConnection->GetID());
	poSock->SetSession(poSession);
	poSock->SetPacketParser(m_poPacketParser);

	return poSock;

fail_ret:
	if(poConnection != NULL)
		CConnectionMgr::Instance()->Release(poConnection);
	
	if(poSock != NULL)
		m_poEpollSockMgr->Release(poSock);

	if(hAcceptSock != -1)
		close(hAcceptSock);

	return NULL;
}
