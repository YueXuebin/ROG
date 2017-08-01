#include "SDnetPCH.h"
#include "sdnetconfig.h"
#include "sdnetwin.h"
#include "sdnetopt.h"
#include "sdlistener.h"
//#include "cpsockmgr.h"
#include "eventmgr.h"
#include "iocpctrl.h"
//#include "connectionmgr.h"
#include "connectctrl.h"
//#include "sdgatelistener.h"
//#include "vercheck.h"
//#include "syssockmgr.h"

ISDNet* SDAPI Crown::SDNetGetModule(const SSGDPVersion *pstVersion)
{
	// begin
	// 2009-09-18 cwy add : version check
	//
// 	if (false == VerCheck(pstVersion, &SDNET_VERSION))
// 	{
// 		return NULL;
// 	}
	// end

	if(NULL == CSDNetWin::Instance())
	{
		WSADATA data;
		WSAStartup(MAKEWORD(2, 2), &data);

		if(false == CSDNetWin::CreateInstance())
		{
			return NULL;
		}

		if(false == CSDNetWin::Instance()->Init())
		{
			CRITICAL("SDNetGetModule, Init CSDNetWin failed");
			CSDNetWin::DestroyInstance();
			return NULL;
		}
	}

    CSDNetWin::Instance()->AddRef();

	return CSDNetWin::Instance();
}

//////////////////////////////////////////////////////////////////////////////////

IMPLEMENT_SINGLETON(CSDNetWin)

void SDAPI Crown::SDNetSetOpt(uint32 dwType, void* pOpt)
{
	switch(dwType)
	{
	case NETWIN_OPT_MAX_CONNECTION:
		{
			SNetWinOptMaxConnection* pstConn = (SNetWinOptMaxConnection*)pOpt;
			MAX_CONNECTION = pstConn->nMaxConnection;
		}
		break;
	case NETWIN_OPT_QUEUE_SIZE:
		{
			SNetWinOptQueueSize* pstQueue = (SNetWinOptQueueSize*)pOpt;
			if(pstQueue->nRecvBufSize != -1)
			{
				RECV_LOOP_BUF_SIZE = pstQueue->nRecvBufSize;
			}
			if(pstQueue->nEventQueueSize != -1)
			{
				MAX_NET_EVENT = pstQueue->nEventQueueSize;
			}
		}
		break;
	case NETWIN_OPT_ADVANCE_PARAM:
		{
			SNetWinOptAdvanceParam* pstParam = (SNetWinOptAdvanceParam*)pOpt;
			if(pstParam->nParam1 != -1)
			{
				DELAY_RELEASE_CPSOCK_TIME = pstParam->nParam1;
			}
		}
		break;
	default:
		break;
	}
}

CSDNetWin::CSDNetWin()
{
	m_dwRef				= 0;
	m_dwNextConnectorID = 0;
}

CSDNetWin::~CSDNetWin()
{

}

void CSDNetWin::AddRef(void)
{
	m_dwRef++;
}

uint32 CSDNetWin::QueryRef(void)
{
	return m_dwRef;
}

void CSDNetWin::Release(void)
{
	if(m_dwRef <= 1)
	{
		Uninit();
        DestroyInstance();
		return;
	}
	m_dwRef--;
}

SSGDPVersion CSDNetWin::GetVersion(void)
{
	return SDNET_VERSION;
}

const char * CSDNetWin::GetModuleName(void)
{
	return SDNET_MODULENAME;
}

ISDConnector* CSDNetWin::CreateConnector(uint32 dwNetIOType)
{
	if(dwNetIOType != NETIO_COMPLETIONPORT)
	{
		return NULL;
	}

	if((int32)m_oMapConnector.size() >= MAX_CONNECTION)
	{
		WARN("CSDNetWin::CreateConnector, Max connector reached");
		return NULL;
	}

	CSDConnector* poConnector = CnNew CSDConnector;
	if(NULL == poConnector)
	{
		CRITICAL("CSDNetWin::CreateConnector, new CSDConnector failed");
		return NULL;
	}

	poConnector->SetID(++m_dwNextConnectorID);
	m_oMapConnector[m_dwNextConnectorID] = poConnector;
	
	return poConnector;
}

