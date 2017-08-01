#include "SDnetPCH.h"
#ifdef WIN
#include <Winsock2.h>
#endif
#include "conndataallocator.h"


CBufPool* ConnDataAllocator::FindBufPool(uint32 dwSize, bool bCreate)
{
	CMapBufPool::iterator it = m_mapBufPool.find(dwSize);
	if(it != m_mapBufPool.end())
	{
		return it->second;
	}

	if(false == bCreate)
	{
		return NULL;
	}

	CBufPool* poBufPool = CnNew CBufPool;
	if(NULL == poBufPool)
	{
		CRITICAL("CCPSockMgr::_FindBufPool, new CBufPool failed");
		return NULL;
	}
	poBufPool->Init(dwSize);
	m_mapBufPool[dwSize] = poBufPool;

	return poBufPool;
}

void ConnDataAllocator::ReleaseBufPool(char * pBuf, uint32 dwSize)
{
	CBufPool * pBufPool = FindBufPool(dwSize, false); 

	if (pBufPool != NULL)
	{
		pBufPool->Release(pBuf); 
	}
}
