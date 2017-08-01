#pragma once

/*
 *	异步Socket管理器
 */
#ifndef sdasyncsocketmgr_h__
#define sdasyncsocketmgr_h__

#include "sdsingleton.h"
#include "sdmutex.h"

namespace Crown{

class CSDAsyncSocket;

class CSDAsyncSocketManager
{
	friend CSDAsyncSocket;

	CSDAsyncSocketManager();
	~CSDAsyncSocketManager();

	DECLARE_SINGLETON(CSDAsyncSocketManager)

	typedef std::map<SOCKET, CSDAsyncSocket*>   CMapSocket;
	typedef std::map<SOCKET, int>               CMapDeadSock;

	class CSLock
	{
	public:
		CSLock(CRITICAL_SECTION& m) : m_CS(m) {EnterCriticalSection(&m_CS);}
		~CSLock()	{LeaveCriticalSection(&m_CS);}
	private:
		CRITICAL_SECTION& m_CS;

		CSLock& operator = (const CSLock&);		// for disable warning C4512
	};

public:
	bool	Init();
	void	Uninit();

	HWND	GetHWnd() {return m_hWnd;}

private:
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);		// 
	static bool PostMessage(UINT message, WPARAM wParam, LPARAM lParam);

	bool CreateWnd();
	ATOM MyRegisterClass(HINSTANCE hInstance);

	CSDAsyncSocket* FromHandle(SOCKET hSocket);
	void AttachHandle( SOCKET hSocket, CSDAsyncSocket* pSocket, bool bDead = false );
	void DetachHandle( SOCKET hSocket, bool bDead = false );
	bool IsDeadSocket(SOCKET hSocket);
	void KillSocket(SOCKET hSocket, CSDAsyncSocket* pSocket, DWORD whyClose);
	void OnSocketDead(WPARAM wParam, LPARAM lParam);
	void OnSocketClose(WPARAM wParam, LPARAM lParam);

	HWND             m_hWnd;					// 接受网络消息的窗口handle
	CRITICAL_SECTION		m_kCriticalSection;

	CMapSocket       m_oMapSock;
	CMapDeadSock     m_oMapDeadSock;

};


}	// namespace Crown

#endif // sdasyncsocketmgr_h__