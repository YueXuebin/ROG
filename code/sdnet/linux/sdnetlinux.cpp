#include "SDnetPCH.h"
#include "sdnetconfig.h"
#include "sdnetlinux.h"
#include "netevent.h"
#include "netevtmgr.h"
#include "netreqmgr.h"
#include "connectionmgr.h"
//#include "sdgatelistener.h"

CSDNetLinux*	CSDNetLinux::m_poInstance = NULL;

ISDNet* Crown::SDNetGetModule(const SSGDPVersion *pstVersion)
{
	if(false == CSDNetLinux::CreateInstance())
		return NULL;
	CSDNetLinux::Instance()->AddRef();

	if(false == CSDNetLinux::Instance()->Init())
	{
		CRITICAL("%s, Init CSDNetLinux failed", __FUNCTION__);
		CSDNetLinux::DestoryInstance();
		return NULL;
	}

	return CSDNetLinux::Instance();
}

void SDAPI Crown::SDNetSetOpt(uint32 dwType, void* pOpt)
{
	switch(dwType)
	{
	case NETLIN_OPT_MAX_CONNECTION:
		{
			SNetLinOptMaxConnection* pstConn = (SNetLinOptMaxConnection*)pOpt;
			MAX_CONNECTION = pstConn->nMaxConnection;
		}
		break;
	case NETLIN_OPT_QUEUE_SIZE:
		{
			SNetLinOptQueueSize* pstQueue = (SNetLinOptQueueSize*)pOpt;
			if(pstQueue->nRecvBufSize != -1)RECV_LOOP_BUF_SIZE = pstQueue->nRecvBufSize;
			if(pstQueue->nSendBufSize != -1)SEND_LOOP_BUF_SIZE = pstQueue->nSendBufSize;
			if(pstQueue->nRequestQueueSize != -1)MAX_NET_REQUEST = pstQueue->nRequestQueueSize;
			if(pstQueue->nEventQueueSize != -1)MAX_NET_EVENT = pstQueue->nEventQueueSize;	
		}
		break;
	case NETLIN_OPT_ADVANCE_PARAM:
		{
			SNetLinOptAdvanceParam* pstParam = (SNetLinOptAdvanceParam*)pOpt;
			if(pstParam->nParam1 != -1)
				POP_TO_BUF_COUNT = pstParam->nParam1;

			if(pstParam->nParam2 != -1)
				VAL_SO_SNDLOWAT = pstParam->nParam2;
		}
		break;
	default:
		break;
	}
}

CSDNetLinux::CSDNetLinux(void)
{
	m_dwRef					= 0;
	m_dwNextConnectorID		= 0;
	m_dwNextListenerID		= 0;
	m_pRecvBuf				= NULL;
}

CSDNetLinux::~CSDNetLinux(void)
{
}

void CSDNetLinux::AddRef(void)
{
	m_dwRef++;
}

uint32 CSDNetLinux::QueryRef(void)
{
	return m_dwRef;
}

void CSDNetLinux::Release(void)
{
	if(m_dwRef <= 1)
	{
		Uninit();
		
		if(NULL != m_poInstance)
		{
			CnDelete m_poInstance;
			m_poInstance = NULL;
		}

		return;
	}
	m_dwRef--;
}

SSGDPVersion CSDNetLinux::GetVersion(void)
{
	return SGDP_VERSION;
}

const char * CSDNetLinux::GetModuleName(void)
{
	return SDNET_MODULENAME;
}

ISDConnector* CSDNetLinux::CreateConnector(uint32 dwNetIOType)
{
	if(NETIO_EPOLL == dwNetIOType)
	{
		CSDConnector* poConnector = CnNew CSDConnector;
		if(NULL == poConnector)
		{
			CRITICAL("%s, new CSDConnector failed", __FUNCTION__);
			return NULL;
		}

		poConnector->SetID(++m_dwNextConnectorID);
		m_oMapConnector[m_dwNextConnectorID] = poConnector;
		return static_cast<ISDConnector*>(poConnector);
	}
	else
	{
		CRITICAL("CSDNetLinux::CreateConnector, not support NetIOType %d", dwNetIOType);
		return NULL;
	}
}

