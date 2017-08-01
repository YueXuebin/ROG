#ifndef SDLISTENER_H
#define SDLISTENER_H

#include <map>
#include "sdnet.h"

using namespace Crown;

class CSDListener : public ISDListener
{
public:
	CSDListener(void);
	virtual ~CSDListener(void);

	//Interface ISDListener
	virtual void SDAPI SetPacketParser(ISDPacketParser* poPacketParser);
	virtual void SDAPI SetSessionFactory(ISDSessionFactory* poSessionFactory);
	virtual void SDAPI SetBufferSize(uint32 dwRecvBufSize, uint32 dwSendBufSize);
	virtual void SDAPI SetOpt(uint32 dwType, void* pOpt);
	virtual bool SDAPI Start(const char* pszIP, uint16 wPort, bool bReUseAddr);
	virtual bool SDAPI Stop(void);
	virtual void SDAPI Release(void);

	void SetID(uint32 dwID)	{ m_dwID = dwID;	}
	uint32 GetID()			{ return m_dwID;	}

protected:
	uint32				m_dwID;
	ISDPacketParser*	m_poPacketParser;
	ISDSessionFactory*	m_poSessionFactory;
	uint32				m_dwRecvBufSize;
	uint32				m_dwSendBufSize;

	int32				m_hListenSock;
	bool				m_bStart;
};

typedef std::map<uint32, CSDListener*> CMapListener;

#endif
