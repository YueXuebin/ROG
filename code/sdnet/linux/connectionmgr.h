#ifndef CONNECTIONMGR_H
#define CONNECTIONMGR_H

#include <pthread.h>
#include "sdconnection.h"

#define CONNECTION_SLOT_SIZE	65536

class CConnectionMgr
{
	CConnectionMgr(void);
	virtual ~CConnectionMgr(void);

public:
	static bool CreateInstance()
	{
		if(NULL == m_poInstance)
		{
			m_poInstance = CnNew CConnectionMgr;
		}

		return m_poInstance != NULL;
	}
	static CConnectionMgr * Instance(){ return m_poInstance; }
	static void DestoryInstance()
	{
		if(m_poInstance != NULL)
		{
			CnDelete m_poInstance;
			m_poInstance = NULL;
		}
	}

	bool Init(int32 nMax);

	CSDConnection* Create();
	void Release(CSDConnection* poConnection);
	CSDConnection* Find(uint32 dwID);

protected:
	void _Lock(){ pthread_mutex_lock(&m_stLock); }
	void _Unlock() { pthread_mutex_unlock(&m_stLock); }
	CSDConnection* _Fetch();
	void _Release(CSDConnection* poConnection);

protected:
	static CConnectionMgr*	m_poInstance;

	CSDConnection*			m_apoConnectionSlot[CONNECTION_SLOT_SIZE];
	uint16					m_awConnectionIndex[CONNECTION_SLOT_SIZE];
	uint16					m_wNextSlot;

	int32					m_nMaxConnection;
	CSDConnection*			m_pConnectionArray;
	CListConnection			m_oFreeList;
	pthread_mutex_t			m_stLock;
};

#endif