ISDListener*  CSDNetLinux::CreateListener(uint32 dwNetIOType)
{
	if(NETIO_EPOLL == dwNetIOType)
	{
		CSDListener* poListener = CnNew CSDListener;
		if(NULL == poListener)
		{
			CRITICAL("%s, new CSDListener failed", __FUNCTION__);
			return NULL;
		}

		poListener->SetID(++m_dwNextListenerID);
		m_oMapListener[m_dwNextListenerID] = poListener;

		return static_cast<ISDListener*>(poListener);
	}
//	else if (NETIO_EPOLL_GATE == dwNetIOType)
//	{
//		CSDListener* poNomalListener = new CSDListener;
//		if(NULL == poNomalListener)
//		{
//			CRITICAL("CSDNetWin::CreateListener, new CSDListener failed");
//			return NULL;
//		}
//
//		CSDGateListener* poGateListener = new CSDGateListener;
//		if(NULL == poGateListener)
//		{
//			CRITICAL("CSDNetWin::CreateListener, new CSDGateListener failed");
//			delete poNomalListener;
//			return NULL;
//		}
//
//		if(false == poGateListener->Init(poNomalListener))
//		{
//			delete poGateListener;
//			delete poNomalListener;
//			return NULL;
//		}
//		poNomalListener->SetID(++m_dwNextListenerID);
//
//		m_oMapListener[m_dwNextListenerID] = poNomalListener;
//		return poGateListener;
//	}
	else
	{
		CRITICAL("CSDNetLinux::CreateListener, not support NetIOType %d", dwNetIOType);
		return NULL;
	}
}

bool CSDNetLinux::Run(int32 nCount)
{
	do
	{
		SNetEvent stEvent;
		if(false == CNetEvtMgr::Instance()->PopFrontNetEvt(&stEvent))
			return false;

		switch(stEvent.nType)
		{
		case NETEVT_RECV:
			_ProcRecvEvt(&stEvent.stUn.stRecv);
			break;
		case NETEVT_SEND:
			_ProcSendEvt(&stEvent.stUn.stSend); 
			break; 
		case NETEVT_ESTABLISH:
			_ProcEstablishEvt(&stEvent.stUn.stEstablish);
			break;
		case NETEVT_TERMINATE:
			_ProcTerminateEvt(&stEvent.stUn.stTerminate);
			break;
		case NETEVT_CONN_ERR:
			_ProcConnErrEvt(&stEvent.stUn.stConnErr);
			break;
		case NETEVT_ERROR:
			_ProcErrorEvt(&stEvent.stUn.stError);
			break;
		case NETEVT_BINDERR:
			_ProcBindErrEvt(&stEvent.stUn.stBindErr);
			break;
		default:
			CnVerify(false);
			break;
		}
	}while(--nCount != 0);

	return true;
}

bool CSDNetLinux::Init()
{
	m_pRecvBuf = CnNew char[MAX_PKG_LEN];
	if(NULL == m_pRecvBuf)
	{
		CRITICAL("%s, new RecvBuf failed, len %d", __FUNCTION__, MAX_PKG_LEN);
		return false;
	}

	if(false == m_oRecvQueue.Init(RECV_LOOP_BUF_SIZE))
	{
		CRITICAL("%s, Init RecvQueue failed, size %d", __FUNCTION__, RECV_LOOP_BUF_SIZE);
		return false;
	}
	if(false == m_oSendQueue.Init(SEND_LOOP_BUF_SIZE))
	{
		CRITICAL("%s, Init SendQueue failed, size %d", __FUNCTION__, SEND_LOOP_BUF_SIZE);
		return false;
	}

	if(false == CNetEvtMgr::CreateInstance())
	{
		CRITICAL("%s, CNetEvtMgr::CreateInstance failed", __FUNCTION__);
		return false;
	}
	if(false == CNetEvtMgr::Instance()->Init(MAX_NET_EVENT, &m_oRecvQueue))
	{
		CRITICAL("%s, CNetEvtMgr init failed, size=%d", __FUNCTION__, MAX_NET_EVENT);
		return false;
	}

	if(false == CNetReqMgr::CreateInstance())
	{
		CRITICAL("%s, CNetReqMgr::CreateInstance failed", __FUNCTION__);
		return false;
	}
	if(false == CNetReqMgr::Instance()->Init(MAX_NET_REQUEST, &m_oSendQueue))
	{
		CRITICAL("%s, CNetReqMgr init failed, size=%d", __FUNCTION__, MAX_NET_REQUEST);
		return false;
	}

	if(false == CConnectionMgr::CreateInstance())
	{
		CRITICAL("%s, CConnectionMgr::CreateInstance failed", __FUNCTION__);
		return false;
	}
	if(false == CConnectionMgr::Instance()->Init(MAX_CONNECTION))
	{
		CRITICAL("%s, CConnectionMgr init failed, MaxConnection=%d", __FUNCTION__, MAX_CONNECTION);
		return false;
	}

	CSDConnection::SetSendQueue(&m_oSendQueue);

	if(false == m_oEpollThrd.Start(&m_oRecvQueue, &m_oSendQueue))
	{
		CRITICAL("%s, EpollThrd Start failed", __FUNCTION__);
		return false;
	}

	return true;
}

