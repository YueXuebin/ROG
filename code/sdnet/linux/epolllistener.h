#ifndef EPOLLLISTENER_H
#define EPOLLLISTENER_H

#include "epollsock.h"

class CEpollSockMgr;
class CEpollCtrl;

class CEpollListener
{
public:
	CEpollListener(void);
	virtual ~CEpollListener(void);

	void * GetEpollData() { return &m_stEpollData; }

	void SetID(uint32 dwID) { m_dwID = dwID; }
	uint32 GetID() { return m_dwID;}

	void SetEpollCtrl(CEpollCtrl* poEpollCtrl) { m_poEpollCtrl = poEpollCtrl; }
	void SetEpollSockMgr(CEpollSockMgr* poEpollSockMgr) { m_poEpollSockMgr = poEpollSockMgr; }

	void SetListenSock(int32 hSock) { m_hListenSock = hSock; }
	int32 GetListenSock() { return m_hListenSock; }

	void SetPacketParser(ISDPacketParser* poPacketParser) { m_poPacketParser = poPacketParser; }
	ISDPacketParser* GetPacketParser() { return m_poPacketParser; }

	void SetSessionFactory(ISDSessionFactory* poFactory) { m_poSessionFactory = poFactory; }
	ISDSessionFactory* GetSessionFactory() { return m_poSessionFactory; }

	void SetBufferSize(uint32 dwRecvBufSize, uint32 dwSendBufSize) { m_dwRecvBufSize = dwRecvBufSize; m_dwSendBufSize = dwSendBufSize; }
	uint32 GetRecvBufSize() { return m_dwRecvBufSize; }
	uint32 GetSendBufSize() { return m_dwSendBufSize; }

	CEpollSock* Accept();

protected:
	SEpollData			m_stEpollData;
	uint32				m_dwID;
	int32				m_hListenSock;
	CEpollCtrl*			m_poEpollCtrl;
	CEpollSockMgr*		m_poEpollSockMgr;
	ISDPacketParser*	m_poPacketParser;
	ISDSessionFactory*	m_poSessionFactory;
	uint32				m_dwRecvBufSize;
	uint32				m_dwSendBufSize;
};

#endif
