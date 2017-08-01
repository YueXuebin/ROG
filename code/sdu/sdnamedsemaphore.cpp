#include "SDuPCH.h"
#include "sdnamedsemaphore.h"
#include "sdtime.h"

namespace Crown
{
    CSDNamedSemaphoreObject::CSDNamedSemaphoreObject() :
    m_pSem(NULL)
    {

    }


    CSDNamedSemaphoreObject::CSDNamedSemaphoreObject(const std::string& name, bool AutoCreate) :
    m_pSem(NULL)
    {
        m_name = name;
        if(AutoCreate)
            Create();
    }

    CSDNamedSemaphoreObject::CSDNamedSemaphoreObject(const CSDNamedSemaphoreObject& semObj) :
    m_pSem(NULL)
    {
        if(semObj.IsValid())
        {
            m_pSem = semObj.m_pSem;
            m_pSem->m_RefCount++;

            m_name = m_pSem->GetName();
        }
    }

    bool CSDNamedSemaphoreObject::Create()
    {
        if(m_pSem)
            return true;

        if(m_name.length() == 0)
            return false;

        m_pSem = CreateNamedSemaphore(m_name.c_str());
        if(m_pSem)
        {
            m_pSem->m_RefCount++;
            return true;
        }
        else
            return false;
    }

    bool CSDNamedSemaphoreObject::Open()
    {
        if(m_pSem)
            return true;

        if(m_name.length() == 0)
            return false;

        m_pSem = OpenNamedSDSemaphore(m_name.c_str());
        if(m_pSem)
        {
            m_pSem->m_RefCount++;
            return true;
        }
        else
            return false;
    }

    void CSDNamedSemaphoreObject::operator = (const CSDNamedSemaphoreObject& other)
    {
        m_pSem = other.m_pSem;
        if(m_pSem)
            m_pSem->m_RefCount++;
        m_name = other.m_name;
    }

    void CSDNamedSemaphoreObject::Release()
    {
        if(m_pSem)
        {
            m_pSem->m_RefCount--;
            if(m_pSem->m_RefCount <= 0)
            {
                DestoryNamedSemaphore(m_pSem);
            }
            m_pSem = NULL;
            m_name.clear();
        }
    }

    CSDNamedSemaphoreObject::~CSDNamedSemaphoreObject()
    {
        Release();
    }


    CSDNamedSemaphore* CreateNamedSemaphore(const char* name, int32 initCount)
    {
        CSDNamedSemaphore* pExist = OpenNamedSDSemaphore(name);
        if(pExist)
            return NULL;
        
        CSDNamedSemaphore* pSemaphore = CnNew CSDNamedSemaphore(name, initCount);
        if(!pSemaphore->IsValid())
        {
            CnDelete pSemaphore;
            pSemaphore = NULL;
        }

        return pSemaphore;
    }

    CSDNamedSemaphore* OpenNamedSDSemaphore(const char* name)
    {
        return CSDNamedSemaphore::open(name);
    }

    void DestoryNamedSemaphore(CSDNamedSemaphore* pSem)
    {
        CnDelete pSem;
    }

    CSDNamedSemaphore::CSDNamedSemaphore(const char* name, int32 initCount) :
    m_RefCount(0)
    {
        m_name = name;
#ifdef WIN
        m_hSem = CreateSemaphore(NULL, initCount, LONG_MAX, name);
        if(!m_hSem)
        {
            uint32 errorcode = GetLastError();
            printf("error=%d\n", errorcode);
        }
#else
        m_hSem = sem_open(name, O_CREAT|O_EXCL, 666, initCount);
        if(m_hSem == SEM_FAILED)
        {
            printf("error=%d", errno);
        }
#endif

        CnAssert(m_hSem);
    }

#ifdef WIN
    CSDNamedSemaphore::CSDNamedSemaphore(const char* name, HANDLE hSem) :
    m_hSem(hSem),
    m_RefCount(0)
    {
        m_name = name;
        CnAssert(m_hSem);
    }
#else
    CSDNamedSemaphore::CSDNamedSemaphore(const char* name, sem_t* hSem) :
    m_hSem(hSem),
    m_RefCount(0)
    {
        m_name = name;
        CnAssert(m_hSem);
    }
#endif

    bool CSDNamedSemaphore::IsValid()
    {
        return m_hSem != 0;
    }

    CSDNamedSemaphore* CSDNamedSemaphore::open( const char* name )
    {
#ifdef WIN
        HANDLE hSemaphore = OpenSemaphore(SEMAPHORE_ALL_ACCESS, FALSE, name);
        if(hSemaphore)
            return CnNew CSDNamedSemaphore(name, hSemaphore);
#else
        sem_t* hSem = sem_open(name, O_EXCL, 0, 0);
        if(hSem != SEM_FAILED)
            return CnNew CSDNamedSemaphore(name, hSem);
#endif
        return NULL;
    }

    CSDNamedSemaphore::~CSDNamedSemaphore()
    {
#ifdef WIN
        CloseHandle(m_hSem);
#else
        sem_close(m_hSem);
        sem_unlink(m_name.c_str());
#endif
        m_hSem = 0;
        m_name.clear();
    }

    void CSDNamedSemaphore::Post()
    {
#ifdef WIN
        ReleaseSemaphore(m_hSem, 1, NULL);
#else
        sem_post(m_hSem);
#endif
    }

    int32 CSDNamedSemaphore::Wait(int32 wait_time)      // 子线程调用
    {
#ifdef WIN
        int32 ret;
        if(wait_time < 0)
            ret = WaitForMultipleObjects(1, &m_hSem, FALSE, INFINITE);
        else
            ret = WaitForMultipleObjects(1, &m_hSem, FALSE, wait_time);

        if(ret == WAIT_OBJECT_0)
            return SEMA_OK;
        else if(ret == WAIT_TIMEOUT)
            return SEMA_TIMEOUT;
        else
            return SEMA_OTHER;
#else
        int32 ret;
        if(wait_time < 0)
            ret = sem_wait(m_hSem);
        else
        {
            struct timespec ts;
            ts.tv_sec = (uint32)Crown::SDNow().GetTimeValue();
            ts.tv_sec = ts.tv_sec + wait_time/1000;
            ts.tv_nsec = wait_time%1000 * 1000;
            ret = sem_timedwait(m_hSem, &ts);
        }

        if(ret == 0)
            return SEMA_OK;
        else
        {
            if(errno == ETIMEDOUT)
                return SEMA_TIMEOUT;
            else
                return SEMA_OTHER;
        }
#endif
    }



}
