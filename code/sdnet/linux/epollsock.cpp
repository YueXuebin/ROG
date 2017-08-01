#include "SDnetPCH.h"
#include "sdnetconfig.h"
#include "epollsock.h"
#include "epollctrl.h"
#include "netevtmgr.h"
#include "netreqmgr.h"
#include "sdconnection.h"
#include "connectionmgr.h"
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

CEpollCtrl*	CEpollSock::m_poEpollCtrl = NULL;
CSimpleLoopBuffer* CEpollSock::m_poRecvQueue = NULL;

CEpollSock::CEpollSock(void)
{
	m_stEpollData.dwType	= EPOLL_SOCK;
	m_stEpollData.ptr		= this;

	Reset();
}

CEpollSock::~CEpollSock(void)
{
	if(m_pRecvBuf != NULL)
		CnDelete m_pRecvBuf;
	if(m_hSock != -1)
		close(m_hSock);
}

void CEpollSock::Reset()
{
	m_dwID				= 0;
	m_dwConnectionID	= 0;
	m_hSock				= -1;
	m_dwParentID		= 0;
	m_bAccepted			= false;

	m_pRecvBuf			= NULL;
	m_pSendBuf			= NULL;
	m_dwRecvBufSize		= 0;
	m_dwSendBufSize		= 0;
	m_nDataReceived		= 0;
	m_nDataSend			= 0;

	m_bConnected		= false;

	m_poPacketParser	= NULL;
	m_poSession			= NULL;

	m_dwLocalIP			= 0;
	m_wLocalPort		= 0;
	m_dwRemoteIP		= 0;
	m_wRemotePort		= 0;

	m_bNeedBind = false;
	m_pszBindIP = NULL;
	m_wBindPort = 0;
}

void CEpollSock::AttachRecvBuf(char* pRecvBuf, uint32 dwRecvBufSize)
{
	m_pRecvBuf		= pRecvBuf;
	m_dwRecvBufSize	= dwRecvBufSize;
	m_nDataReceived	= 0;
}

uint32 CEpollSock::DetachRecvBuf(char** ppRecvBuf)
{
	*ppRecvBuf = m_pRecvBuf;
	uint32 dwRet = m_dwRecvBufSize;
	
	m_dwRecvBufSize = 0;
	m_nDataReceived	= 0;
	m_pRecvBuf		= NULL;
	
	return dwRet;
}

void CEpollSock::AttachSendBuf(char* pSendBuf, uint32 dwSendBufSize)
{
	m_pSendBuf		= pSendBuf;
	m_dwSendBufSize = dwSendBufSize;
	m_nDataSend		= 0;
}

uint32 CEpollSock::DetachSendBuf(char** ppSendBuf)
{
	*ppSendBuf		= m_pSendBuf;
	uint32 dwRet	= m_dwSendBufSize;
	
	m_dwSendBufSize = 0;
	m_nDataSend		= 0;
	m_pSendBuf		= NULL;

	return dwRet;
}

