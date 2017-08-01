#include "SDuPCH.h"
#include <stdio.h>
#include "sddataqueue.h"

using namespace Crown;

CSDDataQueue::CSDDataQueue(void)
{
    m_pArray	= nullptr;
    m_nHead		= 0;
    m_nTail		= 0;
    m_nSize		= 0;

}

CSDDataQueue::~CSDDataQueue(void)
{
    if(m_pArray != nullptr)
    {
        CnDelete[] m_pArray;
    }
}

bool SDAPI CSDDataQueue::Init(int32 nSize)
{
    m_pArray = CnNew void*[nSize];
    m_nSize = nSize;
    return true;
}

bool SDAPI CSDDataQueue::PushBack(void* ptr)
{
    int32 nDist = m_nTail + m_nSize - m_nHead;
    int32 nUsed = nDist >= m_nSize ? (nDist-m_nSize) : nDist;

    if(nUsed >= m_nSize-1)
    {
        return false;
    }

    m_pArray[m_nTail] = ptr;
    if(++m_nTail >= m_nSize)
    {
        m_nTail = 0;
    }

    return true;
}

void * SDAPI CSDDataQueue::PopFront()
{
    int32 nDist = m_nTail + m_nSize - m_nHead;
    int32 nUsed = nDist >= m_nSize ? (nDist-m_nSize) : nDist;
    if(0 == nUsed)
    {
        return nullptr;
    }

    void* ptr = m_pArray[m_nHead];
    if(++m_nHead >= m_nSize)
    {
        m_nHead = 0;
    }

    return ptr;
}