ISDListener* CSDNetWin::CreateListener(uint32 dwNetIOType)
{
    if (dwNetIOType == NETIO_COMPLETIONPORT)
    {
	    CSDListener* poListener = CnNew CSDListener;
	    if(NULL == poListener)
	    {
		    CRITICAL("CSDNetWin::CreateListener, new CSDListener failed");
		    return NULL;
	    }

	    return poListener;
    }
	//else if (NETIO_COMPLETIONPORT_GATE == dwNetIOType)
	//{
	//	CSDListener* poNomalListener = new CSDListener;
	//	if(NULL == poNomalListener)
	//	{
	//		CRITICAL("CSDNetWin::CreateListener, new CSDListener failed");
	//		return NULL;
	//	}

	//	CSDGateListener* poGateListener = new CSDGateListener;
	//	if(NULL == poGateListener)
	//	{
	//		CRITICAL("CSDNetWin::CreateListener, new CSDGateListener failed");
	//		delete poNomalListener;
	//		return NULL;
	//	}

	//	if(false == poGateListener->Init(poNomalListener))
	//	{
	//		delete poGateListener;
	//		delete poNomalListener;
	//		return NULL;
	//	}
	//	
	//	return poGateListener;
	//}
    else
    {
        return NULL;
    }
}

void CSDNetWin::ReleaseConnector(CSDConnector* poConnector)
{
	if(NULL == poConnector)
	{
		return;
	}

	m_oMapConnector.erase(poConnector->GetID());
	CnDelete poConnector;
}

CSDConnector* CSDNetWin::FindConnector(uint32 dwID)
{
	CMapConnector::iterator it = m_oMapConnector.find(dwID);
	if(it == m_oMapConnector.end())
	{
		return NULL;
	}

	return it->second;
}

bool CSDNetWin::Init()
{
	if(false == _CreateComponent())
	{
		return false;
	}

	if(false == _InitComponent())
	{
		return false;
	}

	return true;
}

void CSDNetWin::Uninit()
{
	_UninitComponent();
	_DesroryComponent();
}

bool CSDNetWin::_CreateComponent()
{

	/*
	if (false == SysSockMgr::CreateInstance() )
	{
		CRITICAL("CSDNetWin::_CreateComponent, SysSockMgr::CreateInstance failed");
		return false;
	}
	*/
	if(false == CEventMgr::CreateInstance())
	{
		CRITICAL("CSDNetWin::_CreateComponent, CEventMgr::CreateInstance failed");
		return false;
	}

	if (false == CConnDataMgr::CreateInstance())
	{
		CRITICAL("CSDNetWin::_CreateComponent, CConnDataMgr::CreateInstance failed");
		return false;
	}
	/*
	if(false == CCPSockMgr::CreateInstance())
	{
		CRITICAL("CSDNetWin::_CreateComponent, CCPSockMgr::CreateInstance failed");
		return false;
	}
	if(false == CConnectionMgr::CreateInstance())
	{
		CRITICAL("CSDNetWin::_CreateComponent, CConnectionMgr::CreateInstance failed");
		return false;
	}
	*/
	if(false == CConnectCtrl::CreateInstance())
	{
		CRITICAL("CSDNetWin::_CreateComponent, CConnectCtrl::CreateInstance failed");
	}
	if(false == CIocpCtrl::CreateInstance())
	{
		CRITICAL("CSDNetWin::_CreateComponent, CIocpCtrl::CreateInstance failed");
		return false;
	}
	
    m_dwLastTick = GetTickCount();
	return true;
}

