#pragma once

#ifndef SDMUTEX_H
#define SDMUTEX_H
/**
* @file sdmutex.h
* @author lw
* @brief 互斥类，包括无锁，有锁，读写锁等
*
**/
#include "sdtype.h"
#include "sdcondition.h"

#if !(defined(WIN32) || defined(WIN64))
#include <pthread.h>
#endif

namespace Crown
{
    /**
    * @defgroup groupsync 线程同步
    * @ingroup  Crown
    * @{
    */
#ifdef WIN32
    typedef CRITICAL_SECTION SDMutexId;
#else
    typedef pthread_mutex_t SDMutexId;
#endif

    bool SDMutexInit(SDMutexId & id);

    void SDMutexLock(SDMutexId & id);

    void SDMutexUnlock(SDMutexId & id);

    bool SDMutexUninit(SDMutexId &id);


    /**
    *@brief 空锁，什么也不做
    */
    class CSDNonMutex
    {
    public:

        /**
        * @brief
        * 什么也不做
        * @return void
        */
        inline void SDAPI Lock() {}

        /**
        * @brief
        * 什么也不做
        * @return void
        */
        inline void SDAPI Unlock() {}

        /**
        * @brief
        * 什么也不做
        * @return void
        */
        inline void SDAPI LockRead() {}

        /**
        * @brief
        * 什么也不做
        * @return void
        */
        inline void SDAPI LockWrite() {}
    };

    /**
    *@brief 线程同步普通锁操作类
    */
    class CSDMutex
    {
        friend class CSDCondition;

    public:
        CSDMutex();
        ~CSDMutex();

        /**
        * @brief
        * 进入锁
        * @return void
        */
        void SDAPI Lock();

        /**
        * @brief
        * 离开锁
        * @return void
        */
        void SDAPI Unlock();

        /**
        * @brief
        * 进入读锁，当前能有多个读线程进入
        * @return void
        */
        void SDAPI LockRead()
        {
            Lock();
        }

        /**
        * @brief
        * 进入写锁，当前只能有一个线程进入
        * @return void
        */
        void SDAPI LockWrite()
        {
            Lock();
        }

    private:
        //  no value sematics, therefore private and not implemented.
        CSDMutex (const CSDMutex&);
        CSDMutex& operator= (const CSDMutex&);

    private:
#if (defined(WIN32) || defined(WIN64))
        SDMutexId m_mutexId;
#else
        mutable SDMutexId m_mutexId;
#endif
    };

    /**
    *@brief 线程同步读写锁操作类
    */
    class CSDRWMutex
    {
    public:
        CSDRWMutex();
        ~CSDRWMutex();

        /**
        * @brief
        * 进入读锁，当前能有多个读线程进入
        * @return void
        */
        void SDAPI LockRead();

        /**
        * @brief
        * 进入写锁，当前只能有一个线程进入
        * @return void
        */
        void SDAPI LockWrite();

        /**
        * @brief
        * 进入锁
        * @return void
        */
        void SDAPI Lock();

        /**
        * @brief
        * 离开锁
        * @return void
        */
        void SDAPI Unlock();

        /**
        * @brief
        * 当前有多少个读取线程同时读取
        * @return 读取者的数量
        */
        unsigned int SDAPI ReaderCount() const;

        /**
        * @brief
        * 当前有多少个写操作线程在等待
        * @return 等待写入者的数量
        */
        unsigned int SDAPI PendingWriterCount() const;

    private:
        CSDMutex m_mutex;
        CSDCondition m_readCond;
        CSDCondition m_pendingWriteCond;
        unsigned int m_readerCount;
        bool m_hasWriterLock;
        unsigned int m_pendingWriteCount;
    };

    /**
    *@brief 线程同步递归锁操作类
    */
    class CSDRecursiveMutex
    {
    public:
        CSDRecursiveMutex();
        ~CSDRecursiveMutex();

        /**
        * @brief
        * 进入锁
        * @return void
        */
        void SDAPI Lock();

        /**
        * @brief
        * 离开锁
        * @return void
        */
        void SDAPI Unlock();

        /**
        * @brief
        * 进入读锁，当前能有多个读线程进入
        * @return void
        */
        void SDAPI LockRead()
        {
            Lock();
        }

        /**
        * @brief
        * 进入写锁，当前只能有一个线程进入
        * @return void
        */
        void SDAPI LockWrite()
        {
            Lock();
        }
    private:
        SDMutexId m_mutexId;
#ifndef WIN32
        pthread_mutexattr_t m_mutexAttr;
#endif
    };

} //namespace Crown



#endif
