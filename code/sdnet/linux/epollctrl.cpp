#include "SDnetPCH.h"
#include "sdnetconfig.h"
#include "nethelp.h"
#include "epollctrl.h"

CEpollCtrl::CEpollCtrl(void)
{
	m_nMaxConn	= 0;
	m_hEpoll	= -1;
	m_pstEvts	= NULL;
}

CEpollCtrl::~CEpollCtrl(void)
{
}

bool CEpollCtrl::Init(int nMaxConn)
{
	DBG("%s, nMaxConn=%d", __FUNCTION__, nMaxConn);

	if(nMaxConn <= 0)
	{
		CRITICAL("%s, Error nMaxConn = %d", __FUNCTION__, nMaxConn);
		return false;
	}
	m_nMaxConn = nMaxConn;

	m_pstEvts = CnNew epoll_event[nMaxConn];
	if(NULL == m_pstEvts)
	{
		CRITICAL("%s, Can not alloc epoll_event, nMaxConn = %d", __FUNCTION__, nMaxConn);
		return false;
	}

	CnVerify(-1 == m_hEpoll);
	m_hEpoll = epoll_create(m_nMaxConn);
	if(m_hEpoll < 0)
	{
		CRITICAL("%s, epoll_create failed, errno %d", __FUNCTION__, errno);
		return false;
	}

	INFO("%s, Success", __FUNCTION__);
	return true;
}

void CEpollCtrl::Uninit()
{
	if(m_hEpoll != -1)
	{
		close(m_hEpoll);
		m_hEpoll = -1;
	}
	if(m_pstEvts != NULL)
	{
		CnDelete m_pstEvts;
		m_pstEvts = NULL;
	}
}

int CEpollCtrl::Wait(int nMilliSecond)
{
	CnVerify(m_hEpoll != -1);
	int nCount = epoll_wait(m_hEpoll, m_pstEvts, m_nMaxConn, nMilliSecond);
	if(nCount < 0)
	{
		if(errno != EINTR)
		{
			CRITICAL("%s, epoll_wait fail, errno %d, %s", __FUNCTION__, errno, strerror(errno));
			return nCount;
		}
		else
		{
			return 0;
		}
	}
	return nCount;
}

bool CEpollCtrl::Add(int hSock, uint32 dwEvents, void* ptr)
{
	DBG("%s, hSock=%d ptr=0x%p", __FUNCTION__, hSock, ptr);

	if(hSock < 0)
	{
		WARN("%s, hSock %d invalid", __FUNCTION__, hSock);
		CnVerify(false);
		return false;
	}
	SetNonBlock(hSock, true);

	epoll_event stEvt;
	stEvt.events	= dwEvents;
	stEvt.data.ptr	= ptr;

	CnVerify(m_hEpoll != -1);
	if(epoll_ctl(m_hEpoll, EPOLL_CTL_ADD, hSock, &stEvt) < 0)
	{
		CRITICAL("%s, epoll_ctl failed, errno %d", __FUNCTION__, errno);
		return false;
	}

	DBG("%s, Success hSock %d", __FUNCTION__, hSock);
	return true;
}

bool CEpollCtrl::Mod(int hSock, uint32 dwEvents, void* ptr)
{
	if(hSock < 0)
	{
		WARN("%s, hSock %d invalid", __FUNCTION__, hSock);
		CnVerify(false);
		return false;
	}

	epoll_event stEvt;
	stEvt.events	= dwEvents;
	stEvt.data.ptr	= ptr;

	CnVerify(m_hEpoll != -1);
	if(epoll_ctl(m_hEpoll, EPOLL_CTL_MOD, hSock, &stEvt) < 0)
	{
		CRITICAL("%s, epoll_ctl failed, errno %d", __FUNCTION__, errno);
		return false;
	}

	DBG("%s, Success hSock %d", __FUNCTION__, hSock);
	return true;
}

bool CEpollCtrl::Del(int hSock)
{
	DBG("%s, hSock=%d", __FUNCTION__, hSock);

	if(hSock < 0)
	{
		WARN("%s, hSock %d invalid", __FUNCTION__, hSock);
		CnVerify(false);
		return false;
	}

	epoll_event stEvt;
	CnVerify(m_hEpoll != -1);
	if(epoll_ctl(m_hEpoll, EPOLL_CTL_DEL, hSock, &stEvt) < 0)
	{
		CRITICAL("%s, epoll_ctl failed, errno %d", __FUNCTION__, errno);
		return false;
	}

	DBG("%s, Success hSock %d", __FUNCTION__, hSock);
	return true;
}

epoll_event * CEpollCtrl::GetEvent(int nPos)
{
	if(nPos < 0 || nPos >= m_nMaxConn)
	{
		CRITICAL("%s, nPos %d is invalid", __FUNCTION__, nPos);
		CnVerify(false);
		return NULL;
	}

	return &m_pstEvts[nPos];
}