bool CSDNetWin::_InitComponent()
{
	//m_pRecvBuf = new char[MAX_PKG_LEN];
	//if(NULL == m_pRecvBuf)
	//{
	//	CRITICAL("%s, new RecvBuf failed, len %d", __FUNCTION__, MAX_PKG_LEN);
	//	return false;
	//}
	

	/*
	if(false == m_oRecvQueue.Init(RECV_LOOP_BUF_SIZE))
	{
		CRITICAL("%s, Init RecvQueue failed, size %d", __FUNCTION__, RECV_LOOP_BUF_SIZE);
		return false;
	}
	*/


	/*
	if (false == SysSockMgr::Instance()->Init())
	{
		CRITICAL("CSDNetWin::_InitComponent, Init SysSockMgr  failed" );
		return false;
	}
	*/

	if (false == CConnDataMgr::Instance()->Init())
	{
		CRITICAL("CSDNetWin::_InitComponent, Init CConnDataMgr failed" );
		return false;
	}
	if(false == CEventMgr::Instance()->Init(MAX_NET_EVENT))
	{
		CRITICAL("CSDNetWin::_InitComponent, Init CEventMgr %d failed", MAX_NET_EVENT);
		return false;
	}

	/*
	if(false == CCPSockMgr::Instance()->Init(MAX_CONNECTION))
	{
		CRITICAL("CSDNetWin::_InitComponent, Init CCPSockMgr %d failed", MAX_CONNECTION);
		return false;
	}

	if(false == CConnectionMgr::Instance()->Init(MAX_CONNECTION))
	{
		CRITICAL("CSDNetWin::_InitComponent, Init CConnectionMgr %d failed", MAX_CONNECTION);
		return false;
	}

	*/
	if(false == CConnectCtrl::Instance()->Init())
	{
		CRITICAL("CSDNetWin::_InitComponent, Init CConnectCtrl failed");
		return false;
	}

	if(false == CIocpCtrl::Instance()->Init())
	{
		CRITICAL("CSDNetWin::_InitComponent, Init CIocpCtrl failed");
		return false;
	}

	return true;
}

void CSDNetWin::_UninitComponent()
{
	CIocpCtrl::Instance()->Uninit();
	CConnectCtrl::Instance()->Uninit();
	CConnDataMgr::Instance()->Uninit(); 

	//SysSockMgr::Instance()->Uninit(); 
	CEventMgr::Instance()->Uninit();

	//if(m_pRecvBuf != NULL)
	//{
	//	delete m_pRecvBuf;
	//	m_pRecvBuf = NULL;
	//}

	_ClearConnector();
}

void CSDNetWin::_DesroryComponent()
{
	CIocpCtrl::DestroyInstance();
	CConnectCtrl::DestroyInstance();
	CConnDataMgr::DestroyInstance(); 
	//SysSockMgr::DestroyInstance(); 
//	CConnectionMgr::DestroyInstance();
//	CCPSockMgr::DestroyInstance();
	CEventMgr::DestroyInstance();
}

void CSDNetWin::_ClearConnector()
{
	for(CMapConnector::iterator it = m_oMapConnector.begin(); it != m_oMapConnector.end(); ++it)
	{
		CnDelete it->second;
	}

	m_oMapConnector.clear();
}

bool CSDNetWin::Run(int32 nCount)
{
	do
	{
// #ifdef SDNET_HAS_GATHER_SEND       
// #pragma message("[preconfig]sdnet collect buffer, has a internal timer")        
//         if (m_pTimerModule)        
//         {
//             m_pTimerModule->Run();
//         }        
// #endif
#ifdef SDNET_HAS_GATHER_SEND 
        static int32 sendCnt = 0;
        ++sendCnt;
        if (sendCnt == 10)
        {
            sendCnt = 0;
            uint32 now = GetTickCount();
            if (now < m_dwLastTick)
            {
                /// 溢出了,发生了数据回绕 \///
                m_dwLastTick = now;
            }

            if ((now - m_dwLastTick) > 50)
            {
                m_dwLastTick = now;            
                FlushBufferedData();
            }
        }       
#endif // 
		//SNetEvent stEvent; 
		SNetEvent *pstEvent  = CEventMgr::Instance()->PopFrontNetEvt();
		if (pstEvent == NULL)
		{
			return false;
		}
		SNetEvent & stEvent = *pstEvent; 
		
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
		case NETEVT_ASSOCIATE:
			_ProcAssociateEvt(&stEvent.stUn.stAssociate);
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
		case NETEVT_BIND_ERR:
			_ProcBindErrEvt(&stEvent.stUn.stBindErr);
			break;
		default:
			CnVerify(false);
			break;
		}
		CEventMgr::Instance()->ReleaseNetEvt(pstEvent); 
	}while(--nCount != 0);

	return true;
}

