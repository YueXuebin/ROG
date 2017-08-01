#ifndef EPOLLSOCK_H
#define EPOLLSOCK_H

#include "sdnet.h"
#include "simpleloopbuffer.h"

using namespace Crown;

#define EPOLL_LISTEN	1
#define EPOLL_SOCK		2
#define MAX_SYS_SEND_BUF		200*1024
struct SEpollData
{
	uint32	dwType;
	void*	ptr;
};

class CEpollCtrl;

class CEpollSock
{
public:
	CEpollSock(void);
	virtual ~CEpollSock(void);

	static void SetEpollCtrl(CEpollCtrl* poCtrl) { m_poEpollCtrl = poCtrl; }
	static void SetRecvQueue(CSimpleLoopBuffer* poRecvQueue) { m_poRecvQueue = poRecvQueue; }

	void Reset();
	SEpollData* GetEpollData() { return &m_stEpollData; }

	void SetID(uint32 dwID) { m_dwID = dwID; }
	uint32 GetID() {return m_dwID; }

	void SetConnectionID(uint32 dwID) { m_dwConnectionID = dwID; }
	uint32 GetConnectionID() { return m_dwConnectionID; }

	void SetSock(int32 hSock) { m_hSock = hSock;	}
	int32 GetSock() { return m_hSock; }

	void SetParentID(uint32 dwID) { m_dwParentID = dwID; }
	uint32 GetParentID() { return m_dwParentID; }

	void SetAccepted(bool bAccepted) { m_bAccepted = bAccepted; }
	bool IsAccepted() { return m_bAccepted; }

	uint32 GetRecvBufSize() { return m_dwRecvBufSize; }

	void SetConnected(bool bConnected) { m_bConnected = bConnected;	}
	bool IsConnected() { return m_bConnected;			}

	void SetPacketParser(ISDPacketParser* poPacketParser) { m_poPacketParser = poPacketParser;	}
	void SetSession(ISDSession* poSession) { m_poSession = poSession; }

	void AttachRecvBuf(char* pRecvBuf, uint32 dwRecvBufSize);
	uint32 DetachRecvBuf(char** ppRecvBuf);

	void AttachSendBuf(char* pSendBuf, uint32 dwSendBufSize);
	uint32 DetachSendBuf(char** ppSendBuf);

	void SetSockName(uint32 dwIP, uint16 wPort) { m_dwLocalIP = dwIP; m_wLocalPort = wPort; }
	void SetPeerName(uint32 dwIP, uint16 wPort) { m_dwRemoteIP = dwIP; m_wRemotePort = wPort; }

	uint32 GetLocalIP() { return m_dwLocalIP; }
	uint16 GetLocalPort() { return m_wLocalPort; }
	uint32 GetRemoteIP() { return m_dwRemoteIP; }
	uint16 GetRemotePort() { return m_wRemotePort; }
	uint32 GetSendBufFree() {return m_dwSendBufSize - m_nDataSend;}
	
	void SetBind(bool bNeedBind = false, const char* pszBindIP = NULL, uint16 wBindPort = 0);
	bool Connect(uint32 dwIP, uint16 wPort);
	bool Send(const char* pData, int32 nLen);
	void Close(bool bPushEvent=true);

	bool OnInitiativeConn();	//主动连接回调
	bool OnPassiveConn();		//监听连接回调
	bool OnRecv();
	bool OnSend();
	void OnClose();
	void OnError(int32 nSDError, int32 nSysError);

protected:
	int32 _Send(const char* pData, int32 nLen);
	bool _AddToSendBuf(const char* pData, int32 nLen);

protected:
	static CEpollCtrl*	m_poEpollCtrl;
	static CSimpleLoopBuffer*	m_poRecvQueue;
	
	SEpollData			m_stEpollData;
	uint32				m_dwID;
	uint32				m_dwConnectionID;	//ISDConnection的ID
	int32				m_hSock;
	uint32				m_dwParentID;		//ISDConnector或ISDListner的ID，要看m_bAccepted的值
	bool				m_bAccepted;		//是否是accept的连接
	char*				m_pRecvBuf;
	char*				m_pSendBuf;
	uint32				m_dwRecvBufSize;
	uint32				m_dwSendBufSize;
	int32				m_nDataReceived;	//接收到的数据的长度
	int32				m_nDataSend;		//待发送的数据的长度
    
	bool				m_bConnected;
	ISDPacketParser*	m_poPacketParser;
	ISDSession*			m_poSession;

	uint32				m_dwLocalIP;
	uint16				m_wLocalPort;
	uint32				m_dwRemoteIP;
	uint16				m_wRemotePort;
	bool				m_bNeedBind;
	const char*			m_pszBindIP;
	uint16				m_wBindPort;
};


#endif
