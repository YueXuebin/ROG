/*
 *	命名信号量
 */
#ifndef sdnamedsemaphore_h__
#define sdnamedsemaphore_h__

#ifdef LINUX
#include <semaphore.h>
#include <fcntl.h>
#include <errno.h>
#endif


namespace Crown
{
    class CSDNamedSemaphore;
    class CSDNamedSemaphoreObject;

#define SEMA_OK         0
#define SEMA_TIMEOUT    1
#define SEMA_OTHER      2

    // 信号量对象,自动析构
    class CSDNamedSemaphoreObject
    {
    public:
        CSDNamedSemaphoreObject();
        CSDNamedSemaphoreObject(const std::string& name, bool AutoCreate = false);
        CSDNamedSemaphoreObject(const CSDNamedSemaphoreObject& semObj);
        ~CSDNamedSemaphoreObject();

        void operator = (const CSDNamedSemaphoreObject&);

        bool Create();
        bool Open();
        void Release();

        bool IsValid() const {return m_pSem != NULL;}

    private:
        CSDNamedSemaphore*  m_pSem;
        std::string     m_name;
    };

    CSDNamedSemaphore* CreateNamedSemaphore(const char* name, int32 initCount = 0);
    CSDNamedSemaphore* OpenNamedSDSemaphore(const char* name);
    void DestoryNamedSemaphore(CSDNamedSemaphore* pSem);

    // 跨平台的信号量
    class CSDNamedSemaphore
    {
        friend class CSDNamedSemaphoreObject;
        friend CSDNamedSemaphore* CreateNamedSemaphore(const char* name, int32 initCount);
        friend CSDNamedSemaphore* OpenNamedSDSemaphore(const char* name);
        friend void DestoryNamedSemaphore(CSDNamedSemaphore* pSem);

        public:
            static CSDNamedSemaphore* open(const char* name);

            const std::string& GetName() {return m_name;}

            void    Post();             // 递增信号值
            int32   Wait(int32 wait_time = -1);             // 递减信号值(wait_time(毫秒) -1表示无限)

        private:
            CSDNamedSemaphore(const char* name, int32 initCount = 0);
#ifdef WIN
            CSDNamedSemaphore(const char* name, HANDLE hSem);
#else
            CSDNamedSemaphore(const char* name, sem_t* hSem);
#endif
            ~CSDNamedSemaphore();
        
            bool IsValid();
            
        private:
            std::string         m_name;

#ifdef WIN
            HANDLE              m_hSem;
#else
            sem_t*              m_hSem;
#endif
            int32               m_RefCount;         // 引用计数(供信号量对象使用)

    };

}
 



#endif // sdnamedsemaphore_h__
