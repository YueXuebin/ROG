#include "SDnetPCH.h"
#include "sdnetconfig.h"
#include "epolllistenermgr.h"

CEpollListenerMgr::CEpollListenerMgr(void)
{
}

CEpollListenerMgr::~CEpollListenerMgr(void)
{
}

void CEpollListenerMgr::Uninit()
{
	for(CMapEpollListener::iterator it = m_oMapListener.begin(); it != m_oMapListener.end(); ++it)
		CnDelete it->second;
	
	m_oMapListener.clear();
}

CEpollListener* CEpollListenerMgr::Create(uint32 dwID)
{
	if(Find(dwID) != NULL)
	{
		CRITICAL("%s, CEpollListener id %u already exist", __FUNCTION__, dwID);
		return NULL;
	}

	CEpollListener* poListener = CnNew CEpollListener;
	if(NULL == poListener)
	{
		CRITICAL("%s, new CEpollListener failed", __FUNCTION__);
		return NULL;
	}
	poListener->SetID(dwID);
	m_oMapListener[dwID] = poListener;
	return poListener;
}

CEpollListener* CEpollListenerMgr::Find(uint32 dwID)
{
	CMapEpollListener::iterator it = m_oMapListener.find(dwID);
	if(it != m_oMapListener.end())
	{
		return it->second;
	}
	return NULL;
}

void CEpollListenerMgr::Release(CEpollListener* poListener)
{
	if(NULL == poListener)
	{
		CnVerify(false);
		return;
	}

	m_oMapListener.erase(poListener->GetID());
	CnDelete poListener;
}
