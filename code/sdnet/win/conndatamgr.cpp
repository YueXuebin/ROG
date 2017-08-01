#include "SDnetPCH.h"
#include "conndataallocator.h"
//
#include "eventmgr.h"
CConnDataMgr * CConnDataMgr::s_instance = NULL; 

IMPLEMENT_SINGLETON(CConnDataMgr); 

CConnData::CConnData(int32 recvBufSize , int32 sendBufSize   )
{

	connection.SetConnData(this);
	connection.SetID(connId);

	sock.AttachRecvBuf(recvBuf,recvBufSize); 
	sock.AttachSendBuf(sendBuf,sendBufSize); 
	sock.SetConnData(this); 
}

CConnData::~CConnData()
{
	char* pRecvBuf			= NULL;
	char* pSendBuf			= NULL;
	uint32 dwRecvBufSize	= sock.DetachRecvBuf(&pRecvBuf);
	uint32 dwSendBufSize	= sock.DetachSendBuf(&pSendBuf); 
}

bool CConnDataMgr::Init()
{
	m_connDataAllocator = CnNew ConnDataAllocator(); 
	m_initState  = 1; 
	return true; 
}

void CConnDataMgr::Uninit()
{
	m_initState = -1; 
	CConnData * pFakeData = m_connDataAllocator->allocate(1,1); 
	m_connDataAllocator->deallocate(pFakeData,1,1); 
	CnDelete m_connDataAllocator; 
}

CConnData * CConnDataMgr::Alloc(uint32 recvBufSize, uint32 sendBufSize)
{
	CConnData *pConnData = m_connDataAllocator->allocate( recvBufSize,  sendBufSize); 	 
	return new (pConnData)CConnData(recvBufSize,sendBufSize);	// placement new
}

bool CConnDataMgr::Find(CCPSock * pSock)
{
	return m_connDataAllocator->find(pSock); 
}

void CConnDataMgr::Release(CConnData * pConnData)
{	 
	uint32 sendBufSize = pConnData->sock.GetSendBufSize(); 
	uint32 recvBufSize = pConnData->sock.GetRecvBufSize(); 
	pConnData->~CConnData(); 
	m_connDataAllocator->deallocate(pConnData,recvBufSize,sendBufSize); 
}

void CConnDataMgr::ReleaseUsedConnData()
{
	UsedConnDataList::iterator itr = m_usedConnData.begin();
	while( itr != m_usedConnData.end() )
	{
		CConnData* pConnData = *itr;
		pConnData->connection.Disconnect();
		Release(pConnData);
		itr = m_usedConnData.begin();
	}
	m_usedConnData.clear(); 
}

/*
void CConnDataMgr::DelayRelease(CCPSock * pSock)
{
	m_delayLock.Lock(); 
	m_delaySockList.push_back(pSock); 
	m_delayLock.Unlock(); 
}
void CConnDataMgr::RemoveDelaySock(CCPSock * pSock)
{
	m_delayLock.Lock(); 

	for(std::list<CCPSock * > ::iterator it = m_delaySockList.begin(); it != m_delaySockList.end();)
	{
		if (*it == pSock)
		{
			m_delaySockList.erase(it); 
			break; 
		}
	}
	m_delayLock.Unlock(); 
}

void CConnDataMgr::CheckDelaySock()
{
	time_t nNow = time(NULL);
	if(nNow - m_nLastCheckTime < 1)
		return;

	m_nLastCheckTime = nNow;

	m_delayLock.Lock();
	for(std::list<CCPSock * > ::iterator it = m_delaySockList.begin(); it != m_delaySockList.end();)
	{
		CCPSock* poSock = *it;

		if ((poSock->GetPostRecv() > 0)&&(0 == poSock->GetPostSend()))
		{
			
			if(poSock->GetSock() != INVALID_SOCKET)
			{
				CancelIo((HANDLE)poSock->GetSock());
				closesocket(poSock->GetSock());
				CEventMgr::Instance()->PushTerminateEvt(poSock->GetConnData(), poSock->GetConnectionID());
				poSock->SetSock(INVALID_SOCKET);
			}
		}

		DBG("[%s:%d]ConnID=%d, PostRecvCnt=%d, PostSendCnt=%d",
			MSG_MARK, poSock->GetConnectionID(), poSock->GetPostRecv(), poSock->GetPostSend());

		if ( ( (0 == poSock->GetPostRecv()) &&  (0 == poSock->GetPostSend()) ) )
		{
			CEventMgr::Instance()->PushTerminateEvt(poSock->GetConnData(), poSock->GetConnectionID());
			this->m_delaySockList.erase(it); 
			continue;
		}

		++it;
	}
	m_delayLock.Unlock();


}
*/
