#include "SDuPCH.h"
#include "sdmemorypool.h"
#include "Assertion.h"
#include <stdio.h>
#include <cstdlib>

// the malloc/free functions are in cstdlib in Mac OS
#ifndef APPLE
#include <malloc.h>
#endif

namespace Crown
{
    CSDVarMemoryPool::CSDVarMemoryPool()
        :m_pHeadPage(nullptr), m_pWorkPage(nullptr), m_pPageBuf(nullptr)
    {
        for (uint32 i = 0; i < UNIT_TYPE_COUNT; ++ i)
        {
            m_pFreeHead[i] = nullptr;
            m_nFreeCount[i] = 0;
        }
    }

    CSDVarMemoryPool::~CSDVarMemoryPool()
    {
        MemoryPage* pMemoryPage = m_pHeadPage;
        while (m_pHeadPage != nullptr)
        {
            pMemoryPage = m_pHeadPage->Next;
            free(m_pHeadPage);
            m_pHeadPage = pMemoryPage;
        }
    }

    void* SDAPI CSDVarMemoryPool::Malloc(uint32 Len)
    {
        CnVerify(Len > 0);

        Len ++;
        if (Len > MAX_UNIT_SIZE)
        {
            // allocate memory from system if requery Len is too large
            void* buf = malloc(Len);
            if (buf == nullptr)
            {
                return nullptr;
            }

            //if content of 1 byte before memory means allocate form system
            *(char*)buf = 0;

            return (byte*)buf + 1;
        }
        else
        {
            return GetPoolMemory(Len);
        }
    }

    void SDAPI CSDVarMemoryPool::Free(void* p)
    {
        CnVerify(p != nullptr);

        byte* temp = (byte*)p - 1;
        unsigned char type = *temp;
        if (type == 0)	//if content of 1 byte before memory means allocate form system
        {
            free(temp);
        }
        else
        {
            FreePoolMemory(temp, type);
        }
    }

    void* CSDVarMemoryPool::GetPoolMemory(uint32 Len)
    {
        Len = (Len + (ALIGNMENT-1)) & ~(ALIGNMENT-1);
        int idx = (Len - 1) / ALIGNMENT;

        //if free memory unit is not enough, first get some free units
        if (m_nFreeCount[idx] == 0
                && !AddFreeMemory(idx))
        {
            return nullptr;
        }

        -- m_nFreeCount[idx];
        byte* buf = m_pFreeHead[idx];
        m_pFreeHead[idx] = (byte*)(*((int64*)m_pFreeHead[idx]));
        *buf = idx + 1;

        return buf + 1;
    }

    void CSDVarMemoryPool::FreePoolMemory(void* memblock, unsigned char type)
    {
        int idx = type - 1;
        *(int64*)memblock = (int64)m_pFreeHead[idx];
        m_pFreeHead[idx] = (byte*)memblock;
        ++ m_nFreeCount[idx];
    }

    bool CSDVarMemoryPool::AddFreeMemory(int idx)
    {
        const int UNIT_SIZE = (idx + 1) * ALIGNMENT;

        if ((m_pPageBuf + UNIT_SIZE ) > GetPageBufEnd(m_pWorkPage)
                && !SetMemoryPage())
        {
            return false;
        }

        byte* page_end = GetPageBufEnd(m_pWorkPage);
        for (uint32 i = 0; i < ALLOC_COUNT; ++ i)
        {
            *(int64*)m_pPageBuf = (int64)m_pFreeHead[idx];
            m_pFreeHead[idx] = m_pPageBuf;

            m_pPageBuf += UNIT_SIZE;
            ++ m_nFreeCount[idx];

            if (m_pPageBuf + UNIT_SIZE > page_end)
                break;
        }

        return true;
    }

    bool CSDVarMemoryPool::SetMemoryPage()
    {
        if(m_pWorkPage->Next != nullptr)
        {
            m_pWorkPage = m_pWorkPage->Next;
        }
        else
        {
            void* buf = malloc(sizeof(MemoryPage) + m_nPageSize);
            if (buf == nullptr)
            {
                return false;
            }
            else
            {
                MemoryPage* pMemoryPage = (MemoryPage*)(buf);
                pMemoryPage->Next = nullptr;
                m_pWorkPage->Next = pMemoryPage;
                m_pWorkPage = pMemoryPage;
            }
        }
        m_pPageBuf = GetPageBufGegin(m_pWorkPage);
        return true;
    }

    int CSDVarMemoryPool::GetMemUsed()
    {
        int used = 0;
        const int PAGE_SIZE = sizeof(MemoryPage) + m_nPageSize;

        MemoryPage* pMemoryPage = m_pHeadPage;
        while (pMemoryPage != nullptr)
        {
            pMemoryPage = pMemoryPage->Next;
            used += PAGE_SIZE;
        }

        return used;
    }

    void SDAPI CSDVarMemoryPool::Clear()
    {
        m_pWorkPage = m_pHeadPage;
        m_pPageBuf = GetPageBufGegin(m_pWorkPage);
    }

