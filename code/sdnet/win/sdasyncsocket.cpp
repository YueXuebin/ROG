#include "SDnetPCH.h"

#include "sdasyncsocket.h"
#include "sdasyncsocketmgr.h"


using namespace std;
using namespace Crown;

CSDAsyncSocket::CSDAsyncSocket(void) :
m_hSocket(INVALID_SOCKET)
{

}

CSDAsyncSocket::~CSDAsyncSocket(void)
{
	if (m_hSocket != INVALID_SOCKET)
	{
		closesocket(m_hSocket);
		CSDAsyncSocketManager::Instance()->KillSocket(m_hSocket, NULL, SDASYNC_SOCK_ERR_NORMAL);	
		m_hSocket = INVALID_SOCKET;
	}
}

bool CSDAsyncSocket::Create(LPCTSTR lpszSocketAddress, UINT nSocketPort, int nSocketType, long lEvent)
{
	if (Socket(nSocketType, lEvent))
	{
		if (Bind(nSocketPort,lpszSocketAddress))
			return true;
		int nResult = GetLastError();
		Close(SDASYNC_SOCK_ERR_NORMAL);
		WSASetLastError(nResult);
	}
	return false;
}

bool CSDAsyncSocket::Socket(int nSocketType, long lEvent,
							int nProtocolType, int nAddressFormat)
{
	CnVerify(m_hSocket == INVALID_SOCKET);

	m_hSocket = socket(nAddressFormat, nSocketType, nProtocolType);
	if (m_hSocket != INVALID_SOCKET)
	{
		CSDAsyncSocketManager::Instance()->AttachHandle(m_hSocket, this, false);
		return AsyncSelect(lEvent);
	}
	else
	{
		int error = WSAGetLastError();
	}
	return false;
}

/////////////////////////////////////////////////////////////////////////////
// CAsyncSocket Attributes

bool CSDAsyncSocket::Attach(SOCKET hSocket, long lEvent)
{
	CnVerify(hSocket != INVALID_SOCKET); 

	if (hSocket == INVALID_SOCKET)
	{
		return false;
	}

	m_hSocket = hSocket;
	CSDAsyncSocketManager::Instance()->AttachHandle(hSocket, this);

	return AsyncSelect(lEvent);
}

SOCKET CSDAsyncSocket::Detach()
{
	SOCKET hSocket = m_hSocket;
	if (AsyncSelect(0))
	{
		CSDAsyncSocketManager::Instance()->KillSocket(hSocket, this, SDASYNC_SOCK_ERR_NORMAL);
		m_hSocket = INVALID_SOCKET;
		return hSocket;
	}
	return INVALID_SOCKET;
}

bool CSDAsyncSocket::GetPeerName(string& rPeerAddress, UINT& rPeerPort)
{
	SOCKADDR_IN sockAddr;
	memset(&sockAddr, 0, sizeof(sockAddr));

	int nSockAddrLen = sizeof(sockAddr);
	bool bResult = GetPeerName((SOCKADDR*)&sockAddr, &nSockAddrLen);
	if (bResult)
	{
		rPeerPort = ntohs(sockAddr.sin_port);
		rPeerAddress = inet_ntoa(sockAddr.sin_addr);
	}
	return bResult;
}

bool CSDAsyncSocket::GetPeerName(SOCKADDR* lpSockAddr, int* lpSockAddrLen)
{
	return getpeername(m_hSocket, lpSockAddr, lpSockAddrLen) != SOCKET_ERROR;
}

bool CSDAsyncSocket::GetSockName(string& rSocketAddress, UINT& rSocketPort)
{
	SOCKADDR_IN sockAddr;
	memset(&sockAddr, 0, sizeof(sockAddr));

	int nSockAddrLen = sizeof(sockAddr);
	bool bResult = GetSockName((SOCKADDR*)&sockAddr, &nSockAddrLen);
	if (bResult)
	{
		rSocketPort = ntohs(sockAddr.sin_port);
		rSocketAddress = inet_ntoa(sockAddr.sin_addr);
	}
	return bResult;
}

bool CSDAsyncSocket::GetSockName(SOCKADDR* lpSockAddr, int* lpSockAddrLen)
{
	return getsockname(m_hSocket, lpSockAddr, lpSockAddrLen) != SOCKET_ERROR;
}


