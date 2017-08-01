#include "net_manager_pch.h"
#include "task_base.h"
#include "int_types.h"

namespace utils {

    // class Task_Base
    Task_Base::Task_Base()
    {
    }

    Task_Base::~Task_Base()
    {
    }

    int Task_Base::activate()
    {
#ifdef WIN32
        m_thread = (HANDLE)_beginthreadex(NULL, 0, thread_proc, (void*)this, 0, NULL);
        if (0L == m_thread) {
            return -1;
        }
#else
        int rc = pthread_create(&m_thread, NULL, thread_proc, (void*)this);
        if (0 != rc) {
            return rc;
        }
#endif

        return 0;
    }

    int Task_Base::wait()
    {
#ifdef WIN32
        WaitForSingleObject(m_thread, INFINITE);
        CloseHandle(m_thread);
#else
        void *value_ptr;
        int rc = pthread_join(m_thread, &value_ptr);
        if (0 != rc) {
            return rc;
        }
#endif

        return 0;
    }


#ifdef WIN32

    unsigned __stdcall Task_Base::thread_proc(void* param)
    {
        Task_Base *pthis = (Task_Base*)param;

        int code = pthis->svc();

        return (unsigned)code;
    }

#else

    void* Task_Base::thread_proc(void* param)
    {
        Task_Base *pthis = (Task_Base*)param;

        intptr_t code = pthis->svc();

        return (void*)code;
    }

#endif


} // namepsace utils

