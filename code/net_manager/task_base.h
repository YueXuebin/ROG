//! @file task_base.h
//! @brief 线程


#ifndef _TASK_BASE_H_
#define _TASK_BASE_H_

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <process.h>
#else
#include <pthread.h>
#endif

namespace utils {


    //! @class Task_Base
    //! @brief 线程类
    class Task_Base
    {
    public:
        Task_Base();
        virtual ~Task_Base();

        //! 启动线程
        int activate();

        //! 等待线程
        int wait();

        //! 线程函数
        virtual int svc() = 0;

    private:
        //! 线程函数
#ifdef WIN32
        static unsigned __stdcall thread_proc(void* param);
#else
        static void* thread_proc(void* param);
#endif

    private:
        //! 线程id
#ifdef WIN32
        HANDLE m_thread;
#else
        pthread_t m_thread;
#endif
    };


} // namepsace utils

#endif // _TASK_BASE_H_
