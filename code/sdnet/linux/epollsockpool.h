#ifndef EPOLLSOCKPOOL_H
#define EPOLLSOCKPOOL_H

#include "epollsock.h"
#include <list>

typedef std::list<CEpollSock*>	CListEpollSock;

class CEpollSockPool
{
public:
	CEpollSockPool(void);
	virtual ~CEpollSockPool(void);

	bool Init(int32 nMax);
	void Uninit();
	CEpollSock* Create();
	void Release(CEpollSock* poSock);

protected:
	CEpollSock*		m_pSockArray;
	CListEpollSock	m_oFreeList;
};


#endif
