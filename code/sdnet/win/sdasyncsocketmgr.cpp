#include "SDnetPCH.h"
// #include <map>
// #include <WinSock2.h>
#include "sdasyncsocketmgr.h"
#include "sdasyncsocket.h"
//#include "sddebug.h"

using namespace Crown;

IMPLEMENT_SINGLETON(CSDAsyncSocketManager)




const char* g_szAsyncSocketWindowClass = "AsyncSocketWindowClass";		// WindowClass Name

Crown::CSDAsyncSocketManager::CSDAsyncSocketManager() :
m_hWnd(NULL)
{
	InitializeCriticalSection(&m_kCriticalSection);
}

Crown::CSDAsyncSocketManager::~CSDAsyncSocketManager()
{
	DeleteCriticalSection(&m_kCriticalSection);
}

bool Crown::CSDAsyncSocketManager::Init()
{
	// 注册Window
	HMODULE hInstance = GetModuleHandle(NULL);
	WNDCLASS wc;
	if (::GetClassInfo(hInstance, g_szAsyncSocketWindowClass, &wc))
	{
		return false;
	}

	wc.style			= 0;
	wc.lpfnWndProc		= (WNDPROC)CSDAsyncSocketManager::WndProc;
	wc.cbClsExtra		= 0;
	wc.cbWndExtra		= 0;
	wc.hInstance		= hInstance;
	wc.hIcon			= NULL;
	wc.hCursor		= NULL;
	wc.hbrBackground	= NULL;
	wc.lpszMenuName	= NULL;
	wc.lpszClassName	= g_szAsyncSocketWindowClass;
	ATOM wRet = RegisterClass(&wc);

	// 创建接收网络消息的Window
	CREATESTRUCT cs;
	cs.dwExStyle        = 0;
	cs.lpszClass        = g_szAsyncSocketWindowClass;
	cs.lpszName         = NULL;
	cs.style            = WS_POPUP;
	cs.x                = 0;
	cs.y                = 0;
	cs.cx               = 0;
	cs.cy               = 0;
	cs.hwndParent       = NULL;
	cs.hMenu            = NULL;
	cs.hInstance        = GetModuleHandle(NULL);
	cs.lpCreateParams   = 0;

	m_hWnd = ::CreateWindowEx(cs.dwExStyle, cs.lpszClass,
		cs.lpszName, cs.style, cs.x, cs.y, cs.cx, cs.cy,
		cs.hwndParent, cs.hMenu, cs.hInstance, cs.lpCreateParams);

	if(NULL == m_hWnd)
	{
		int nErr = ::GetLastError();
		return false;
	}

	return true;

}

void Crown::CSDAsyncSocketManager::Uninit()
{
	BOOL ret = ::DestroyWindow(m_hWnd);
	CnVerify(ret);
	m_hWnd = NULL;

	HMODULE hInstance = GetModuleHandle(NULL);
	ret = UnregisterClass(g_szAsyncSocketWindowClass, hInstance);
	CnVerify(ret);
}

bool CSDAsyncSocketManager::PostMessage(UINT message, WPARAM wParam, LPARAM lParam)
{
	if(m_poInstance->m_hWnd != NULL)
		return ::PostMessage(m_poInstance->m_hWnd, message, wParam, lParam);

	return false;
}

LRESULT CALLBACK CSDAsyncSocketManager::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if(NULL == m_poInstance)
		return 0;

	switch(message)
	{
	case WM_SDSOCKET_NOTIFY:
		{
			CnVerify(hWnd == m_poInstance->m_hWnd);
			if(hWnd != m_poInstance->m_hWnd)
				return 0;
			CSDAsyncSocket* pSocket = m_poInstance->FromHandle((SOCKET)wParam);

			if(!pSocket)
			{
				m_poInstance->OnSocketDead(wParam, lParam);
				return 0;
			}
			if( m_poInstance->IsDeadSocket( (SOCKET)wParam) )		// Has the socket be closed, ignore message
			{
				CnVerify(false);
				return 0;
			}
			else
				pSocket->DoCallBack(wParam, lParam);
		}
		return 0;

	case WM_SDSOCKET_DEAD:
		{
			CnVerify(hWnd == m_poInstance->m_hWnd);
			if(hWnd != m_poInstance->m_hWnd)
				return 0;
			CnVerify(m_poInstance->IsDeadSocket((SOCKET)wParam));
			m_poInstance->OnSocketDead(wParam, lParam);
		}
		return 0;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
}


