#include "SDnetPCH.h"
#include "sdnetconfig.h"
#include "sdconnection.h"
#include "cpsock.h"
#include "sdnetopt.h"
#include "sdnetwin.h"
#include "cpsockmgr.h"
#include "connectionmgr.h"
#include "sdstring.h"
#include "conndatamgr.h"
#include "sdconfig.h"


CSDConnection::CSDConnection()
{
	Reset();
}

CSDConnection::~CSDConnection()
{

}

void CSDConnection::Reset()
{
	m_dwID			= 0;
	m_dwParentID	= 0;
	m_bAccept		= false;
	m_nConnStat		= CONN_NONE;
	m_dwLocalIP		= 0;
	m_wLocalPort	= 0;
	m_dwRemoteIP	= 0;
	m_wRemotePort	= 0;
	//m_szLocalIP[0]	= '\0';
	//m_szRemoteIP[0]	= '\0';
	m_pConnData = NULL;
	m_poSession		= NULL;
}

bool CSDConnection::IsConnected(void)
{
	return (CONN_ASSOCIATE == m_nConnStat);
}

void CSDConnection::SetRemoteIP(uint32 dwIP)
{
	m_dwRemoteIP = dwIP;
	//StrSafeCopy(m_szRemoteIP, inet_ntoa( (in_addr&)m_dwRemoteIP));
}

void CSDConnection::SetLocalIP(uint32 dwIP)
{
	m_dwLocalIP = dwIP;
	//StrSafeCopy(m_szLocalIP, inet_ntoa( (in_addr&)m_dwLocalIP));
}

void CSDConnection::Send(const char* pBuf, uint32 dwLen)
{
	if(m_nConnStat != CONN_ASSOCIATE)
	{
		return;
	}

	if(NULL == pBuf || 0 == dwLen)
	{
		return;
	}

	CnVerify(m_pConnData != NULL);
	m_pConnData->sock.Send(pBuf, dwLen);
}

void CSDConnection::SetOpt(uint32 dwType, void* pOpt)
{
	if(m_nConnStat != CONN_ASSOCIATE)
	{
		return;
	}

	CnVerify(m_pConnData != NULL);

	switch(dwType)
	{
	case CONNECTION_OPT_SOCKOPT:
		{
			SConnectionOptSockopt* pstOpt = (SConnectionOptSockopt*)pOpt;
			if(NULL == pstOpt->pOptVal || pstOpt->nOptLen > 8)
			{
				return;
			}

			setsockopt(m_pConnData->sock.GetSock(), pstOpt->nLevel, pstOpt->nOptName, pstOpt->pOptVal, pstOpt->nOptLen);
		}
		break;
	default:
		break;
	}
}

void CSDConnection::Disconnect(void)
{
	if(IsConnected() )
	{
		m_nConnStat = CONN_CLOSING;

		CnVerify(m_pConnData!= NULL);
		m_pConnData->sock.Close();

		DBG("Disconnect [%s:%d]", GetRemoteIPStr(), m_wRemotePort);
	}
}

void CSDConnection::OnConnect()
{
	CnVerify(CONN_NONE == m_nConnStat);

	m_nConnStat = CONN_OK;

	//如果是主动连接对方，那么要通知Connector连接事件
	if(!m_bAccept)
	{
		CSDConnector* poConnector = CSDNetWin::Instance()->FindConnector(m_dwParentID);
		if(NULL == poConnector)
		{
			return;
		}
		poConnector->OnConnect();
	}
}


void CSDConnection::OnAssociate()
{
	//CnVerify(CONN_OK == m_nConnStat);
	
	m_nConnStat = CONN_ASSOCIATE;

	//如果是主动连接对方，那么要通知Connector连接事件
	if(!m_bAccept)
	{
		CSDConnector* poConnector = CSDNetWin::Instance()->FindConnector(m_dwParentID);
		if(NULL == poConnector)
		{
			return;
		}
		poConnector->OnConnect();
	}
	CnVerify(m_poSession != NULL);
	m_poSession->SetConnection(this);
	m_poSession->OnEstablish();
}

void CSDConnection::OnSend(const char * pBuf, uint32 dwLen)
{
#ifdef SDNET_HAS_SEND_REPORT
	if (m_poSession != NULL )
	{
		m_poSession->OnSend(pBuf, dwLen); 
	}
#endif
}

void CSDConnection::OnClose()
{

	INFO("[%s:%d]CSDConnection::OnClose  Release session state %d", MSG_MARK, m_nConnStat);

	if (m_nConnStat == CONN_NONE) 
	{
		return ; 
	}
	

	/*
	if(CONN_ASSOCIATE == m_nConnStat)
	{
		m_poSession->SetConnection(this);
		m_poSession->OnEstablish();
	}
	*/

	m_nConnStat = CONN_NONE;

	//如果是主动连接对方，要通知Connector断线事件
	if(false == m_bAccept)
	{
		CSDConnector* poConnector = CSDNetWin::Instance()->FindConnector(m_dwParentID);
		if(poConnector != NULL)
		{
			poConnector->OnClose();
		}
	}


	CnVerify(m_poSession != NULL);
	m_poSession->OnTerminate();
	m_poSession->Release();
	m_poSession = NULL;

	CConnDataMgr::Instance()->Release(m_pConnData); 
	//CCPSockMgr::Instance()->Release(m_poSock);
	//CConnectionMgr::Instance()->Release(this);
	//CCPSockMgr::Instance()->DelayRelease(m_poSock);
}

//// cwy add 2009-03-24 add for expanding
//
// returns the free send-buffer size
//
uint32 SDAPI CSDConnection::GetSendBufFree( void )
{
	if (NULL != m_pConnData)
	{
		return m_pConnData->sock.GetSendBufFree();
	}
	
	return 0;
}
