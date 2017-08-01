#pragma once

#ifndef SDMEMORYPOOL_H
#define SDMEMORYPOOL_H

/**
* @file sdmemorypool.h
* @author lw
* @brief 可变和非可变内存池
*
**/
#include "sdtype.h"

namespace Crown
{
    /**
    * @defgroup groupmemorypool 内存池(包括对象池)
    * @ingroup  Crown
    * @{
    */

    /**
    * @brief 可变内存块大小内存池，
    * 内存池申请的内存不会自动释放，只在内存池销毁时释放。
    * 注意：此类“非”线程安全
    * 使用示例：
    * CSDVarMemoryPool pool;
    * pool.Create();
    * 某一线程：
    * char * p1 = pool.Malloc(512);//记得加锁
    * char * p2 = pool.Malloc(31);//记得加锁
    * char * p3 = pool.Malloc(128);//记得加锁
    *
    * 另外一个线程：
    * pool.Free(p3);//记得加锁
    * pool.Free(p2);//记得加锁
    * pool.Free(p1);//记得加锁
    */
    class CSDVarMemoryPool
    {
        struct MemoryPage
        {
            MemoryPage* Next;	// next memory page
        };

    public:
        CSDVarMemoryPool();
        ~CSDVarMemoryPool();

        /**
        * @brief
        * 创建可变内存池
        * @param dwPageSize : 内部分配的内存页大小，内存不够时，内存池会申请一块新的内存页
        * @return 创建成功返回true，否则返回false
        **/
        bool SDAPI Create(uint32 dwPageSize = 0x80000);

        /**
        * @brief
        * 分配Len长度的Buffer
        * @param dwLen : 获得的内存块大小长度
        * @return  返回的内存，如果返回为NULL，则代表分配失败
        **/
        void* SDAPI Malloc(uint32 dwLen);

        /**
        * @brief
        * 回收内存
        * @param p : 指向需要回收的内存
        * @return void
        **/
        void SDAPI Free(void* p);

        /**
        * @brief
        * 清空内存池，使所有的内存都可以使用，此方法不会将内存返回给操作系统
        * @return void
        **/
        void SDAPI Clear();

        /**
        * @brief
        * 获取当前内存使用量
        * @return 当前内存使用量
        **/
        int32 SDAPI GetMemUsed();

    private:
        void* GetPoolMemory(uint32 dwLen);
        void FreePoolMemory(void* pMemBlock, byte dwType);

        bool AddFreeMemory(int32 dwIndex);
        bool SetMemoryPage();

        inline byte* GetPageBufGegin(MemoryPage *pPage)
        {
            return (byte*)(pPage + 1);
        }

        inline byte* GetPageBufEnd(MemoryPage *pPage)
        {
            return (byte*)(pPage + 1) + m_nPageSize;
        }

    private:
        static const uint32 ALIGNMENT = 8;
        static const uint32 ALLOC_COUNT = 16;
        static const uint32 MIN_PAGESIZE = 0x40000;	// min pPage size
        static const uint32 MAX_UNIT_SIZE = 128;
        static const uint32 UNIT_TYPE_COUNT = 16;

        byte* m_pFreeHead[UNIT_TYPE_COUNT];
        int32 m_nFreeCount[UNIT_TYPE_COUNT];

        MemoryPage* m_pHeadPage;
        MemoryPage* m_pWorkPage;
        byte* m_pPageBuf;
        uint32 m_nPageSize;
    };




    /**
    *@brief 固定内存块大小内存池，用于分配固定大小的内存块
    * 内存池申请的内存不会自动释放，只在内存池销毁时释放
    * 注意：此类“非”线程安全
    * 使用示例：
    * CSDFixMemoryPool pool;
    * pool.Create(128);
    * 某一线程：
    * char * p1 = pool.Malloc();//记得加锁
    * char * p2 = pool.Malloc();//记得加锁
    * char * p3 = pool.Malloc();//记得加锁
    *
    * 另外一个线程：
    * pool.Free(p3);//记得加锁
    * pool.Free(p2);//记得加锁
    * pool.Free(p1);//记得加锁
    */
    class CSDFixMemoryPool
    {
        struct MemoryPage
        {
            MemoryPage* Next;	// next memory page
            int32 nFreeHead;		// the first free unit in page
            int32 nFreecount;		// free unit in page
        };

    public:
        CSDFixMemoryPool();
        ~CSDFixMemoryPool();

        /**
        * @brief
        * 初始化内存池
        * @param dwUnitSize : 每一个分配的内存块大小
        * @param dwPageSize : 内部分配的内存页大小，内存不够时，内存池会申请一块新的内存页
        * @return  创建成功返回true，失败返回false
        **/
        bool SDAPI Create(uint32 dwUnitSize, uint32 dwPageSize = 0x40000);

        /**
        * @brief
        * 得到一块新的内存
        * @return  void*
        **/
        void* SDAPI Malloc();

        /**
        * @brief
        * 归还一块分配的内存
        * @param p : 内存的地址
        * @return  void
        **/
        void SDAPI Free(void* p);

        /**
        * @brief
        * 清空内存池，使所有的内存都可以使用，此方法不会将内存返回给操作系统
        * @return void
        **/
        void SDAPI Clear();

        /**
        * @brief
        * 获取当前内存使用量
        * @return 当前内存使用量
        **/
        int32 SDAPI GetMemUsed();

    private:
        bool AddMemoryPage();
        void InitPage(MemoryPage *pPage);

        inline char* GetPageBuf(MemoryPage *pPage)
        {
            return (char*)(pPage + 1);
        }

    private:
        static const int32 ALIGNMENT = 4;
        static const uint32 MIN_PAGESIZE = 0x40000;	// min page size

        MemoryPage* m_pHeadPage;					// first page
        uint32 m_nUnitSize;					// unit memory size
        uint32 m_nPageSize;					// total memory in page
    };


}//namespace Crown

#endif
