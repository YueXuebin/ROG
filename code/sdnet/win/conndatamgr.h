#pragma once

#ifndef _CONNDATAMGR_H_
#define _CONNDATAMGR_H_

#include "sdsingleton.h"
#ifdef WIN
#include "cpsock.h"
#else 
#include "epollsock.h"
#endif
 
#include "sdobjectpool.h"
#include <vector>
#include <set>
#include <list>
#include <time.h>
#include "bufpool.h"

#include "sdconnection.h"
class ConnDataAllocator;
class ConnDataAllocator ;

#ifndef WIN
	typedef CEpollSock CCPSock; 
#endif 
 

struct CConnData 
{
	CConnData(int32 recvBufSize =DEFAULT_RECVBUF_SIZE , int32 sendBufSize  =DEFAULT_SENDBUF_SIZE ); 
	
	~CConnData(); 

	// don't modify below values in your application 
	CSDConnection connection; 
	CCPSock sock; 
	char * sendBuf; 
	char * recvBuf; 	 
 
	uint32 connId; 
};



class CConnDataMgr
{
	CConnDataMgr()
	{
		m_connDataAllocator = NULL; 
		 
		m_initState = 0 ; 
	}
	~CConnDataMgr()
	{
	};
	typedef std::vector<CConnData*> UsedConnDataList; 

public:
	DECLARE_SINGLETON(CConnDataMgr); 

	bool Init(); 

	void Uninit(); 

	CConnData * Alloc(uint32 recvBufSize, uint32 sendBufSize); 

	bool Find(CCPSock * pSock); 

    void Release(CConnData * pConnData); 
	/*
	void DelayRelease(CCPSock * pSock); 
	void RemoveDelaySock(CCPSock * pSock); 
	void CheckDelaySock(); 
	*/
private:

	inline void AddUsedConnData(CConnData * pConnData)
	{
		m_usedConnData.push_back(pConnData); 
	}
	void RemoveUsedConnData(CConnData * pConnData)
	{
		for (UsedConnDataList::iterator itr = m_usedConnData.begin(); itr != m_usedConnData.end(); ++itr)
		{
			if (pConnData == (*itr))
			{
				m_usedConnData.erase(itr); 
				return ; 
			}
		}
	}

	bool FindUsedConnSock(CCPSock* pSock)
	{
		for (UsedConnDataList::iterator itr = m_usedConnData.begin(); itr != m_usedConnData.end(); ++itr)
		{
			if (pSock == &((*itr)->sock))
			{
				return true; 
			}
		}
		return false; 
	}
	


	
	friend class ConnDataAllocator; 
	void ReleaseUsedConnData(); 	
 
	ConnDataAllocator * m_connDataAllocator; 	

	static CConnDataMgr * s_instance; 

	UsedConnDataList m_usedConnData; 

	std::list<CCPSock * >  m_delaySockList; 

	CSDMutex   m_delayLock; 

	time_t					m_nLastCheckTime;

	int m_initState; 
};


#endif //
