#include "transfer_pch.h"
#include "fake_network_pipe.h"

FakeNetWorkPipe::FakeNetWorkPipe() :
m_PushThreadId(0),
m_PopThreadId(0)
{

}

FakeNetWorkPipe::~FakeNetWorkPipe()
{

}

void FakeNetWorkPipe::Send(void* pData, uint32 len)
{
#if defined(_DEBUG) && defined(WIN)
	if( !m_PushThreadId )
		m_PushThreadId = GetCurrentThreadId();
	CnVerify(m_PushThreadId == GetCurrentThreadId());
#endif
	Crown::CSDLock<Crown::CSDMutex> lock(m_mutex);

	if( !m_FreeListHeader )
	{	// FreeList中没有元素了
		Grow();
	}

	Crown::_PoolMemLinkNode<NetPack>* pCurNode = m_FreeListHeader;
	// 把该元素从FreeList中移出
	m_FreeListHeader = m_FreeListHeader->pNext;

	pCurNode->unlink();

	// 把该元素加入到BusyList中
	pCurNode->pNext = m_BusyListHeader;
	if(m_BusyListHeader)
	{
		m_BusyListHeader->pPrev = pCurNode;		
	}
	else
	{
		CnVerify(!m_BusyListTailor);
		m_BusyListTailor = pCurNode;
	}
	m_BusyListHeader = pCurNode;

	// 赋值
	if (len > MAX_PACK_LEN)
	{	// 数据包长度超过上限，暂不支持分包发送
		CnVerify(false);
		len = MAX_PACK_LEN;
	}
    memcpy(pCurNode->Val().pData, pData, len);
    pCurNode->Val().len = len;
	int delayTime = int( (real)rand() / RAND_MAX * (NETPACK_DELAY_MAX - NETPACK_DELAY_MIN) + NETPACK_DELAY_MIN );
    pCurNode->Val().RevcTime = Crown::SDGetTickCount() + delayTime;		// 接收时间

}

uint32 FakeNetWorkPipe::Recv(void* pData, uint32 len)
{
#if defined(_DEBUG) && defined(WIN)
	if( !m_PopThreadId )
		m_PopThreadId = GetCurrentThreadId();
	CnVerify(m_PopThreadId == GetCurrentThreadId());
#endif
	Crown::CSDLock<Crown::CSDMutex> lock(m_mutex);

	if( !m_BusyListTailor )
		return false;

    NetPack* pNetPack = &m_BusyListTailor->Val();
	if( pNetPack->RevcTime < Crown::SDGetTickCount() )
	{	// 取走该数据包
		Crown::_PoolMemLinkNode<NetPack>* pCurNode = m_BusyListTailor;		// 该Node从BusyList的末端取走
		if(pCurNode == m_BusyListHeader)
		{
			CnVerify(m_BusyListTailor->pNext == nullptr && m_BusyListTailor->pPrev == nullptr);
			m_BusyListHeader = nullptr;		
		}
		m_BusyListTailor = pCurNode->pPrev;	

		pCurNode->unlink();

		// 把该Node加入到FreeList中
		pCurNode->pNext = m_FreeListHeader;
		if(m_FreeListHeader)
			m_FreeListHeader->pPrev = pCurNode;
		m_FreeListHeader = pCurNode;

		// 取值
		uint32 recvLen = len > pNetPack->len ? pNetPack->len : len;
		memcpy(pData, pNetPack->pData, pNetPack->len);
		return recvLen;
	}
	else
	{	// 还未到取走该数据包的时间
		return 0;
	}
}
