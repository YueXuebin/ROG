#include "SDnetPCH.h"
#include "sdnetconfig.h"
#include "bufpool.h"

CBufPool::CBufPool(void)
{
	m_dwSize = 0;
}

CBufPool::~CBufPool(void)
{
}

void CBufPool::Init(uint32 dwSize)
{
	m_dwSize = dwSize;
}

void CBufPool::Uninit()
{
	for(CSetBuf::iterator it = m_oSetBuf.begin(); it != m_oSetBuf.end(); ++it)
	{
		CnDelete[] *it;
	}
	
	m_oSetBuf.clear();
	m_oListBuf.clear();
}

uint32 CBufPool::GetSize()
{
	return m_dwSize;
}

char* CBufPool::Create()
{
	char* pBuf = NULL;
	if(!m_oListBuf.empty())
	{
		pBuf = m_oListBuf.front();
		CnVerify(pBuf != NULL);
		m_oListBuf.pop_front();

#ifdef _DEBUG
		CSetBuf::iterator it = m_oSetFree.find(pBuf);
		if(it == m_oSetFree.end())
		{
			CnVerify(false);
		}
		else
		{
			m_oSetFree.erase(it);
		}
#endif
	}
	else
	{
		pBuf = CnNew char[m_dwSize];
		memset(pBuf, 0, m_dwSize);
		m_oSetBuf.insert(pBuf);
	}

	return pBuf;
}

void CBufPool::Release(char* pBuf)
{
	if(NULL == pBuf)
	{
		return;
	}

#ifdef _DEBUG
	memset(pBuf, 0, m_dwSize);
	CSetBuf::iterator it = m_oSetBuf.find(pBuf);
	if(it == m_oSetBuf.end())
	{
		CnVerify(false);
	}

	it = m_oSetFree.find(pBuf);
	if(it != m_oSetFree.end())
	{
		CnVerify(false);
	}
	else
	{
		m_oSetFree.insert(pBuf);
	}
#endif

	m_oListBuf.push_back(pBuf);
}