bool CEpollSock::Connect(uint32 dwIP, uint16 wPort)
{
	sockaddr_in stAddr	= {0};
	int32 nSysError = 0;
	uint32 dwSendBufSize;

	m_bConnected = false;

	CnVerify(-1 == m_hSock && false == m_bAccepted);
	m_hSock = socket(AF_INET, SOCK_STREAM, 0);
	if(-1 == m_hSock)
	{
		CRITICAL("%s, create socket failed, errno %d, %s", __FUNCTION__, errno, strerror(errno));
		nSysError = errno;
		goto fail_ret;
	}

	dwSendBufSize = m_dwSendBufSize;
	if(dwSendBufSize > MAX_SYS_SEND_BUF)
	{
		dwSendBufSize = MAX_SYS_SEND_BUF;
		setsockopt(m_hSock, SOL_SOCKET, SO_SNDLOWAT, &VAL_SO_SNDLOWAT, sizeof(VAL_SO_SNDLOWAT));
	}

	setsockopt(m_hSock, SOL_SOCKET, SO_SNDBUF, &dwSendBufSize, sizeof(dwSendBufSize));

	if (true == m_bNeedBind)
	{
		sockaddr_in stAddr = {0};
		stAddr.sin_family = AF_INET;
		stAddr.sin_addr.s_addr = inet_addr(m_pszBindIP);
		stAddr.sin_port = htons(m_wBindPort);

		if (0 > ::bind(m_hSock, (sockaddr*)&stAddr, sizeof(stAddr)))
		{
			CRITICAL("%s, bind failed, errno %d, %s", __FUNCTION__, errno, strerror(errno));
			nSysError = errno;
			CNetEvtMgr::Instance()->PushBindErrEvt(nSysError, m_dwParentID);
			Close();
			return false;
		}
	}

	if(false == m_poEpollCtrl->Add(m_hSock, EPOLLOUT|EPOLLIN, &m_stEpollData))
	{
		CRITICAL("%s, epoll add failed, errno %d, %s", __FUNCTION__, errno, strerror(errno));
		nSysError = errno;
		goto fail_ret;
	}

	stAddr.sin_family = AF_INET;
	stAddr.sin_addr.s_addr = dwIP;
	stAddr.sin_port = htons(wPort);
	if(-1 == connect(m_hSock, (sockaddr*)&stAddr, sizeof(stAddr)))
	{
		if(errno != EINPROGRESS)
		{
			CRITICAL("%s, connect failed, errno %d, %s", __FUNCTION__, errno, strerror(errno));
			nSysError = errno;
			goto fail_ret;
		}
	}

	m_dwRemoteIP	= dwIP;
	m_wRemotePort	= wPort;

	return true;

fail_ret:
	CNetEvtMgr::Instance()->PushConnErrEvt(nSysError, m_dwParentID);
	Close();
	return false;
}

bool CEpollSock::_AddToSendBuf(const char* pData, int32 nLen)
{
	//发送缓冲中还有数据没被发送，将数据追加到缓冲区尾部
	if( (uint32)(nLen + m_nDataSend) > m_dwSendBufSize)
	{
		return false;
	}

	CnVerify(m_pSendBuf != NULL);
	memcpy(m_pSendBuf+m_nDataSend, pData, nLen);
	m_nDataSend += nLen;
	return true;
}

bool CEpollSock::Send(const char* pData, int32 nLen)
{
	if(m_nDataSend > 0)
	{
		if(false == _AddToSendBuf(pData, nLen))
		{
			DBG("CEpollSock::Send, send overflow 1");
			OnError(NET_SEND_OVERFLOW, 0);
			return false;
		}
		return true;
	}

	int32 nRet = send(m_hSock, pData, nLen, 0);
	if(nRet < 0)
	{
		if( (EAGAIN == errno) && (m_pSendBuf != NULL) )
		{
			if(false == _AddToSendBuf(pData, nLen))
			{
				DBG("CEpollSock::Send, send overflow 2");
				OnError(NET_SEND_OVERFLOW, 0);
				return false;
			}
			if(false == m_poEpollCtrl->Mod(m_hSock, EPOLLOUT|EPOLLIN, &m_stEpollData))
			{
				CRITICAL("CEpollSock::Send 1: epoll mod failed, errno %d, %s", errno, strerror(errno));
				OnError(NET_SEND_ERROR, errno);
				return false;
			}
			return true;
		}

		DBG("%s, send failed, errno %d, %s", __FUNCTION__, errno, strerror(errno));
		OnError(NET_SEND_ERROR, errno);
		return false;
	}
	else if(nRet != nLen)
	{
		if(NULL == m_pSendBuf)
		{
			//没有额外的SendBuf
			DBG("CEpollSock::Send, send overflow 3");
			OnError(NET_SEND_OVERFLOW, 0);
			return false;
		}
		
		//有额外的SendBuf，将没有发送出去的数据放到发送缓冲中
		nLen -= nRet;
		CnVerify(nLen > 0);
		CnVerify(0 == m_nDataSend);

		if(false == _AddToSendBuf(pData+nRet, nLen))
		{
			DBG("CEpollSock::Send, send overflow 4");
			OnError(NET_SEND_OVERFLOW, 0);
			return false;
		}
		if(false == m_poEpollCtrl->Mod(m_hSock, EPOLLOUT|EPOLLIN, &m_stEpollData))
		{
			CRITICAL("CEpollSock::Send 2: epoll mod failed, errno %d, %s", errno, strerror(errno));
			OnError(NET_SEND_ERROR, errno);
			return false;
		}

		CNetEvtMgr::Instance()->PushSendEvt(m_dwConnectionID,  m_pSendBuf, nLen);
		return true;
	}
	CNetEvtMgr::Instance()->PushSendEvt(m_dwConnectionID,  m_pSendBuf, nLen);
	return true;
}

