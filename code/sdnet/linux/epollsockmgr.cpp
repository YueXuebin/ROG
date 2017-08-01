#include "SDnetPCH.h"
#include "sdnetconfig.h"
#include "epollsockmgr.h"

CEpollSockMgr::CEpollSockMgr(void)
{
	m_wNextSlot = 0;
	
	for(int32 i = 0; i < EPOLLSOCK_SLOT_SIZE; i++)
		m_apoSockSlot[i] = NULL;

	memset(m_awSockIndex, '\0', sizeof(m_awSockIndex));
}

CEpollSockMgr::~CEpollSockMgr(void)
{
}

bool CEpollSockMgr::Init(int32 nMax)
{
	if(false == m_oSockPool.Init(nMax))
		return false;

	return true;
}

void CEpollSockMgr::Uninit()
{
	for(int32 i = 0; i < EPOLLSOCK_SLOT_SIZE; i++)
	{
		CEpollSock* poSock = m_apoSockSlot[i];
		if(poSock != NULL)
		{
			poSock->Close(false);
			Release(poSock);
		}
	}

	m_oSockPool.Uninit();
	
	for(CMapBufPool::iterator it = m_oMapBufPool.begin(); it != m_oMapBufPool.end(); ++it)
	{
		CnDelete it->second;
	}
	m_oMapBufPool.clear();
}

CEpollSock* CEpollSockMgr::Create(uint32 dwRecvBufSize, uint32 dwSendBufSize)
{
	CEpollSock* poSock = m_oSockPool.Create();
	if(NULL == poSock)
	{
		CRITICAL("CEpollSockMgr::Create, create CEpollSock failed");
		return NULL;
	}
	
	//为Sock分配发送和接收缓冲
	char* pRecvBuf = NULL;
	char* pSendBuf = NULL;
	CBufPool* poSendBufPool = NULL;
	CBufPool* poRecvBufPool = _FindBufPool(dwRecvBufSize);

	if(NULL == poRecvBufPool)
	{
		CRITICAL("CEpollSockMgr::Create, FindBufPool Recv size %u failed", dwRecvBufSize);
		goto fail_ret;
	}
	pRecvBuf = poRecvBufPool->Create();
	if(NULL == pRecvBuf)
	{
		CRITICAL("CEpollSockMgr::Create, Create RecvBuf failed, size %u", dwRecvBufSize);
		goto fail_ret;
	}

	//当发送缓冲大于系统所提供的最大缓冲时才进行实际发送缓冲分配
	if(dwSendBufSize > MAX_SYS_SEND_BUF)
	{
		poSendBufPool = _FindBufPool(dwSendBufSize);
		if(NULL == poSendBufPool)
		{
			CRITICAL("CEpollSockMgr::Create, FindBufPool Send size %u failed", dwSendBufSize);
			goto fail_ret;
		}
		pSendBuf = poSendBufPool->Create();
		if(NULL == pSendBuf)
		{
			CRITICAL("CEpollSockMgr::Create, Create SendBuf failed, size %u", dwSendBufSize);
			goto fail_ret;
		}
	}

	for(int32 i = 0; i < EPOLLSOCK_SLOT_SIZE; i++)
	{
		if(m_apoSockSlot[m_wNextSlot] != NULL)
		{
			m_wNextSlot++;
			continue;
		}
		
		m_awSockIndex[m_wNextSlot]++;
		if(0 == m_awSockIndex[m_wNextSlot])
			m_awSockIndex[m_wNextSlot] = 1;

		poSock->SetID( (((uint32)m_wNextSlot)<<16) | m_awSockIndex[m_wNextSlot] );
		m_apoSockSlot[m_wNextSlot] = poSock;
		m_wNextSlot++;
		
		poSock->AttachRecvBuf(pRecvBuf, dwRecvBufSize);
		poSock->AttachSendBuf(pSendBuf, dwSendBufSize);
		return poSock;
	}
	CRITICAL("CEpollSockMgr::Create, can not find EpollSockSlot");

fail_ret:
	if(pRecvBuf != NULL)
		poRecvBufPool->Release(pRecvBuf);

	if(pSendBuf != NULL)
		poSendBufPool->Release(pSendBuf);

	m_oSockPool.Release(poSock);
	return NULL;
}

void CEpollSockMgr::Release(CEpollSock* poSock)
{
	if(NULL == poSock)
		return;

	char* pRecvBuf			= NULL;
	char* pSendBuf			= NULL;
	uint32 dwRecvBufSize	= poSock->DetachRecvBuf(&pRecvBuf);
	uint32 dwSendBufSize	= poSock->DetachSendBuf(&pSendBuf);

	//释放RecvBuf
	if(pRecvBuf != NULL)
	{
		CBufPool* poRecvBufPool = _FindBufPool(dwRecvBufSize, false);
		if(poRecvBufPool != NULL)
		{
			poRecvBufPool->Release(pRecvBuf);
		}
		else
		{
			CnVerify(false);
			CnDelete pRecvBuf;
		}
	}
	else
	{
		CnVerify(pRecvBuf != NULL);
	}

	//释放SendBuf
	if(pSendBuf != NULL)
	{
		CBufPool* poSendBufPool = _FindBufPool(dwSendBufSize, false);
		if(poSendBufPool != NULL)
		{
			poSendBufPool->Release(pSendBuf);
		}
		else
		{
			CnVerify(false);
			CnDelete pSendBuf;
		}
	}

	uint32 dwSlot = poSock->GetID();
	dwSlot = dwSlot >> 16;
	if(m_apoSockSlot[dwSlot] == poSock)
		m_apoSockSlot[dwSlot] = NULL;

	m_oSockPool.Release(poSock);
}

CEpollSock* CEpollSockMgr::Find(uint32 dwID)
{
	CEpollSock* poSock = m_apoSockSlot[dwID>>16];
	if(NULL == poSock)
		return NULL;
	if(dwID != poSock->GetID())
		return NULL;

	return poSock;
}

CBufPool* CEpollSockMgr::_FindBufPool(uint32 dwSize, bool bCreate)
{
	CMapBufPool::iterator it = m_oMapBufPool.find(dwSize);
	if(it != m_oMapBufPool.end())
		return it->second;

	if(false == bCreate)
		return NULL;

	CBufPool* poBufPool = CnNew CBufPool;
	if(NULL == poBufPool)
	{
		CRITICAL("CEpollSockMgr::_FindBufPool, new CBufPool failed");
		return NULL;
	}
	poBufPool->Init(dwSize); 

	m_oMapBufPool[dwSize] = poBufPool;

	return poBufPool;
}
