#ifndef SDNETLINUX_H
#define SDNETLINUX_H

#include "sdnet.h"
#include "epollthrd.h"
#include "simpleloopbuffer.h"
#include "sdlistener.h"
#include "sdconnector.h"
#include "netevent.h"

using namespace Crown;

class CSDNetLinux : public ISDNet
{
	CSDNetLinux(void);
	virtual ~CSDNetLinux(void);

public:
	static bool CreateInstance()
	{
		if(NULL == m_poInstance)
		{
			m_poInstance = CnNew CSDNetLinux;
		}

		return m_poInstance != NULL;
	}
	static CSDNetLinux * Instance(){ return m_poInstance; }
	static void DestoryInstance()
	{
		if(m_poInstance != NULL)
		{
			CnDelete m_poInstance;
			m_poInstance = NULL;
		}
	}

	//Interface ISDBase
	virtual void SDAPI AddRef(void);
	virtual uint32  SDAPI QueryRef(void);
	virtual void SDAPI Release(void);
	virtual SSGDPVersion SDAPI GetVersion(void);
	virtual const char * SDAPI GetModuleName(void);

	//Interface ISDNetModule
	virtual ISDConnector* SDAPI CreateConnector(uint32 dwNetIOType);
	virtual ISDListener*  SDAPI CreateListener(uint32 dwNetIOType);
	virtual bool SDAPI Run(int32 nCount);

	bool Init();
	void Uninit();
	void ReleaseConnector(CSDConnector* poConnector);
	CSDConnector* FindConnector(uint32 dwID);
	void ReleaseListener(CSDListener* poListener);
	CSDListener* FindListener(uint32 dwID);

protected:
	void _ClearConnector();
	void _ClearListener();

	inline void _ProcEstablishEvt(SEstablishEvt* pstEvent);
	inline void _ProcConnErrEvt(SConnErrEvt* pstEvent);
	inline void _ProcErrorEvt(SErrorEvt* pstEvent);
	inline void _ProcTerminateEvt(STerminateEvt* pstEvent);
	inline void _ProcRecvEvt(SRecvEvt* pstEvent);
	inline void _ProcSendEvt(SSendEvt* pstEvent);
	inline void _ProcBindErrEvt(SBindErrEvt* pstEvent);

protected:
	static CSDNetLinux*		m_poInstance;
	uint32					m_dwRef;
	CEpollThrd				m_oEpollThrd;

	uint32					m_dwNextConnectorID;
	CMapConnector			m_oMapConnector;
	uint32					m_dwNextListenerID;
	CMapListener			m_oMapListener;
	CSimpleLoopBuffer		m_oRecvQueue;
	CSimpleLoopBuffer		m_oSendQueue;
	char*				m_pRecvBuf;
};


#endif