void CSDAsyncSocket::DoCallBack(WPARAM wParam, LPARAM lParam)
{
	if (wParam == 0 && lParam == 0)
		return;

	int nErrorCode = WSAGETSELECTERROR(lParam);
	switch (WSAGETSELECTEVENT(lParam))
	{
	case FD_READ:
		{
			fd_set fds;
			int nReady;
			timeval timeout;

			timeout.tv_sec = 0;
			timeout.tv_usec = 0;

			FD_ZERO(&fds);
			FD_SET(m_hSocket, &fds);
			nReady = select(0, &fds, NULL, NULL, &timeout);
			if (nReady == SOCKET_ERROR)
				nErrorCode = WSAGetLastError();
			if ((nReady == 1) || (nErrorCode != 0))
				_OnReceive(nErrorCode);
		}
		break;
	case FD_WRITE:
		OnSend(nErrorCode);
		break;
	case FD_OOB:
		OnOutOfBandData(nErrorCode);
		break;
	case FD_ACCEPT:
		OnAccept(nErrorCode);
		break;
	case FD_CONNECT:
		OnConnect(nErrorCode);
		break;
	case FD_CLOSE:
		Close(SDASYNC_SOCK_ERR_NORMAL);
		OnClose(nErrorCode);
		break;
	}
}
///////////////////////////////////////////////////////////////////////////////
//// CAscynSocket Operations

bool CSDAsyncSocket::Accept(CSDAsyncSocket& rConnectedSocket,
							SOCKADDR* lpSockAddr, int* lpSockAddrLen)
{
	CnVerify(rConnectedSocket.m_hSocket == INVALID_SOCKET);
	CnVerify(CSDAsyncSocketManager::Instance()->FromHandle(INVALID_SOCKET) == NULL);

	CSDAsyncSocketManager::Instance()->AttachHandle(INVALID_SOCKET, &rConnectedSocket);
	if (CSDAsyncSocketManager::Instance()->FromHandle(INVALID_SOCKET) == NULL)
	{
		// AttachHandle Call has failed
		return false;
	}

	SOCKET hTemp = accept(m_hSocket, lpSockAddr, lpSockAddrLen);	

	if (hTemp == INVALID_SOCKET)
	{
		DWORD dwProblem = GetLastError();
		CSDAsyncSocketManager::Instance()->DetachHandle(rConnectedSocket.m_hSocket, false);
		rConnectedSocket.m_hSocket = INVALID_SOCKET;
		SetLastError(dwProblem);
	}
	else if (CSDAsyncSocketManager::Instance()->FromHandle(INVALID_SOCKET) != NULL)
	{
		rConnectedSocket.m_hSocket = hTemp;
		CSDAsyncSocketManager::Instance()->DetachHandle(INVALID_SOCKET, false);
		CSDAsyncSocketManager::Instance()->AttachHandle(hTemp, &rConnectedSocket);
	}

	return (hTemp != INVALID_SOCKET);
}

bool CSDAsyncSocket::Bind(UINT nSocketPort, LPCTSTR lpszSocketAddress)
{
	SOCKADDR_IN sockAddr;
	memset(&sockAddr,0,sizeof(sockAddr));

	sockAddr.sin_family = AF_INET;

	if (lpszSocketAddress == NULL)
		sockAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	else
	{
		DWORD lResult = inet_addr(lpszSocketAddress);
		if (lResult == INADDR_NONE)
		{
			WSASetLastError(WSAEINVAL);
			return false;
		}
		sockAddr.sin_addr.s_addr = lResult;
	}

	sockAddr.sin_port = htons((u_short)nSocketPort);

	return Bind((SOCKADDR*)&sockAddr, sizeof(sockAddr));
}

bool CSDAsyncSocket::Bind(const SOCKADDR* lpSockAddr, int nSockAddrLen)
{
	return bind(m_hSocket, lpSockAddr, nSockAddrLen) != SOCKET_ERROR;
}

void CSDAsyncSocket::Close(DWORD whyClose)
{
	if (m_hSocket != INVALID_SOCKET)
	{
		closesocket(m_hSocket);
		CSDAsyncSocketManager::Instance()->KillSocket(m_hSocket, this, whyClose);
		m_hSocket = INVALID_SOCKET;
	}
}


bool CSDAsyncSocket::Connect(LPCTSTR lpszHostAddress, UINT nHostPort)
{
	CnVerify(lpszHostAddress != NULL);

	if (lpszHostAddress == NULL)
	{
		return false;
	}

	SOCKADDR_IN sockAddr;
	memset(&sockAddr,0,sizeof(sockAddr));

	sockAddr.sin_family = AF_INET;
	sockAddr.sin_addr.s_addr = inet_addr(lpszHostAddress);

	if (sockAddr.sin_addr.s_addr == INADDR_NONE)
	{
		LPHOSTENT lphost;
		lphost = gethostbyname(lpszHostAddress);
		if (lphost != NULL)
			sockAddr.sin_addr.s_addr = ((LPIN_ADDR)lphost->h_addr)->s_addr;
		else
		{
			WSASetLastError(WSAEINVAL);
			return false;
		}
	}

	sockAddr.sin_port = htons((u_short)nHostPort);

	return Connect((SOCKADDR*)&sockAddr, sizeof(sockAddr));
}

