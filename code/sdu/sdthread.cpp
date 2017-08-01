#include "SDuPCH.h"
#include "sdthread.h"
#if (defined(WIN32) || defined(WIN64))
#include <process.h>
#else
#include <pthread.h>
#endif

namespace Crown
{

    SDTHREAD_DECLARE(CSDThread::SDThreadFunc) (void * arg)
    {
        CSDThread *pThread = static_cast<CSDThread*>(arg);
        if (pThread != NULL)
        {
            pThread->ThrdProc();
        }
        return 0;
    }


    SDTHREADID SDAPI SDGetThreadId()
    {
#ifdef WIN32
        return ::GetCurrentThreadId();
#else
        return pthread_self();
#endif  //WIN32
    }

    CSDThread::CSDThread()
        :m_bstart(false)
    {
        m_thread_handle = SDINVALID_HANDLE;
        m_tid = 0;
    }

    CSDThread::~CSDThread()
    {
        if (m_thread_handle != SDINVALID_HANDLE)
        {
            //Terminate();      //  无需调用Terminate，线程应该已经执行完成
            m_thread_handle = SDINVALID_HANDLE;
        }
    }

    bool SDAPI CSDThread::Start(PFThrdProc pthrdProc, void * pArg, bool bSuspend /*= false*/)
    {
        if (!m_bstart)
        {
            m_thread_handle = SDCreateThread(NULL, pthrdProc, pArg, &m_tid, bSuspend);
            if (SDINVALID_HANDLE == m_thread_handle)
            {
                m_bstart = false;
                return false;
            }
            m_bstart = true;
        }
        return true;
    }

    bool SDAPI CSDThread::Start(bool bSuspend)
    {
        if (!m_bstart)
        {
            m_thread_handle = SDCreateThread(NULL, SDThreadFunc, this, &m_tid, bSuspend);
            if (SDINVALID_HANDLE == m_thread_handle)
            {
                return false;
            }
            m_bstart = true;
        }
        return m_bstart;
    }

    void SDAPI CSDThread::Wait()
    {
        if(m_bstart)
        {
            m_bstart = false;
            SDThreadWait(m_thread_handle);
        }
    }

    void SDAPI CSDThread::Terminate()
    {
        if(m_bstart)
        {
            SDThreadTerminate(m_thread_handle);
            this->OnTerminated();
            m_bstart = false;
        }
    }

    void SDAPI CSDThread::Suspend()
    {
        SDThreadSuspend(m_thread_handle);
    }

    void SDAPI CSDThread::Resume()
    {
        SDThreadResume(m_thread_handle);
    }

    SDHANDLE SDAPI SDCreateThread(SThreadAttr * pAttr, PFThrdProc pThrdProc,
        void * pParam, SDTHREADID * pThreadId/* = NULL*/, bool bSuspend /*= false*/)
    {
#if (defined(WIN32) || defined(WIN64))
        int thrdFlag = bSuspend ? CREATE_SUSPENDED : 0;
        // CreateThread(NULL, 0, pThrdProc, pParam, thrdFlag, pThreadId);
        return (SDHANDLE)_beginthreadex(NULL, 0, pThrdProc, pParam, thrdFlag, pThreadId);
#else
        if (0 != pthread_create((pthread_t*)pThreadId, NULL, pThrdProc, pParam))
        {
            return SDINVALID_HANDLE;
        }
        return *pThreadId;
#endif // 
    }

    void SDAPI CSDThread::SetAttribute(SThreadAttr * pAttr)
    {
        SDSetThreadAttr(m_thread_handle, pAttr);
    }

    SThreadAttr *  SDAPI CSDThread::GetAttribute()
    {
        return SDGetThreadAttr(m_thread_handle);
    }

    void SDAPI SDThreadCloseHandle(SDHANDLE handle)
    {
        SDCloseHandle(handle);
    }

    int32  SDAPI SDThreadWait(SDHANDLE handle)
    {
#if (defined(WIN32) || defined(WIN64))
        return ::WaitForSingleObject(handle, INFINITE);
#else
        return pthread_join(handle, NULL);
#endif //
    }

    void SDAPI SDThreadTerminate(SDHANDLE handle)
    {
#if (defined(WIN32) || defined(WIN64))
        TerminateThread(handle, 0);
        CloseHandle(handle);
#else
        pthread_cancel(handle);
#endif
    }

    void SDAPI SDThreadSuspend(SDHANDLE handle)
    {
#if (defined(WIN32) || defined(WIN64))
        SuspendThread(handle);
#endif // 
    }

    void SDAPI SDThreadResume(SDHANDLE handle)
    {
#if (defined(WIN32) || defined(WIN64))
        ResumeThread(handle);
#endif // 
    }


    void SDAPI SDSetThreadAttr(SDHANDLE,SThreadAttr * pAttr)
    {

    }

    SThreadAttr* SDAPI SDGetThreadAttr(SDHANDLE)
    {
        return NULL;
    }



}  //namespace Crown