void CSDNetWin::_ProcEstablishEvt(SEstablishEvt* pstEvent)
{
	CConnData* pConnData = pstEvent->pConnData;
	CnVerify(pConnData != NULL);
	pConnData->connection.OnConnect();
}

void CSDNetWin::_ProcAssociateEvt(SAssociateEvt* pstEvent)
{
	CConnData* pConnData = pstEvent->pConnData;
	CnVerify(pConnData != NULL);

#ifdef SDNET_HAS_GATHER_SEND 
	m_connSockets.push_back(&pConnData->sock);
#endif //

	if(pConnData->connId != pstEvent->dwConnectionID)
	{
		return;
	}

	pConnData->connection.OnAssociate();
}

void CSDNetWin::_ProcConnErrEvt(SConnErrEvt* pstEvent)
{
	CSDConnector* poConnector = FindConnector(pstEvent->dwConnectorID);
	if(NULL == poConnector)
	{
		return;
	}

	poConnector->OnConnectErr(pstEvent->nSysErrCode);
}

//// 2009-03-24 cwy add for interface expanding, add bind function
void CSDNetWin::_ProcBindErrEvt( SBindErrEvt* pstEvent )
{
	CSDConnector* poConnector = FindConnector(pstEvent->dwConnectorID);
	if(NULL == poConnector)
	{
		return;
	}
	poConnector->OnBindErr(pstEvent->nSDErrCode, pstEvent->nSysErrCode);
}

void CSDNetWin::_ProcErrorEvt(SErrorEvt* pstEvent)
{
	CConnData* pConnData = pstEvent->pConnData;
	CnVerify(pConnData != NULL);
	if(pConnData->connId != pstEvent->dwConnectionID)
	{
		return;
	}

	pConnData->connection.OnError(pstEvent->nSDErrCode, pstEvent->nSysErrCode);
}

void CSDNetWin::_ProcTerminateEvt(STerminateEvt* pstEvent)
{
	CConnData* pConnData = pstEvent->pConnData;
	CnVerify(pConnData != NULL);

#ifdef SDNET_HAS_GATHER_SEND 
	for (ConnectedSockets::iterator itr = m_connSockets.begin();
		itr != m_connSockets.end(); ++itr)
	{
		if ( (*itr) == &pConnData->sock)
		{
			m_connSockets.erase(itr);
			break;
		}
	}
#endif 

	pConnData->connection.OnClose();
}

void CSDNetWin::ProcRecvData(CConnData * pConnData, const char * pData, int len)
{
	CnVerify(pConnData != NULL);
	pConnData->connection.OnRecv(pData, len);
}

void CSDNetWin::_ProcRecvEvt(SRecvEvt* pstEvent)
{
	char * pRecvBuf = NULL;
	CConnData* pConnData = pstEvent->pConnData;
	CnVerify(pConnData != NULL);

	/*
	if(false == m_oRecvQueue.PopFront(m_pRecvBuf, pstEvent->nLen))
	{
		CRITICAL("%s, PopFront RecvData Len %d failed", __FUNCTION__, pstEvent->nLen);
		return;
	}
	*/

	if(pConnData->connId != pstEvent->dwConnectionID)
	{
		return;
	}
	
	pstEvent->pLoopBuf->PopFront(pRecvBuf, pstEvent->nLen);

	pConnData->connection.OnRecv(pRecvBuf, pstEvent->nLen);
}

void CSDNetWin::_ProcSendEvt(SSendEvt * pstEvent)
{
	CConnData* pConnData = pstEvent->pConnData;
	CnVerify(pConnData != NULL);
	pConnData->connection.OnSend(pstEvent->pBuf, pstEvent->dwLen);
}
 
