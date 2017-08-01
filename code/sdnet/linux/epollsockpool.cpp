#include "SDnetPCH.h"
#include "sdnetconfig.h"
#include "epollsockpool.h"

CEpollSockPool::CEpollSockPool(void)
{
	m_pSockArray	= NULL;
}

CEpollSockPool::~CEpollSockPool(void)
{
}

bool CEpollSockPool::Init(int32 nMax)
{
	if(0 == nMax)
		return false;

	m_pSockArray = CnNew CEpollSock[nMax];
	if(NULL == m_pSockArray)
	{
		CRITICAL("CEpollSockPool::Init, new CEpollSock %d failed", nMax);
		return false;
	}
    
	for(int32 i = 0; i < nMax; i++)
		m_oFreeList.push_back(&m_pSockArray[i]);

	return true;
}

void CEpollSockPool::Uninit()
{
	if(m_pSockArray != NULL)
	{
		CnDelete [] m_pSockArray;
		m_pSockArray = NULL;
	}
	
	m_oFreeList.clear();
}

CEpollSock* CEpollSockPool::Create()
{
	CEpollSock* poSock = NULL;
	if(!m_oFreeList.empty())
	{
		poSock = m_oFreeList.front();
		m_oFreeList.pop_front();
		poSock->Reset();
	}
		
	return poSock;
}

void CEpollSockPool::Release(CEpollSock* poSock)
{
	if(NULL == poSock)
		return;

	m_oFreeList.push_back(poSock);
}
