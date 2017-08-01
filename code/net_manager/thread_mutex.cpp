#include "net_manager_pch.h"
#include "thread_mutex.h"


namespace utils {


// class Thread_Mutex
Thread_Mutex::Thread_Mutex()
{
#ifdef WIN32
	InitializeCriticalSection(&m_thread_section);
#else
	pthread_mutex_init(&m_thread_mutex, NULL);
#endif
}

Thread_Mutex::~Thread_Mutex()
{
#ifdef WIN32
	DeleteCriticalSection(&m_thread_section);
#else
	pthread_mutex_destroy(&m_thread_mutex);
#endif
}

int Thread_Mutex::acquire()
{
#ifdef WIN32
	EnterCriticalSection(&m_thread_section);
#else
	int rc = pthread_mutex_lock(&m_thread_mutex);
	if (0 != rc) {
		return -1;
	}
#endif

	return 0;
}

int Thread_Mutex::release()
{
#ifdef WIN32
	LeaveCriticalSection(&m_thread_section);
#else
	int rc = pthread_mutex_unlock(&m_thread_mutex);
	if (0 != rc) {
		return -1;
	}
#endif

	return 0;
}




// class Thread_Mutex_Guard
Thread_Mutex_Guard::Thread_Mutex_Guard(Thread_Mutex& mutex) : m_mutex(mutex)
{
	m_mutex.acquire();
}

Thread_Mutex_Guard::~Thread_Mutex_Guard()
{
	m_mutex.release();
}


} // namepsace utils