bool CSDAsyncSocket::Connect(const SOCKADDR* lpSockAddr, int nSockAddrLen)
{
	return ConnectHelper(lpSockAddr, nSockAddrLen);
}

int CSDAsyncSocket::Receive(void* lpBuf, int nBufLen, int nFlags)
{
	return recv(m_hSocket, (LPSTR)lpBuf, nBufLen, nFlags);
}

int CSDAsyncSocket::ReceiveFrom(void* lpBuf, int nBufLen, string& rSocketAddress, UINT& rSocketPort, int nFlags)
{
	SOCKADDR_IN sockAddr;

	memset(&sockAddr, 0, sizeof(sockAddr));

	int nSockAddrLen = sizeof(sockAddr);
	int nResult = ReceiveFrom(lpBuf, nBufLen, (SOCKADDR*)&sockAddr, &nSockAddrLen, nFlags);
	if(nResult != SOCKET_ERROR)
	{
		rSocketPort = ntohs(sockAddr.sin_port);
		rSocketAddress = inet_ntoa(sockAddr.sin_addr);
	}
	return nResult;
}

int CSDAsyncSocket::ReceiveFrom(void* lpBuf, int nBufLen, SOCKADDR* lpSockAddr, int* lpSockAddrLen, int nFlags)
{
	return ReceiveFromHelper(lpBuf, nBufLen, lpSockAddr, lpSockAddrLen, nFlags);
}

int CSDAsyncSocket::Send(const void* lpBuf, int nBufLen, int nFlags)
{	 
	return send(m_hSocket, (LPSTR)lpBuf, nBufLen, nFlags);
}

int CSDAsyncSocket::SendTo(const void* lpBuf, int nBufLen, UINT nHostPort, LPCTSTR lpszHostAddress, int nFlags)
{
	SOCKADDR_IN sockAddr;

	memset(&sockAddr,0,sizeof(sockAddr));

	sockAddr.sin_family = AF_INET;

	if (lpszHostAddress == NULL)
		sockAddr.sin_addr.s_addr = htonl(INADDR_BROADCAST);
	else
	{
		sockAddr.sin_addr.s_addr = inet_addr(lpszHostAddress);
		if (sockAddr.sin_addr.s_addr == INADDR_NONE)
		{
			LPHOSTENT lphost;
			lphost = gethostbyname(lpszHostAddress);
			if (lphost != NULL)
				sockAddr.sin_addr.s_addr = ((LPIN_ADDR)lphost->h_addr)->s_addr;
			else
			{
				WSASetLastError(WSAEINVAL);
				return SOCKET_ERROR;
			}
		}
	}

	sockAddr.sin_port = htons((u_short)nHostPort);

	return SendTo(lpBuf, nBufLen, (SOCKADDR*)&sockAddr, sizeof(sockAddr), nFlags);
}

int CSDAsyncSocket::SendTo(const void* lpBuf, int nBufLen, const SOCKADDR* lpSockAddr, int nSockAddrLen, int nFlags)
{
	return SendToHelper(lpBuf, nBufLen, lpSockAddr, nSockAddrLen, nFlags);
}

bool CSDAsyncSocket::AsyncSelect(long lEvent)
{
	CnVerify(m_hSocket != INVALID_SOCKET);

	CnVerify(CSDAsyncSocketManager::Instance()->m_hWnd != NULL);

	return WSAAsyncSelect(m_hSocket, CSDAsyncSocketManager::Instance()->m_hWnd, WM_SDSOCKET_NOTIFY, lEvent) != SOCKET_ERROR;
}

