#pragma once

/*
 *	条件变量,用作线程同步
 */
#ifndef SDCONDITION_H
#define SDCONDITION_H

#include "sdtype.h"

#ifndef WIN32
#  include <pthread.h>
#  include <errno.h>
#  include <sys/time.h>
#endif

namespace  Crown
{
    /**
    * @defgroup groupsync 线程同步
    * @ingroup  Crown
    * @{
    */

// Attempt to resolve POSIX behaviour conformance for win32 build.
#define CONDITION_WIN32_CONFORMANCE_TO_POSIX

    class CSDMutex;

    /**
    *@brief 条件变量，用于线程同步
    */
    class CSDCondition
    {
    public:
        CSDCondition();
        ~CSDCondition();


        /**
        * @brief
        * 等待条件被触发，这里有三种情况，一为条件改变，二为被中断，三为超时。
        * 当条件被触发后，原子的进入互斥量的互斥范围内。
        * @param mutex : 和条件变量相关的互斥量的引用
        * @param ms : 等待超时时间，单位为毫秒
        * @return true 条件被触发; false 超时或者被中断
        **/
        bool SDAPI Wait (CSDMutex &mutex, uint32 dwMs = SDINFINITE);



        /**
        * @brief
        * 等待条件被触发，这里有三种情况，一为条件改变，二为被中断，三为超时.
        * 当条件被触发后，原子的进入互斥量的互斥范围内。
        * @param mutex : 和条件变量相关的互斥量的指针
        * @param ms : 等待超时时间，单位为毫秒
        * @return true 条件被触发; false 超时或者被中断
        **/
        bool SDAPI Wait (CSDMutex *mutex, uint32 dwMs = SDINFINITE);

        /**
        * @brief
        * 触发所有等待中的随机一个等待的Wait。
        * @return void
        **/
        void SDAPI Signal();

        /**
        * @brief
        * 触发所有的等待的Wait。
        * @return void
        **/
        void SDAPI Broadcast();

    private:

        //  no value sematics, therefore private and not implemented.
        CSDCondition (const CSDCondition&);
        CSDCondition& operator = (const CSDCondition&);

    private:
#ifdef WIN32
#  ifdef CONDITION_WIN32_CONFORMANCE_TO_POSIX

        // boost clone with modification
        // licesnse text below
        void EnterWait ();
        void* m_gate;
        void* m_queue;
        void* m_mutex;
        uint32 m_gone;  // # threads that timed out and never made it to m_queue
        uint32 m_blocked; // # threads blocked on the condition
        uint32 m_waiting; // # threads no longer waiting for the condition but
        // still waiting to be removed from m_queue
#  else
        HANDLE mId;
#  endif

#else
        mutable  pthread_cond_t mId;
#endif
    };

}

#endif

