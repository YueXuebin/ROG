#include "SDnetPCH.h"
#include "sdnetconfig.h"
#include "sdconnector.h"
#include "netreqmgr.h"
#include "sdnetlinux.h"
#include "sdnetopt.h"

using namespace Crown;

CSDConnector::CSDConnector(void)
{
	m_dwID				= 0;
	m_poPacketParser	= NULL;
	m_poSession			= NULL;
	m_dwRecvBufSize		= DEFAULT_RECVBUF_SIZE;
	m_dwSendBufSize		= DEFAULT_SENDBUF_SIZE;
	memset(m_szIP, '\0', sizeof(m_szIP));
	m_wPort				= 0;
	m_nConnStat			= CONN_NONE;
	m_bNeedBind = false;
	m_pszBindIP = NULL;
	m_wPort = 0;
}

CSDConnector::~CSDConnector(void)
{
}

void CSDConnector::SetPacketParser(ISDPacketParser* poPakcetParser)
{
	m_poPacketParser = poPakcetParser;
}

void CSDConnector::SetSession(ISDSession* poSession)
{
	m_poSession = poSession;
}

void CSDConnector::SetBufferSize(uint32 dwRecvBufSize, uint32 dwSendBufSize)
{
	m_dwRecvBufSize = dwRecvBufSize;
	m_dwSendBufSize = dwSendBufSize;
}

bool CSDConnector::Connect(const char* pszIP, uint16 wPort)
{
	strncpy(m_szIP, pszIP, sizeof(m_szIP)-1);
	m_szIP[sizeof(m_szIP)-1] = '\0';
	m_wPort = wPort;

	return ReConnect();
}

bool CSDConnector::ReConnect(void)
{
	if(NULL == m_poPacketParser)
	{
		WARN("CSDConnector::Connect, Not set PacketParser");
		return false;
	}
	if(NULL == m_poSession)
	{
		WARN("CSDConnector::Connect, Not set Session");
		return false;
	}

	if(m_nConnStat != CONN_NONE)
		return false;

	SNetRequest* pstReq = CNetReqMgr::Instance()->GetFreeReq();
	CnVerify(pstReq != NULL);
	
	pstReq->nType = NETREQ_CONNECT;
	SConnReq* pstConnReq		= &pstReq->stUn.stConn;
    pstConnReq->dwConnectorID	= m_dwID;
	pstConnReq->dwIP			= inet_addr(m_szIP);
	pstConnReq->wPort			= m_wPort;
	pstConnReq->poPacketParser	= m_poPacketParser;
	pstConnReq->poSession		= m_poSession;
	pstConnReq->dwRecvBufSize	= m_dwRecvBufSize;
	pstConnReq->dwSendBufSize	= m_dwSendBufSize;
	pstConnReq->bNeedBind		= m_bNeedBind;
	pstConnReq->pszBindIP		= m_pszBindIP;
	pstConnReq->wBindPort	= m_wBindPort;

	CNetReqMgr::Instance()->PushBackNetReq(pstReq);
	
	m_nConnStat = CONN_PROC;

	return true;
}

void CSDConnector::Release(void)
{
	CSDNetLinux::Instance()->ReleaseConnector(this);
}

void CSDConnector::OnConnect()
{
	m_nConnStat = CONN_OK;
}

void CSDConnector::OnConnectErr(int32 nSysError)
{
	m_nConnStat = CONN_NONE;
	m_poSession->OnError(NET_CONNECT_FAIL, nSysError);
}

void CSDConnector::OnClose()
{
	m_nConnStat = CONN_NONE;
}

void SDAPI CSDConnector::SetOpt( uint32 dwType, void* pOpt )
{
	switch (dwType)
	{
	case CONNECTOR_OPT_SOCKBIND:
		{
			SConnectorOptSockBind* pBindOpt = (SConnectorOptSockBind*)pOpt;
			m_bNeedBind = true;
			m_pszBindIP = pBindOpt->pszIP;
			m_wPort = pBindOpt->wPort;
		}	
		break;
	default:
		break;
	}
}

void CSDConnector::OnBindErr( int32 nSysError )
{
	m_nConnStat = CONN_NONE;
	m_poSession->OnError(NET_BIND_FAIL, nSysError);
}
