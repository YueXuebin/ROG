#ifndef SDCONNECTION_H
#define SDCONNECTION_H

#include <list>
#include <map>
#include "sdnet.h"
#include "sdnetopt.h"
#include "simpleloopbuffer.h"
#include "epollsockmgr.h"
#include "sdnetconfig.h"

using namespace Crown;

class CSDConnection : public ISDConnection
{
public:
	CSDConnection(void);
	virtual ~CSDConnection(void);

	static void SetSendQueue(CSimpleLoopBuffer* poSendQueue) { m_poSendQueue = poSendQueue;}
	//// 2009-03-25 cwy add for interface expanding
	static void SetEpollSockMgr(CEpollSockMgr* poEpollSockMgr) {m_poEpollSockMgr = poEpollSockMgr;}
	virtual bool SDAPI IsConnected(void);
	virtual void SDAPI Send(const char* pBuf,uint32 dwLen);
	virtual void SDAPI SetOpt(uint32 dwType, void* pOpt);
	virtual void SDAPI Disconnect(void);
	virtual const uint32 SDAPI GetRemoteIP(void)	{ return m_dwRemoteIP;	}
	virtual const char* SDAPI GetRemoteIPStr(void)	{ return m_szRemoteIP;	}
	virtual uint16  SDAPI GetRemotePort(void)		{ return m_wRemotePort; }
	virtual const uint32 SDAPI GetLocalIP(void)		{ return m_dwLocalIP;	}
	virtual const char* SDAPI GetLocalIPStr(void)	{ return m_szLocalIP;	}
	virtual uint16 SDAPI GetLocalPort(void)			{ return m_wLocalPort;	}
	virtual uint32 SDAPI GetSendBufFree(void);
	void Reset();

	void SetRemoteIP(uint32 dwIP);
	void SetRemotePort(uint16 wPort)	{ m_wRemotePort = wPort;	}
	void SetLocalIP(uint32 dwIP);
	void SetLocalPort(uint16 wPort)		{ m_wLocalPort = wPort;		}

	uint32 GetID()			{ return m_dwID;	}
	void SetID(uint32 dwID)	{ m_dwID = dwID;	}

	void SetAccept(bool bAccept)	{ m_bAccept = bAccept;	}
	bool IsAccept()					{ return m_bAccept;		}

	void SetParentID(uint32 dwParentID)	{ m_dwParentID = dwParentID;	}
	uint32 GetParentID()				{ return m_dwParentID;			}

	void SetEpollSockID(uint32 dwID)	{ m_dwEpollSockID = dwID;		}
	uint32 GetEpollSockID()				{ return m_dwEpollSockID;		}

	void SetSession(ISDSession* poSession)	{ m_poSession = poSession;	}
	ISDSession* GetSession()				{ return m_poSession;		}

	void OnConnect();
	void OnSend(const char * pBuf, uint32 dwLen); 
	void OnClose();
	inline void OnRecv(const char* pData, int32 nLen)
	{
		if(false == m_bConnected)
			return;

		CnAssert(m_poSession != NULL);
		m_poSession->OnRecv(pData, nLen);
	}
	inline void OnError(int32 nSDError, int32 nSysError)
	{
		if(false == m_bConnected)
			return;

		CnAssert(m_poSession != NULL);
		m_poSession->OnError(nSDError, nSysError);
	}

protected:
	bool		m_bConnected;
	uint32		m_dwLocalIP;
	uint16		m_wLocalPort;
	uint32		m_dwRemoteIP;
	uint16		m_wRemotePort;
	char		m_szLocalIP[16];
	char		m_szRemoteIP[16];

	uint32		m_dwID;
	bool		m_bAccept;
	uint32		m_dwParentID;
	uint32		m_dwEpollSockID;

	ISDSession*	m_poSession;

	static CSimpleLoopBuffer*		m_poSendQueue;
	//// 2009-03-25 cwy add for interface expanding
	static CEpollSockMgr* m_poEpollSockMgr;
};

typedef std::list<CSDConnection*> CListConnection;
typedef std::map<uint32, CSDConnection*> CMapConnection;

#endif
