#pragma once

#ifndef _SDASYNCSOCKET_H_
#define _SDASYNCSOCKET_H_

#include "sdsingleton.h"

//using namespace std;


// 断开连接的错误描述
enum ENDisconnErr
{
	SDASYNC_SOCK_ERR_NORMAL = 0,
	SDASYNC_SOCK_ERR_PACKET = -1,
};

// 自定义网络消息
#define WM_SDSOCKET_NOTIFY          (WM_USER+1000)
#define WM_SDSOCKET_DEAD            (WM_USER+2000)

namespace Crown{

class CSDAsyncSocketManager;

class CSDAsyncSocket
{
	friend CSDAsyncSocketManager;
public:
	CSDAsyncSocket(void);
	virtual ~CSDAsyncSocket(void);

	// Construction
public:
	bool Create(LPCTSTR lpszSocketAddress = NULL, UINT nSocketPort = 0,
		int nSocketType=SOCK_STREAM,
		long lEvent = FD_READ | FD_WRITE | FD_OOB | FD_ACCEPT | FD_CONNECT | FD_CLOSE);

	// Attributes
public:
	SOCKET m_hSocket;

	operator SOCKET() const { return m_hSocket; }
	bool Attach(SOCKET hSocket, long lEvent = FD_READ | FD_WRITE | FD_OOB | FD_ACCEPT | FD_CONNECT | FD_CLOSE);
	SOCKET Detach();

	bool GetPeerName(std::string& rPeerAddress, UINT& rPeerPort);
	bool GetPeerName(SOCKADDR* lpSockAddr, int* lpSockAddrLen);

	bool GetSockName(std::string& rSocketAddress, UINT& rSocketPort);
	bool GetSockName(SOCKADDR* lpSockAddr, int* lpSockAddrLen);

	bool SetSockOpt(int nOptionName, const void* lpOptionValue, int nOptionLen, int nLevel = SOL_SOCKET);
	bool GetSockOpt(int nOptionName, void* lpOptionValue, int* lpOptionLen, int nLevel = SOL_SOCKET);

	static int GetLastError();

	// Operations
public:

	virtual bool Accept(CSDAsyncSocket& rConnectedSocket, SOCKADDR* lpSockAddr = NULL, int* lpSockAddrLen = NULL);

	bool Bind(UINT nSocketPort, LPCTSTR lpszSocketAddress = NULL);
	bool Bind (const SOCKADDR* lpSockAddr, int nSockAddrLen);

	void Close(DWORD whyClose);

	bool Connect(LPCTSTR lpszHostAddress, UINT nHostPort);
	bool Connect(const SOCKADDR* lpSockAddr, int nSockAddrLen);

	bool IOCtl(long lCommand, DWORD* lpArgument);

	bool Listen(int nConnectionBacklog=5);

	virtual int Receive(void* lpBuf, int nBufLen, int nFlags = 0);

	int ReceiveFrom(void* lpBuf, int nBufLen,
		std::string& rSocketAddress, UINT& rSocketPort, int nFlags = 0);
	int ReceiveFrom(void* lpBuf, int nBufLen,
		SOCKADDR* lpSockAddr, int* lpSockAddrLen, int nFlags = 0);

	bool ShutDown(int nHow/*SD_SEND*/);

	int Send(const void* lpBuf, int nBufLen, int nFlags = 0);

	int SendTo(const void* lpBuf, int nBufLen,
		UINT nHostPort, LPCTSTR lpszHostAddress = NULL, int nFlags = 0);
	int SendTo(const void* lpBuf, int nBufLen,
		const SOCKADDR* lpSockAddr, int nSockAddrLen, int nFlags = 0);

	bool AsyncSelect(long lEvent =
		FD_READ | FD_WRITE | FD_OOB | FD_ACCEPT | FD_CONNECT | FD_CLOSE);

	bool Socket(int nSocketType=SOCK_STREAM, long lEvent =
		FD_READ | FD_WRITE | FD_OOB | FD_ACCEPT | FD_CONNECT | FD_CLOSE,
		int nProtocolType = 0, int nAddressFormat = PF_INET);

	// 子类继承
protected:
	virtual void _OnReceive(int nErrorCode) = 0;
	//virtual void OnReceive(DWORD MsgId, void* pData) = 0;
	virtual void OnSend(int nErrorCode) = 0;
	virtual void OnOutOfBandData(int nErrorCode) = 0;
	virtual void OnAccept(int nErrorCode) = 0;
	virtual void OnConnect(int nErrorCode) = 0;
	virtual void OnClose(int nErrorCode) = 0;

	// 默认实现
protected:
	virtual bool ConnectHelper(const SOCKADDR* lpSockAddr, int nSockAddrLen);
	virtual int ReceiveFromHelper(void* lpBuf, int nBufLen,
		SOCKADDR* lpSockAddr, int* lpSockAddrLen, int nFlags);
	virtual int SendToHelper(const void* lpBuf, int nBufLen,
		const SOCKADDR* lpSockAddr, int nSockAddrLen, int nFlags);

private:
	void DoCallBack(WPARAM wParam, LPARAM lParam);
	

	
};

}	// namespace Crown

#endif

