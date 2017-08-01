#ifndef SDCONNECTOR_H
#define SDCONNECTOR_H

#include "sdnet.h"
#include <map>

using namespace Crown;

class CSDConnector : public ISDConnector
{
	enum EConnStat
	{
		CONN_NONE = 0,
		CONN_PROC,
		CONN_OK
	};
public:
	CSDConnector(void);
	virtual ~CSDConnector(void);

	virtual void SDAPI SetPacketParser(ISDPacketParser* poPakcetParser);
	virtual void SDAPI SetSession(ISDSession* poSession);
	virtual void SDAPI SetBufferSize(uint32 dwRecvBufSize, uint32 dwSendBufSize);
	virtual bool SDAPI Connect(const char* pszIP, uint16 wPort);
	virtual bool SDAPI ReConnect(void);
	virtual void SDAPI Release(void);
	virtual void SDAPI SetOpt(uint32 dwType, void* pOpt);

	void SetID(uint32 dwID)	{ m_dwID = dwID; }
	uint32 GetID()			{ return m_dwID; }

	void OnConnect();
	void OnConnectErr(int32 nSysError);
	void OnClose();
	
	void OnBindErr(int32 nSysError);

protected:
	uint32					m_dwID;
	ISDPacketParser*		m_poPacketParser;
	ISDSession*				m_poSession;
	uint32					m_dwRecvBufSize;
	uint32					m_dwSendBufSize;
	char					m_szIP[32];
	uint16					m_wPort;
	EConnStat				m_nConnStat;

	bool						m_bNeedBind;
	const char*			m_pszBindIP;
	uint16						m_wBindPort;
};

typedef std::map<uint32, CSDConnector*>	CMapConnector;

#endif
