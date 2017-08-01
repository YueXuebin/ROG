#include "SDnetPCH.h"
#include "sdnetconfig.h"
#include "connectionmgr.h"

CConnectionMgr * CConnectionMgr::m_poInstance = NULL;

CConnectionMgr::CConnectionMgr(void)
{
	m_wNextSlot = 0;

	for(int32 i = 0; i < CONNECTION_SLOT_SIZE; i++)
		m_apoConnectionSlot[i] = NULL;

	memset(m_awConnectionIndex, '\0', sizeof(m_awConnectionIndex));

	m_nMaxConnection	= 0;
	m_pConnectionArray	= NULL;

	pthread_mutex_init(&m_stLock, NULL);
}

CConnectionMgr::~CConnectionMgr(void)
{
	if(m_pConnectionArray != NULL)
		CnDelete [] m_pConnectionArray;
	
	pthread_mutex_destroy(&m_stLock);
}

bool CConnectionMgr::Init(int32 nMax)
{
	m_pConnectionArray = CnNew CSDConnection[nMax];
	if(NULL == m_pConnectionArray)
	{
		CRITICAL("CConnectionMgr::Init, new CSDConnection[%d] failed", nMax);
		return false;
	}

	m_nMaxConnection = nMax;
	for(int32 i = 0; i < nMax; i++)
		m_oFreeList.push_back(&m_pConnectionArray[i]);

	return true;
}

CSDConnection * CConnectionMgr::CConnectionMgr::Create()
{
	CSDConnection* poConnection = _Fetch();
	if(NULL == poConnection)
		return NULL;

	poConnection->Reset();

	for(int32 i = 0; i < CONNECTION_SLOT_SIZE; i++)
	{
		if(m_apoConnectionSlot[m_wNextSlot] != NULL)
		{
			m_wNextSlot++;
			continue;
		}

		m_awConnectionIndex[m_wNextSlot]++;
		if(0 == m_awConnectionIndex[m_wNextSlot])
			m_awConnectionIndex[m_wNextSlot] = 1;

		poConnection->SetID( (((uint32)m_wNextSlot)<<16) | m_awConnectionIndex[m_wNextSlot] );
		m_apoConnectionSlot[m_wNextSlot] = poConnection;
		m_wNextSlot++;

		return poConnection;
	}

	CRITICAL("%s, can not find ConnectionSlot", __FUNCTION__);
	_Release(poConnection);
	return NULL;
}

void CConnectionMgr::Release(CSDConnection* poConnection)
{
	if(NULL == poConnection)
		return;

	uint32 dwSlot = poConnection->GetID();
	dwSlot = dwSlot >> 16;
	if(m_apoConnectionSlot[dwSlot] == poConnection)
		m_apoConnectionSlot[dwSlot] = NULL;

	_Release(poConnection);
}

CSDConnection* CConnectionMgr::Find(uint32 dwID)
{
	CSDConnection* poConnection = m_apoConnectionSlot[dwID>>16];
	if(NULL == poConnection)
		return NULL;
	if(dwID != poConnection->GetID())
		return NULL;

	return poConnection;
}

CSDConnection* CConnectionMgr::_Fetch()
{
	CSDConnection* poConnection = NULL;

	_Lock();
	if(!m_oFreeList.empty())
	{
		poConnection = m_oFreeList.front();
		m_oFreeList.pop_front();
	}

	DBG("CConnectionMgr::_Fetch, Free Count %d", m_oFreeList.size());
	_Unlock();

	return poConnection;
}

void CConnectionMgr::_Release(CSDConnection* poConnection)
{
	CnVerify(poConnection != NULL);

	_Lock();
	m_oFreeList.push_back(poConnection);
	DBG("CConnectionMgr::_Release, Free Count %d", m_oFreeList.size());
	_Unlock();
}