CSDAsyncSocket* CSDAsyncSocketManager::FromHandle(SOCKET hSocket)
{
	CSLock lock(m_kCriticalSection);

	CMapSocket::iterator it;

	it = m_oMapSock.find(hSocket);
	if(it != m_oMapSock.end())
		return it->second;

	return NULL;
}

bool CSDAsyncSocketManager::IsDeadSocket(SOCKET hSocket)
{
	CSLock lock(m_kCriticalSection);

	CMapDeadSock::iterator it = m_oMapDeadSock.find(hSocket);

	if(it != m_oMapDeadSock.end())
		return it->second != 0 ? true : false;

	return false;
}

void CSDAsyncSocketManager::AttachHandle( SOCKET hSocket, CSDAsyncSocket* pSocket, bool bDead )
{
	CSLock lock(m_kCriticalSection);

	if (!bDead)
	{
		CnVerify(FromHandle(hSocket) == NULL);
	
		if (NULL == Instance())
		{
			CnVerify(false);
		}

		m_oMapSock[hSocket] = pSocket;
	}
	else
	{
		CMapDeadSock::iterator it = m_oMapDeadSock.find(hSocket);
		if(it != m_oMapDeadSock.end())
		{
			int nCount = it->second;
			nCount++;
			m_oMapDeadSock[hSocket] = nCount;
		}
		else
		{
			m_oMapDeadSock[hSocket] = 1;
		}
	}
}

void CSDAsyncSocketManager::DetachHandle(SOCKET hSocket, bool bDead)
{
	if (!bDead)
	{
		CnVerify(FromHandle(hSocket) != NULL);

		m_oMapSock.erase(hSocket);
		//
		// amend by hejiaqi@20071112
		//

		if (m_oMapSock.empty())
		{
			//CSDSockWnd::DestoryInstance();

			m_oMapDeadSock.clear();

		}

		//
		// amend end
		//
	}
	else
	{
		CnVerify(IsDeadSocket(hSocket));

		CMapDeadSock::iterator it = m_oMapDeadSock.find(hSocket);
		if(it != m_oMapDeadSock.end())
		{
			int nCount = it->second;
			nCount--;
			CnVerify(nCount >= 0);

			if(nCount <= 0)
				m_oMapDeadSock.erase(it);
			else
				m_oMapDeadSock[hSocket] = nCount;
		}
	}
}

void CSDAsyncSocketManager::KillSocket(SOCKET hSocket, CSDAsyncSocket* pSocket, DWORD whyClose)
{
	CSLock lock(m_kCriticalSection);

	CnVerify(FromHandle(hSocket) != NULL);

	DetachHandle(hSocket, false);

	AttachHandle(hSocket, pSocket, true);		// 标记该AsyncSocket Dead

	if(!CSDAsyncSocketManager::Instance()->PostMessage(WM_SDSOCKET_DEAD, (WPARAM)hSocket, (LPARAM)whyClose))
	{
		DetachHandle(hSocket, true);
	}
}

void CSDAsyncSocketManager::OnSocketDead(WPARAM wParam, LPARAM lParam)
{
	CnVerify(IsDeadSocket((SOCKET)wParam));
	DetachHandle(wParam, true);
	CSDAsyncSocket* poSocket = (CSDAsyncSocket*)lParam;
	if(poSocket)
		poSocket->OnClose((DWORD)lParam);
}