#ifndef EPOLLTHRD_H
#define EPOLLTHRD_H

#include "sdthread.h"
#include "simpleloopbuffer.h"
#include "epollctrl.h"
#include "epolllistenermgr.h"
#include "epollsockmgr.h"
#include "netrequest.h"

using namespace Crown;

class CEpollThrd 
{
public:
	CEpollThrd(void);
	virtual ~CEpollThrd(void);

	//Interface ISDThread
	virtual void SDAPI ThrdProc();
	virtual void SDAPI Terminate();

	bool Start(CSimpleLoopBuffer* poRecvQueue, CSimpleLoopBuffer* poSendQueue);
	void Stop();

protected:
	bool _Init();
	void _MainLoop();
	void _Uninit();

	//// 2009-03-25 cwy add for interface expanding
	void _SetEpollSockMgrToConnection();

	bool _ProcEpoll();
	void _ProcRequest();

	void _ProcStartListenReq(SStartListenReq* pstReq);
	void _ProcStopListenReq(SStopListenReq* pstReq);
	void _ProcConnReq(SConnReq* pstReq);
	void _ProcDisconnReq(SDisconnReq* pstReq);
	void _ProcSendReq(SSendReq* pstReq);
	void _ProcSockOptReq(SSockOptReq* pstReq);
	static SDTHREAD_DECLARE(RunThreadFunc) (void * arg); 
protected:
	bool				m_bTerminate;
	CSDThread		m_oThrdCtrl;
	
	CSimpleLoopBuffer*		m_poRecvQueue;
	CSimpleLoopBuffer*		m_poSendQueue;
	CEpollCtrl			m_oEpollCtrl;
	CEpollSockMgr		m_oEpollSockMgr;
	CEpollListenerMgr	m_oListenerMgr;
	char*				m_pSendBuf;
};

#endif
