#include "SDuPCH.h"
#include "Assertion.h"
#include <cerrno>
#include "sdmutex.h"
#include "sdlock.h"

#if defined(WIN32)
#  include <windows.h>
#  include <winbase.h>
#else
#  include <pthread.h>
#endif

namespace Crown
{



    bool SDMutexInit(SDMutexId & mid)
    {
#ifdef WIN32
        // Note:  Windows Critical sections are recursive in nature and perhaps
        //        this implementation calls for a non-recursive implementation
        //        (since there also exists a RecursiveMutex class).  The effort
        //        to make this non-recursive just doesn't make sense though. (SLG)
        InitializeCriticalSection(&mid);
        return true;
#else
        int  rc = pthread_mutex_init(&mid,0);
        CnVerify( rc == 0 );
        return rc == 0;
#endif
    }

    void SDMutexLock(SDMutexId & mid)
    {
#ifdef WIN32
        EnterCriticalSection(&mid);
#else
        int  rc = pthread_mutex_lock(&mid);
        CnVerify( rc != EINVAL );
        CnVerify( rc != EDEADLK );
        CnVerify( rc == 0 );
#endif
    }

    void SDMutexUnlock(SDMutexId & mid)
    {
#ifdef WIN32
        LeaveCriticalSection(&mid);
#else
        int  rc = pthread_mutex_unlock(&mid);
        CnVerify( rc != EINVAL );
        CnVerify( rc != EPERM );
        CnVerify( rc == 0 );
#endif
    }

    bool SDMutexUninit(SDMutexId &mid)
    {
#ifdef WIN32
        DeleteCriticalSection(&mid);
        return 0;
#else
        int  rc = pthread_mutex_destroy(&mid);
        CnVerify( rc != EBUSY );  // currently locked
        CnVerify( rc == 0 );
        return rc == 0;
#endif
    }

    CSDMutex::CSDMutex()
    {
        SDMutexInit(m_mutexId);
    }


    CSDMutex::~CSDMutex ()
    {
        SDMutexUninit(m_mutexId);
    }

    void SDAPI CSDMutex::Lock()
    {
        SDMutexLock(m_mutexId);
    }

    void SDAPI CSDMutex::Unlock()
    {
        SDMutexUnlock(m_mutexId);
    }

    CSDRWMutex::CSDRWMutex() :
        m_readerCount(0),
        m_hasWriterLock(false),
        m_pendingWriteCount(0)
    {

    }


    CSDRWMutex::~CSDRWMutex()
    {
    }


    void CSDRWMutex::LockRead()
    {
        CSDLock<CSDMutex> guard(m_mutex);
        while ( m_hasWriterLock || m_pendingWriteCount > 0 )
        {
            m_readCond.Wait(m_mutex);
        }
        m_readerCount++;
    }

    void CSDRWMutex::LockWrite()
    {
        CSDLock<CSDMutex> lock(m_mutex);

        m_pendingWriteCount++;

        while ( m_hasWriterLock || m_readerCount > 0 )
        {
            m_pendingWriteCond.Wait(m_mutex);
        }

        m_pendingWriteCount--;
        m_hasWriterLock = true;
    }


    void CSDRWMutex::Lock()
    {
        LockWrite();
    }


    void CSDRWMutex::Unlock()
    {
        CSDLock<CSDMutex>   lock(m_mutex);
        // Unlocking a write lock.
        //
        if ( m_hasWriterLock )
        {
            CnVerify( m_readerCount == 0 );

            m_hasWriterLock = false;

            // Pending writers have priority. Could potentially starve readers.
            //
            if ( m_pendingWriteCount > 0 )
            {
                m_pendingWriteCond.Signal();
            }

            // No writer, no pending writers, so all the readers can go.
            //
            else
            {
                m_readCond.Broadcast();
            }
        }

        // Unlocking a read lock.
        //
        else
        {
            CnVerify( m_readerCount > 0 );
            m_readerCount--;

            if ( m_readerCount == 0 && m_pendingWriteCount > 0 )
            {
                m_pendingWriteCond.Signal();
            }
        }
    }

    unsigned int CSDRWMutex::ReaderCount() const
    {
        return ( m_readerCount );
    }

    unsigned int CSDRWMutex::PendingWriterCount() const
    {
        return ( m_pendingWriteCount );
    }


    CSDRecursiveMutex::CSDRecursiveMutex()
    {
#ifndef WIN32
        int rc = pthread_mutexattr_init(&m_mutexAttr);
#if defined(__linux__)
        pthread_mutexattr_settype(&m_mutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
#else
        pthread_mutexattr_settype(&m_mutexAttr, PTHREAD_MUTEX_RECURSIVE);
#endif

        rc = pthread_mutex_init(&m_mutexId, &m_mutexAttr);
        CnVerify( rc == 0 );
#else
        InitializeCriticalSection(&m_mutexId);
#endif
    }


    CSDRecursiveMutex::~CSDRecursiveMutex ()
    {
        SDMutexUninit(m_mutexId);

    }

    void CSDRecursiveMutex::Lock()
    {
        SDMutexLock(m_mutexId);
    }

    void CSDRecursiveMutex::Unlock()
    {
        SDMutexUnlock(m_mutexId);
    }


} // namespace Crown