void CEpollSock::Close(bool bPushEvent)
{
	if(m_bConnected)
	{
		if(bPushEvent)
			CNetEvtMgr::Instance()->PushTerminateEvt(m_dwConnectionID, m_bAccepted, m_dwParentID);
		
		m_bConnected = false;
	}
	if(m_hSock != -1)
	{
		m_poEpollCtrl->Del(m_hSock);
		close(m_hSock);
		m_hSock = -1;
	}
}

void CEpollSock::OnError(int32 nSDError, int32 nSysError)
{
	if(false == m_bConnected && false == m_bAccepted)
	{
		//连接失败
		CNetEvtMgr::Instance()->PushConnErrEvt(nSysError, m_dwParentID);
	}
	else
	{
		CNetEvtMgr::Instance()->PushErrorEvt(m_dwConnectionID, nSDError, nSysError);
	}
}

bool CEpollSock::OnInitiativeConn()
{
	int32 nError = 0;
	socklen_t nLen = sizeof(nError);
	if(getsockopt(m_hSock, SOL_SOCKET, SO_ERROR, &nError, &nLen) < 0)
	{
		OnError(NET_CONNECT_FAIL, errno);
		return false;
	}
	
	if(nError != 0)
	{
		INFO("CEpollSock::OnInitiativeConn, error %d, %s", nError, strerror(nError));
		OnError(NET_CONNECT_FAIL, nError);
		return false;
	}

	if(false == m_poEpollCtrl->Mod(m_hSock, EPOLLIN, &m_stEpollData))
	{
		CRITICAL("%s, epoll mod failed, errno %d, %s", __FUNCTION__, errno, strerror(errno));
		CNetEvtMgr::Instance()->PushConnErrEvt(errno, m_dwParentID);
		return false;
	}

	CSDConnection* poConnection = CConnectionMgr::Instance()->Create();
	if(NULL == poConnection)
	{
		CRITICAL("%s, Create CSDConnection failed", __FUNCTION__);
		CNetEvtMgr::Instance()->PushConnErrEvt(0, m_dwParentID);
		return false;
	}
	m_dwConnectionID = poConnection->GetID();

	poConnection->SetAccept(false);
	poConnection->SetParentID(m_dwParentID);
	poConnection->SetSession(m_poSession);
	poConnection->SetEpollSockID(m_dwID);

	sockaddr_in stAddr = {0};
	uint32 dwAddrLen = sizeof(stAddr);
	getsockname(m_hSock, (sockaddr*)&stAddr, &dwAddrLen);
	poConnection->SetLocalIP(stAddr.sin_addr.s_addr);
	poConnection->SetLocalPort(ntohs(stAddr.sin_port));

	m_dwLocalIP		= stAddr.sin_addr.s_addr;
	m_wLocalPort	= ntohs(stAddr.sin_port);

	poConnection->SetRemoteIP(m_dwRemoteIP);
	poConnection->SetRemotePort(m_wRemotePort);

	//linux 2.6内核有bug，有时无法获取peername
	//memset(&stAddr, '\0', sizeof(stAddr));
	//dwAddrLen = sizeof(stAddr);
	//getpeername(m_hSock, (sockaddr*)&stAddr, &dwAddrLen);
	//poConnection->SetRemoteIP(stAddr.sin_addr.s_addr);
	//poConnection->SetRemotePort(ntohs(stAddr.sin_port));

	m_bConnected = true;
	CNetEvtMgr::Instance()->PushEstablishEvt(m_dwID, m_dwConnectionID, m_bAccepted, m_dwParentID);
	return true;
}

bool CEpollSock::OnPassiveConn()
{
	m_bConnected = true;
	CNetEvtMgr::Instance()->PushEstablishEvt(m_dwID, m_dwConnectionID, true, m_dwParentID);
	return true;
}

