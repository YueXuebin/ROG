#include "SDuPCH.h"
#include <stdio.h>
#include <string.h>
#include "sdloopbuffer.h"

namespace Crown
{

    CSDLoopBuffer::CSDLoopBuffer(void)
    {
        m_pBuffer	= nullptr;
        m_pHead		= nullptr;
        m_pTail		= nullptr;
        m_nSize		= 0;
    }

    CSDLoopBuffer::~CSDLoopBuffer(void)
    {
        if(m_pBuffer != nullptr)
        {
            CnDelete m_pBuffer;
        }
    }

    bool SDAPI CSDLoopBuffer::Init(int32 nSize)
    {
        m_pBuffer = CnNew char[nSize];
        if(nullptr == m_pBuffer)
        {
            return false;
        }

        m_pHead = m_pBuffer;
        m_pTail = m_pBuffer;
        m_nSize = nSize;

        return true;
    }

    bool SDAPI CSDLoopBuffer::PushBack(const char* pData, int32 nLen)
    {
        int32 nDist = (int32)(m_pTail + (uint32)m_nSize - m_pHead);
        int32 nUsed = nDist >= m_nSize ? (nDist-m_nSize) : nDist;
        if(nLen + nUsed + 1 > m_nSize)
        {
            return false;
        }

        if(m_pTail + nLen >= m_pBuffer + m_nSize)
        {
            int32 nSeg1 = (int32)(m_pBuffer + m_nSize - m_pTail);
            int32 nSeg2 = nLen - nSeg1;
            memcpy(m_pTail, pData, nSeg1);
            memcpy(m_pBuffer, pData+nSeg1, nSeg2);
            m_pTail = m_pBuffer + nSeg2;
        }
        else
        {
            memcpy(m_pTail, pData, nLen);
            m_pTail += nLen;
        }

        return true;
    }

    bool SDAPI CSDLoopBuffer::PopFront(char* pBuf, int32 nLen)
    {
        int32 nDist = (int32)(m_pTail + (uint32)m_nSize - m_pHead);
        int32 nUsed = nDist >= m_nSize ? (nDist-m_nSize) : nDist;

        if(nLen > nUsed)
        {
            return false;
        }

        if(m_pHead + nLen >= m_pBuffer + m_nSize)
        {
            int32 nSeg1 = (int32)(m_pBuffer + m_nSize - m_pHead);
            int32 nSeg2 = nLen - nSeg1;
            memcpy(pBuf, m_pHead, nSeg1);
            memcpy(pBuf+nSeg1, m_pBuffer, nSeg2);
            m_pHead = m_pBuffer + nSeg2;
        }
        else
        {
            memcpy(pBuf, m_pHead, nLen);
            m_pHead += nLen;
        }

        return true;
    }

    void SDAPI CSDLoopBuffer::DiscardFront(int32 nLen)
    {
        int32 nDist = (int32)(m_pTail + (uint32)m_nSize - m_pHead);
        int32 nUsed = nDist >= m_nSize ? (nDist-m_nSize) : nDist;

        if(nLen > nUsed)
        {
            return;
        }

        if(m_pHead + nLen >= m_pBuffer + m_nSize)
        {
            int32 nSeg1 = (int32)(m_pBuffer + m_nSize - m_pHead);
            int32 nSeg2 = nLen - nSeg1;
            m_pHead = m_pBuffer + nSeg2;
        }
        else
        {
            m_pHead += nLen;
        }
    }
}
