#include "SDnetPCH.h"
#include "sdnetconfig.h"
#include "sdnetopt.h"
#include "sdconnector.h"
#include "sdnetwin.h"
#include "connectctrl.h"

CSDConnector::CSDConnector()
{
	m_dwID				= 0;
	m_poPacketParser	= NULL;
	m_poSession			= NULL;
	m_dwRecvBufSize		= DEFAULT_RECVBUF_SIZE;
	m_dwSendBufSize		= DEFAULT_SENDBUF_SIZE;
	m_dwIP				= 0;
	m_wPort				= 0;
	m_nConnStat			= CONN_NONE;

	//// 2009-03-24 cwy add for interface expanding
	// add bind function
	m_bNeedBind		= false;
	m_pszBindIP		= NULL;
	m_wBindPort		= 0;
	//// ~
}

CSDConnector::~CSDConnector()
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
	m_dwIP	= inet_addr(pszIP);
	m_wPort	= wPort;

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
	{
		WARN("CSDConnector::ReConnect, m_nConnStat is %d", m_nConnStat);
		return false;
	}

	if(false == CConnectCtrl::Instance()->PushConnReq(m_dwID, m_dwIP, m_wPort, 
		m_poPacketParser, m_poSession, m_dwRecvBufSize, m_dwSendBufSize, 
		m_bNeedBind, m_pszBindIP, m_wBindPort))
	{
		return false;
	}

	m_nConnStat = CONN_PROC;
	
	return true;
}

void CSDConnector::Release(void)
{
	CSDNetWin::Instance()->ReleaseConnector(this);
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

//// 2009-03-24 cwy add for interface expanding
// add bind function
void CSDConnector::OnBindErr( int32 nSDError, int32 nSysError )
{
	m_nConnStat = CONN_NONE;
	m_poSession->OnError(NET_BIND_FAIL, nSysError);
}
//// ~

void CSDConnector::OnClose()
{
	m_nConnStat = CONN_NONE;
}

//// 2009-03-24 cwy add for interface expanding
// add bind function
void SDAPI CSDConnector::SetOpt( uint32 dwType, void* pOpt )
{
	switch(dwType)
	{
	case CONNECTOR_OPT_SOCKBIND:
		{
			SConnectorOptSockBind* pstBind = (SConnectorOptSockBind*)pOpt;
			if (NULL == pstBind->pszIP || 0 == pstBind->wPort)
			{
				return;
			}
			m_bNeedBind = true;
			m_pszBindIP = pstBind->pszIP;
			m_wBindPort = pstBind->wPort;
		}
		break;
	default:
		break;
	}
}

//// ~