void CSDNetLinux::Uninit()
{
	m_oEpollThrd.Stop();
	CConnectionMgr::DestoryInstance();
	CNetReqMgr::DestoryInstance();
	CNetEvtMgr::DestoryInstance();

	if(m_pRecvBuf != NULL)
	{
		CnDelete[] m_pRecvBuf;
		m_pRecvBuf = NULL;
	}

	_ClearConnector();
	_ClearListener();
}

void CSDNetLinux::ReleaseConnector(CSDConnector* poConnector)
{
	if(NULL == poConnector)
		return;

	m_oMapConnector.erase(poConnector->GetID());
	CnDelete poConnector;
}

CSDConnector* CSDNetLinux::FindConnector(uint32 dwID)
{
	CMapConnector::iterator it = m_oMapConnector.find(dwID);
	if(it == m_oMapConnector.end())
		return NULL;

	return it->second;
}

void CSDNetLinux::ReleaseListener(CSDListener* poListener)
{
	if(NULL == poListener)
		return;

	m_oMapListener.erase(poListener->GetID());
	CnDelete poListener;
}

CSDListener* CSDNetLinux::FindListener(uint32 dwID)
{
	CMapListener::iterator it = m_oMapListener.find(dwID);
	if(it == m_oMapListener.end())
		return NULL;

	return it->second;
}

void CSDNetLinux::_ProcEstablishEvt(SEstablishEvt* pstEvent)
{
	CSDConnection* poConnection = CConnectionMgr::Instance()->Find(pstEvent->dwConnID);
	if(NULL == poConnection)
		return;

	poConnection->OnConnect();
}

void CSDNetLinux::_ProcConnErrEvt(SConnErrEvt* pstEvent)
{
	CSDConnector* poConnector = FindConnector(pstEvent->dwConnectorID);
	if(NULL == poConnector)
		return;

	poConnector->OnConnectErr(pstEvent->nSysErrCode);
}

void CSDNetLinux::_ProcErrorEvt(SErrorEvt* pstEvent)
{
	CSDConnection* poConnection = CConnectionMgr::Instance()->Find(pstEvent->dwConnectionID);
	if(NULL == poConnection)
		return;

	poConnection->OnError(pstEvent->nSDErrCode, pstEvent->nSysErrCode);
}

void CSDNetLinux::_ProcTerminateEvt(STerminateEvt* pstEvent)
{
	CSDConnection* poConnection = CConnectionMgr::Instance()->Find(pstEvent->dwConnectionID);
	if(NULL == poConnection)
		return;

	poConnection->OnClose();
	CConnectionMgr::Instance()->Release(poConnection);
}

void CSDNetLinux::_ProcRecvEvt(SRecvEvt* pstEvent)
{
	//即使没找到Connection也要将接收缓冲Pop出来，接收缓冲和EvtQueue中的Recv事件是一一匹配的
	CnVerify(pstEvent->nLen <= MAX_PKG_LEN);
	char* pRecvBuf = NULL;

	if(pstEvent->pData != NULL)
	{
		pRecvBuf = m_pRecvBuf;
		memcpy(pRecvBuf, pstEvent->pData, pstEvent->nLen);
		CnDelete pstEvent->pData;
	}
	else
	{
		if(false == m_oRecvQueue.PopFront(pRecvBuf, pstEvent->nLen))
		{
			CRITICAL("%s, PopFront RecvData Len %d failed", __FUNCTION__, pstEvent->nLen);
			return;
		}
	}

	CSDConnection* poConnection = CConnectionMgr::Instance()->Find(pstEvent->dwConnectionID);
	if(NULL == poConnection)
		return;

	poConnection->OnRecv(pRecvBuf, pstEvent->nLen);
}

void CSDNetLinux::_ProcSendEvt(SSendEvt* pstEvent)
{
	CSDConnection* poConnection = CConnectionMgr::Instance()->Find(pstEvent->dwConnectionID);
	if(NULL == poConnection)
		return;

	poConnection->OnSend(pstEvent->pBuf,pstEvent->dwLen); 

}
void CSDNetLinux::_ClearConnector()
{
	for(CMapConnector::iterator it = m_oMapConnector.begin(); it != m_oMapConnector.end(); ++it)
		CnDelete it->second;

	m_oMapConnector.clear();
}

void CSDNetLinux::_ClearListener()
{
	for(CMapListener::iterator it = m_oMapListener.begin(); it != m_oMapListener.end(); ++it)
		CnDelete it->second;

	m_oMapListener.clear();
}

void CSDNetLinux::_ProcBindErrEvt( SBindErrEvt* pstEvent )
{
	CSDConnector* poConnector = FindConnector(pstEvent->dwConnectorID);
	if(NULL == poConnector)
		return;

	poConnector->OnBindErr(pstEvent->nSysErrCode);
}
