#ifndef EPOLLCTRL_H
#define EPOLLCTRL_H

#include <sys/epoll.h>

class CEpollCtrl
{
public:
	CEpollCtrl(void);
	virtual ~CEpollCtrl(void);

	//最大连接数
	bool Init(int nMaxConn);

	void Uninit();

	//毫秒
	int Wait(int nMilliSecond);

	//增加
	bool Add(int hSock, uint32 dwEvents, void* ptr);

	//修改
	bool Mod(int hSock, uint32 dwEvents, void* ptr);

	//删除
	bool Del(int hSock);

	//取得有事件发生的指针
	epoll_event * GetEvent(int nPos);

protected:
	int				m_nMaxConn;
    int				m_hEpoll;
    epoll_event*	m_pstEvts;
};


#endif
