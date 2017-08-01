#ifndef EPOLLLISTENERMGR_H
#define EPOLLLISTENERMGR_H

#include <map>
#include "epolllistener.h"

typedef std::map<uint32, CEpollListener*>	CMapEpollListener;

class CEpollListenerMgr
{
public:
	CEpollListenerMgr(void);
	virtual ~CEpollListenerMgr(void);

	void Uninit();
	CEpollListener* Create(uint32 dwID);
	CEpollListener* Find(uint32 dwID);
	void Release(CEpollListener* poListener);

protected:
	CMapEpollListener	m_oMapListener;
};

#endif
