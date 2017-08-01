#pragma once


#ifndef SD_THREAD_H
#define SD_THREAD_H

/**
* @file sdthread.h
* @author lw
* @brief 线程工具类
**/

#include "sdtype.h"

namespace Crown
{
    /**
    * @defgroup groupthread 多进程多线程(包括线程池)
    * @ingroup  Crown
    * @{
    */

#ifdef WIN32
    typedef uint32 SDTHREADID;
#else
    typedef llong SDTHREADID;
#endif // WIN32 

    /**
    * @brief
    * 得到线程id号
    * @return 线程id号
    */
    SDTHREADID SDAPI SDGetThreadId();

    typedef struct tagSThreadAttr
    {

    } SThreadAttr;

#if (defined(WIN32) || defined(WIN64))
    typedef unsigned int (WINAPI *PFThrdProc)(void *);
#define SDTHREAD_DECLARE(x)  unsigned int WINAPI  x
#define SDTHREAD_RETURN(x)   return x
#else
    typedef void *  (*PFThrdProc)(void *);
#define SDTHREAD_DECLARE(x)  void *  x
#define SDTHREAD_RETURN(x)   return x
#endif // 


    SDHANDLE SDAPI SDCreateThread(
        SThreadAttr * pAttr,
        PFThrdProc pThrdProc,
        void * pArg,
        SDTHREADID *pThreadId = NULL,
        bool bSuspend = false);

    int32 SDAPI SDThreadWait(SDHANDLE handle);

    void SDAPI SDThreadCloseHandle(SDHANDLE handle);

    void SDAPI SDThreadTerminate(SDHANDLE handle);

    void SDAPI SDThreadSuspend(SDHANDLE handle);

    void SDAPI SDThreadResume(SDHANDLE handle);

    void SDAPI SDSetThreadAttr(SDHANDLE,SThreadAttr * pAttr);

    SThreadAttr* SDAPI SDGetThreadAttr(SDHANDLE);

    /**
    * @brief 线程操作类
    */
    class CSDThread
    {

    public:
        CSDThread();
        virtual ~CSDThread();

        /**
        * @brief
        * 启动一个线程
        * @param pThredProc: 线程执行处理函数，注意：使用次接口后，不会调用类的成员函数ThrdProc
        * @param pArg : 线程执行参数
        * @param bSuspend : 设置线程状态为挂起
        * @return 成功返回true，失败返回false
        */
        bool SDAPI Start(PFThrdProc pfThrdProc, void *pArg, bool bSuspend = false);

        /**
        * @brief
        * 启动一个线程 注意：使用此接口启动线程会将类成员函数ThrdProc作为线程执行函数，用户需继承此
        * 类来执行线程
        * @param bSuspend : 设置线程状态为挂起
        * @return 成功返回true，失败返回false
        */
        bool SDAPI Start(bool bSuspend = false);


        /**
        * @brief
        * 当线程终止时的回调函数，可以继承此函数实现用户自己的功能
        * @return void
        */
        virtual void SDAPI OnTerminated()
        {
        }

        /**
        * @brief
        * 等待线程停止
        * @return void
        */
        void SDAPI Wait();

        /**
        * @brief
        * 强制线程终止
        * @return void
        */
        void SDAPI Terminate();

        /**
        * @brief
        * 挂起线程，此方法只在Windows操作系统中可用
        * @return void
        */
        void SDAPI Suspend();

        /**
        * @brief
        * 回复线程，此方法只在Windows操作系统中可用
        * @return void
        */
        void SDAPI Resume();

        /**
        * @brief
        * 设置线程属性
        * @param pAttr : 设置的属性结构体
        * @return void
        */
        void SDAPI SetAttribute(SThreadAttr *pAttr);

        /**
        * @brief
        * 获取线程属性
        * @return 返回获取的线程属性
        */
        SThreadAttr* SDAPI GetAttribute();

	protected:
        /**
        * @brief
        * 线程类的线程执行函数体. 用户需要重载此函数运行自己的线程函数
        * 如果用户调用Start(bSuspend)来启动线程，CSDThread类将会调用ThrdProc执行线程
        * @return void
        */
        virtual void SDAPI ThrdProc()
        {
        }
    protected:

        static SDTHREAD_DECLARE(SDThreadFunc) (void * arg); 
        CSDThread (const CSDThread&);
        CSDThread& operator = (const CSDThread&);

        bool m_bstart;
        SDTHREADID m_tid;
        SDHANDLE m_thread_handle;
        void* m_arg;
    };


    
}//

#endif


