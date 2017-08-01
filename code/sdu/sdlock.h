#pragma once

// 线程同步
#ifndef SDLOCK_H
#define SDLOCK_H
/**
* @file sdlock.h
* @author lw
* @brief 锁类
*
**/
#include "sdmutex.h"

namespace  Crown
{
    /**
    * @defgroup groupsync 线程同步
    * @ingroup  Crown
    * @{
    */
    /**
    *@brief 线程同步lock
    */
    template <class MT = CSDMutex>
    class CSDLock
    {
    public:
        CSDLock(MT & mt):m_mutex(mt)
        {
            m_mutex.Lock();
        }
        ~CSDLock()
        {
            m_mutex.Unlock();
        }
    private:
        MT & m_mutex;
    };

    typedef CSDLock<CSDMutex>  CSDMutexLock;

    typedef CSDLock<CSDRecursiveMutex>  CSDRecursiveLock;

    /**
    *@brief 线程读同步lock
    */
    class CSDReadLock
    {
    public:
        CSDReadLock (CSDRWMutex &lock):m_mutex(lock)
        {
            m_mutex.LockRead();
        }
        ~CSDReadLock()
        {
            m_mutex.Unlock();
        }

    private:
        CSDRWMutex & m_mutex ;
    };

    /**
    *@brief 线程写同步lock
    */
    class CSDWriteLock
    {
    public:
        CSDWriteLock (CSDRWMutex &lock):m_mutex(lock)
        {
            m_mutex.LockWrite();
        }
        ~CSDWriteLock()
        {
            m_mutex.Unlock();
        }

    private:
        CSDRWMutex & m_mutex ;
    };



    /**
    * @brief 线程同步lock，主要用于从进入一个函数到退出该函数的整个过程加锁，
    * 从使用感觉而言，此加锁为面向范围（对一定代码范围，只能有一个线程同时进入）
    * 的加锁而非一般加锁的面向对象（对于某个类(CriticalSection, Mutex等)的对象，只能有一个线程占有此资源）
    * 加锁。
    * 由于一个函数可能有多个退出点，对此函数使用这种加锁，相对于其他锁而言，不用关心最后更新代码时要随着退
    * 出点的增减而改写锁的退出代码，从而避免出错
    */
    template<class Mutex>
    class CSDLockGuard
    {
    private:
        Mutex& m_lock;
    public:
        CSDLockGuard(Mutex& oLock) : m_lock(oLock)
        {
            m_lock.Lock();
        }
        ~CSDLockGuard()
        {
            m_lock.Unlock();
        }

        class GuardOff
        {
            CSDLockGuard& m_guard;
        public:
            GuardOff(CSDLockGuard<Mutex>& g):m_guard(g)
            {
                m_guard.m_lock.Unlock();
            }
            ~GuardOff()
            {
                m_guard.m_lock.Lock();
            }
        };
    };


}
#endif