bool CEpollSock::OnRecv()
{
	int32 nLen = recv(m_hSock, m_pRecvBuf+m_nDataReceived, m_dwRecvBufSize-m_nDataReceived, 0);
	if(nLen > 0)
	{
		m_nDataReceived += nLen;
		char* p = m_pRecvBuf;
		while(m_nDataReceived > 0)
		{
			int nUsed = m_poPacketParser->ParsePacket(p, m_nDataReceived);
			if(0 == nUsed)
			{
				//接收缓冲区已满，但还没有装下一个完整包，只好断开连接
				if(m_nDataReceived == (int32)m_dwRecvBufSize)
					return false;
				break;
			}
			else if(nUsed > 0)
			{
				if(nUsed > m_nDataReceived)
				{
					OnError(NET_PACKET_ERROR, 0);
					return false;
				}
                
				while(!m_poRecvQueue->PushBack(p, nUsed))
				{
					WARN("Push Data to RecvQueue fail! RecvQueue full, usleep and try again");
					CNetReqMgr::Instance()->PopToBuffer();
					usleep(10);
				}
				CNetEvtMgr::Instance()->PushRecvEvt(m_dwConnectionID, nUsed);

				p += nUsed;
				m_nDataReceived -= nUsed;
			}
			else	// < 0
			{
				OnError(NET_PACKET_ERROR, 0);
				return false;
			}
		}

		if(m_nDataReceived && (p != m_pRecvBuf))memmove(m_pRecvBuf, p, m_nDataReceived);

		return true;
	}
	else if(nLen < 0)
	{
		if( (errno != EAGAIN) && (errno != EINPROGRESS) )
		{
			//出错
			DBG("CEpollSock::OnRecv, recv fail, errno %d, %s", errno, strerror(errno));
			OnError(NET_RECV_ERROR, errno);
			return false;
		}
        return true;		
	}
	else if(0 == nLen)
	{
		//正常关闭
		DBG("CEpollSock::OnRecv, recv FIN");
		return false;
	}

	return true;
}

bool CEpollSock::OnSend()
{
	CnVerify(m_pSendBuf != NULL && m_nDataSend != 0);

	int32 nRet = send(m_hSock, m_pSendBuf, m_nDataSend, 0);
	if(nRet < 0)
	{
		DBG("CEpollSock::OnSend, send failed, errno %d, %s", errno, strerror(errno));
		OnError(NET_SEND_ERROR, errno);
		return false;
	}
	else if(nRet != m_nDataSend)
	{
		CNetEvtMgr::Instance()->PushSendEvt(m_dwConnectionID,  m_pSendBuf, nRet);
		m_nDataSend -= nRet;
		CnVerify(m_nDataSend > 0);
		memmove(m_pSendBuf, m_pSendBuf+nRet, m_nDataSend);
		return true;
	}

	CNetEvtMgr::Instance()->PushSendEvt(m_dwConnectionID,  m_pSendBuf, nRet);
	
	m_nDataSend = 0;

	if(false == m_poEpollCtrl->Mod(m_hSock, EPOLLIN, &m_stEpollData))
	{
		CRITICAL("CEpollSock::Send, epoll add failed, errno %d, %s", errno, strerror(errno));
		OnError(NET_SEND_ERROR, errno);
		return false;
	}

	return true;
}

int32 CEpollSock::_Send(const char* pData, int32 nLen)
{
	int32 nSend = 0;
	do
	{
		int32 nPiece = nLen - nSend;
		if(nPiece > 4096)
			nPiece = 4096;

		int32 nRet = send(m_hSock, pData+nSend, nPiece, 0);
		if(nRet < 0)
			return -1;
		
		nSend += nRet;
		if(nRet != nPiece)
			break;

	}while(nSend < nLen);

	return nSend;
}

void CEpollSock::SetBind( bool bNeedBind /*= false*/, const char* pszBindIP /*= NULL*/, uint16 wBindPort /*= 0*/ )
{
	m_bNeedBind = bNeedBind;
	m_pszBindIP = pszBindIP;
	m_wBindPort = wBindPort;
}