    bool CSDVarMemoryPool::Create( uint32 PageSize /*= 0x80000*/ )
    {
        PageSize = (PageSize + (ALIGNMENT-1)) & ~(ALIGNMENT-1);
        if (PageSize < MIN_PAGESIZE)
        {
            m_nPageSize = MIN_PAGESIZE;
        }
        else
        {
            m_nPageSize = PageSize;
        }

        void* buf = malloc(sizeof(MemoryPage) + m_nPageSize);
        if (buf == nullptr)
        {
            return false;
        }
        else
        {
            MemoryPage* pMemoryPage = (MemoryPage*)(buf);
            pMemoryPage->Next = nullptr;
            m_pWorkPage = pMemoryPage;
            m_pPageBuf = GetPageBufGegin(m_pWorkPage);
            m_pHeadPage = m_pWorkPage;
        }

        return true;
    }

// Fix Memory pool size

    CSDFixMemoryPool::CSDFixMemoryPool()
        :m_pHeadPage(nullptr), m_nUnitSize(0), m_nPageSize(0)
    {
    }

    CSDFixMemoryPool::~CSDFixMemoryPool()
    {
        MemoryPage* pMemoryPage = m_pHeadPage;
        while (m_pHeadPage != nullptr)
        {
            pMemoryPage = m_pHeadPage->Next;
            free(m_pHeadPage);
            m_pHeadPage = pMemoryPage;
        }
    }

    void* CSDFixMemoryPool::Malloc()
    {
        MemoryPage* pMemoryPage = m_pHeadPage;
        while (pMemoryPage != nullptr && pMemoryPage->nFreecount == 0)
        {
            pMemoryPage = pMemoryPage->Next;
        }

        // add new page if space is not enough
        if (pMemoryPage == nullptr)
        {
            if(!AddMemoryPage())
            {
                return nullptr;
            }
            pMemoryPage = m_pHeadPage;
        }

        // get unused memory
        -- pMemoryPage->nFreecount;
        char* buf = GetPageBuf(pMemoryPage) + pMemoryPage->nFreeHead * m_nUnitSize;
        pMemoryPage->nFreeHead = *(int*)(buf);

        return buf;
    }

    void CSDFixMemoryPool::Free(void* p)
    {
        // don't check null point for fast
        MemoryPage* pMemoryPage = m_pHeadPage;
        char* buf = GetPageBuf(m_pHeadPage);

        // find point in which page
        while((p < buf ||
                p > buf + m_nPageSize) &&
                pMemoryPage != nullptr)
        {
            pMemoryPage = pMemoryPage->Next;
            buf = GetPageBuf(pMemoryPage);
        }

        // do not in any page
        if (pMemoryPage == nullptr)
        {
            return;
        }

        *(int*)p = pMemoryPage->nFreeHead;
        pMemoryPage->nFreeHead = ((char*)p - buf) / m_nUnitSize;
        ++ pMemoryPage->nFreecount;

        return;
    }

    bool CSDFixMemoryPool::AddMemoryPage()
    {
        void* buf = malloc(sizeof(MemoryPage) + m_nPageSize);
        if (buf == nullptr)
        {
            return false;
        }

        MemoryPage* pMemoryPage = (MemoryPage*)(buf);
        InitPage(pMemoryPage);

        if (m_pHeadPage == nullptr)
        {
            pMemoryPage->Next = nullptr;
            m_pHeadPage = pMemoryPage;
        }
        else
        {
            pMemoryPage->Next = m_pHeadPage;
            m_pHeadPage = pMemoryPage;
        }

        return true;
    }

    int32 SDAPI CSDFixMemoryPool::GetMemUsed()
    {
        int used = 0;
        const int PAGE_SIZE = sizeof(MemoryPage) + m_nPageSize;

        MemoryPage* pMemoryPage = m_pHeadPage;
        while (pMemoryPage != nullptr)
        {
            pMemoryPage = pMemoryPage->Next;
            used += PAGE_SIZE;
        }

        return used;
    }

    void SDAPI CSDFixMemoryPool::Clear()
    {
        MemoryPage* pMemoryPage = m_pHeadPage;
        while (pMemoryPage != nullptr)
        {
            InitPage(pMemoryPage);
            pMemoryPage = pMemoryPage->Next;
        }
    }

    void CSDFixMemoryPool::InitPage(MemoryPage *Page)
    {
        Page->nFreecount = m_nPageSize / m_nUnitSize;
        Page->nFreeHead = 0;

        void* head = GetPageBuf(Page);
        for (int i = 1; i < Page->nFreecount; ++i)
        {
            *(int*)head = i;
            head = (int*)((char*)head + m_nUnitSize);
        }
    }

    bool CSDFixMemoryPool::Create( uint32 UnitSize, uint32 PageSize /*= 0x40000*/ )
    {
        if (UnitSize < 4)
        {
            m_nUnitSize = 4;
        }
        {
            m_nUnitSize = (UnitSize + (ALIGNMENT-1)) & ~(ALIGNMENT-1);
        }

        if (PageSize < MIN_PAGESIZE)
        {
            m_nPageSize = MIN_PAGESIZE;
        }
        else
        {
            m_nPageSize = (PageSize / m_nUnitSize) * m_nUnitSize;
        }

        return AddMemoryPage();
    }

}


