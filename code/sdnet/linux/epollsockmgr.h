#ifndef EPOLLSOCKMGR_H
#define EPOLLSOCKMGR_H

#include "epollsock.h"
#include "epollsockpool.h"
#include "bufpool.h"
#include <map>
using namespace std;
using namespace Crown;
#define EPOLLSOCK_SLOT_SIZE	65536


class CEpollSockMgr
{
public:
	CEpollSockMgr(void);
	virtual ~CEpollSockMgr(void);

	bool Init(int32 nMax);
	void Uninit();

	CEpollSock* Create(uint32 dwRecvBufSize, uint32 dwSendBufSize);
	void Release(CEpollSock* poSock);

	CEpollSock* Find(uint32 dwID);

protected:
	CBufPool* _FindBufPool(uint32 dwSize, bool bCreate = true);

protected:
	CEpollSock*				m_apoSockSlot[EPOLLSOCK_SLOT_SIZE];
	uint16					m_awSockIndex[EPOLLSOCK_SLOT_SIZE];
	uint16					m_wNextSlot;
	CEpollSockPool			m_oSockPool;
	CMapBufPool				m_oMapBufPool;
};

#endif
