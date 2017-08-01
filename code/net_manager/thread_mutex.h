//! @file thread_mutex.h
//! @brief 线程锁


#ifndef _THREAD_MUTEX_H_
#define _THREAD_MUTEX_H_


#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
  #include <windows.h>
#else
  #include <pthread.h>
#endif


namespace utils {


//! @class Thread_Mutex
//! @brief 线程锁
class Thread_Mutex
{
public:
	Thread_Mutex();
	~Thread_Mutex();

	//! 加锁
	//! @return 0:成功, <0:失败
	int acquire();

	//! 解锁
	//! @return 0:成功, <0:失败
	int release();

private:
	//! 线程锁
#ifdef WIN32
	CRITICAL_SECTION m_thread_section;
#else
	pthread_mutex_t m_thread_mutex;
#endif
};


//! @class Thread_Mutex_Guard
//! @brief 线程锁工具类
//!
//! 此类在构造函数加锁, 析构函数解锁
class Thread_Mutex_Guard
{
public:
	//! 构造函数
	//! @param mutex 用到的线程锁
	Thread_Mutex_Guard(Thread_Mutex& mutex);

	//! 析构函数
	~Thread_Mutex_Guard();

private:
	Thread_Mutex& m_mutex;
};


} // namepsace utils


#endif // _THREAD_MUTEX_H_