// void CSDAsyncSocket::_OnReceive(int /*nErrorCode*/)
// {
// 	int nRecvLen = Receive(m_szNetBuf + m_nDataLen , ASYNC_SOCK_BUF_SIZE - m_nDataLen);
// 	
// 	switch (nRecvLen)
// 	{
// 	case 0:
// 		OnClose(SDASYNC_SOCK_ERR_NORMAL);		// 服务器主动断开
// 		break;
// 	case SOCKET_ERROR:
// 		if (GetLastError() != WSAEWOULDBLOCK) 
// 		{
// 			OnClose(GetLastError());
// 		}
// 		break;
// 	default:
// 		{
// 			m_nDataLen += nRecvLen;
// 			while(m_nDataLen > 0)
// 			{
// 
// 				int32 nUsed = YnCliPacketParser::Instance()->ParsePacket(m_szNetBuf, m_nDataLen);	// nUsed为该包的长度
// 
// 				if(nUsed > 0)
// 				{
// 					CnVerify(nUsed <= m_nDataReceived);
// 
// 					// 解包头
// 					const STNetMsgHeader* pstHeader = reinterpret_cast<STNetMsgHeader*>(const_cast<char *>(m_szNetBuf));
// 					uint32 wMsgID = ntohl(pstHeader->wMsgID);
// 					uint32 dwDataLen = ntohl(pstHeader->dwDataLen);
// 
// 					// Decode data.
// 					char* pBodyData = const_cast<char*>(m_szNetBuf + sizeof(STNetMsgHeader));
// 					int32 nBodyLen = dwDataLen - sizeof(STNetMsgHeader);
// 					char pData[MAX_PACK_LEN];
// 					int32 nDecodeLen = g_GBGameProto.Decode( wMsgID, pBodyData, nBodyLen, pData, MAX_PACK_LEN);
// 					
// 					OnReceive( wMsgID, pData );
// 				}
// 				else if( nUsed == 0)
// 				{
// 					if( m_nDataLen > ASYNC_SOCK_BUF_SIZE )
// 					{
// 						OnClose(SDASYNC_SOCK_ERR_PACKET);			// 数据包太长，超过了buffer大小，没法收						
// 					}
// 					break;
// 				}				
// 				else
// 				{	// 包头数据不合法
// 					OnClose(SDASYNC_SOCK_ERR_PACKET);
// 					break;
// 				}
// 			}
// 		}
// 	}
// }
// 
// void CSDAsyncSocket::OnSend(int nErrorCode)
// {
// }
// 
// void CSDAsyncSocket::OnOutOfBandData(int /*nErrorCode*/)
// {
// }
// 
// void CSDAsyncSocket::OnAccept(int /*nErrorCode*/)
// {
// }
// 
// void CSDAsyncSocket::OnConnect(int /*nErrorCode*/)
// {
// }
// 
// void CSDAsyncSocket::OnClose(int /*nErrorCode*/)
// {
// }



int CSDAsyncSocket::ReceiveFromHelper(void* lpBuf, int nBufLen, SOCKADDR* lpSockAddr, int* lpSockAddrLen, int nFlags)
{
	return recvfrom(m_hSocket, (LPSTR)lpBuf, nBufLen, nFlags, lpSockAddr, lpSockAddrLen);
}

int CSDAsyncSocket::SendToHelper(const void* lpBuf, int nBufLen, const SOCKADDR* lpSockAddr, int nSockAddrLen, int nFlags)
{
	return sendto(m_hSocket, (LPSTR)lpBuf, nBufLen, nFlags, lpSockAddr, nSockAddrLen);
}

bool CSDAsyncSocket::ConnectHelper(const SOCKADDR* lpSockAddr, int nSockAddrLen)
{
	if (SOCKET_ERROR == connect(m_hSocket, lpSockAddr, nSockAddrLen))
	{
		int nErr = GetLastError();
		if (WSAEWOULDBLOCK == nErr)
		{
			return true;
		}
		else
			return false;
	}
	return true;
}

bool CSDAsyncSocket::ShutDown(int nHow)
{
	return shutdown(m_hSocket, nHow) != SOCKET_ERROR;
}

bool CSDAsyncSocket::IOCtl(long lCommand, DWORD* lpArgument)
{
	return ioctlsocket(m_hSocket, lCommand, lpArgument) != SOCKET_ERROR;
}

bool CSDAsyncSocket::Listen(int nConnectionBacklog)
{
	return listen(m_hSocket, nConnectionBacklog) != SOCKET_ERROR;
}

bool CSDAsyncSocket::SetSockOpt(int nOptionName, const void* lpOptionValue, int nOptionLen, int nLevel)
{
	return setsockopt(m_hSocket, nLevel, nOptionName, (const char*)lpOptionValue, nOptionLen) != SOCKET_ERROR;
}

bool CSDAsyncSocket::GetSockOpt(int nOptionName, void* lpOptionValue, int* lpOptionLen, int nLevel)
{
	return getsockopt(m_hSocket, nLevel, nOptionName, (char*)lpOptionValue, lpOptionLen) != SOCKET_ERROR;
}

int CSDAsyncSocket::GetLastError()
{
	return WSAGetLastError();
}
