#include "SDnetPCH.h"
#include "simpleloopbuffer.h"
#include <stdio.h>
#include <memory.h>
CSimpleLoopBuffer::CSimpleLoopBuffer() : 
m_pBuffer(NULL),
m_pNextRead(NULL),
m_pNextWrite(NULL),
m_pEnd(NULL),
m_nSize(0)
{

}

CSimpleLoopBuffer::~CSimpleLoopBuffer()
{
	if(NULL != m_pBuffer)
	{
		CnDelete [] m_pBuffer;
		m_pBuffer = NULL;
	}
}

bool CSimpleLoopBuffer::Init(int32 nSize)
{
	m_pBuffer = CnNew char[nSize + 1];
	if(NULL == m_pBuffer)
	{
		return false;
	}

	m_pNextRead = m_pBuffer;
	m_pNextWrite = m_pBuffer;
	m_pEnd = m_pBuffer + nSize + 1;
	m_nSize = nSize;

	return true;

}

bool CSimpleLoopBuffer::PushBack(const char *pData, int32 nLen)
{
	if(m_pNextWrite >= m_pNextRead)
	{
		if(m_pEnd - m_pNextWrite < nLen && m_pNextRead - m_pBuffer <= nLen)
		{
			return false;
		}
		if(m_pEnd - m_pNextWrite < nLen && m_pNextRead - m_pBuffer > nLen)
		{
			m_pEnd = m_pNextWrite;
			m_pNextWrite = m_pBuffer;
		}
	}
	if(m_pNextWrite < m_pNextRead)
	{
		if(m_pNextRead - m_pNextWrite <= nLen)
		{
			return false;
		}
	}
	memcpy(m_pNextWrite, pData, nLen);
	m_pNextWrite += nLen;

	return true;
}

bool CSimpleLoopBuffer::PopFront(char * &pBuf, int32 nLen)
{
	if(m_pNextWrite == m_pNextRead)
	{
		return false;
	}
	if(m_pNextWrite > m_pNextRead)
	{
		if(m_pNextWrite - m_pNextRead < nLen)
		{
			return false;
		}
	}
	if(m_pNextWrite < m_pNextRead)
	{
		if(m_pEnd == m_pNextRead)
		{
			m_pNextRead = m_pBuffer;
			m_pEnd = m_pBuffer + m_nSize + 1;
		}
		if(m_pEnd - m_pNextRead < nLen)
		{
			return false;
		}
	}
	pBuf = m_pNextRead;
	m_pNextRead += nLen;
	if(m_pNextRead == m_pEnd)
	{
		m_pNextRead = m_pBuffer;
		m_pEnd = m_pBuffer + m_nSize + 1;
	}

	return true;
}

bool CSimpleLoopBuffer::DiscardFront(int32 nLen)
{
	if(m_pNextWrite >= m_pNextRead)
	{
		if(m_pNextWrite - m_pNextRead < nLen)
		{
			return false;
		}
	}
	if(m_pNextWrite < m_pNextRead)
	{
		if(m_pEnd - m_pNextRead < nLen)
		{
			return false;
		}
	}
	m_pNextRead += nLen;
	if(m_pNextRead == m_pEnd)
	{
		m_pNextRead = m_pBuffer;
		m_pEnd = m_pBuffer + m_nSize;
	}

	return true;
}
