#include "SDuPCH.h"
#include <stdio.h>
#include "sdstring.h"
#include "sdshmem.h"
#ifdef WIN
#include <Windows.h>
#else
#include <sys/types.h>
#include <sys/shm.h>
#include <stddef.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#endif

namespace Crown
{

#if (defined(WIN32) || defined(WIN64))
    static void ThansPath(std::string& file)
    {
        for (std::string::iterator itr = file.begin(); itr != file.end(); ++itr)
        {
            if (*itr == '/')
            {
                *itr = '\\';
            }
        }
    }
#else
    static void ThansPath(std::string& file)
    {
        for (std::string::iterator itr = file.begin(); itr != file.end(); ++itr)
        {
            if (*itr == '\\')
            {
                *itr = '/';
            }
        }
    }
#endif

    static bool SDAttachShm(SShmem *shm, const char *file)
    {

        std::string path = file;
        ThansPath(path);
#if (defined(WIN32) || defined(WIN64))

        shm->handle = OpenFileMappingA(FILE_MAP_READ|FILE_MAP_WRITE, false, path.c_str());
        if (shm->handle == nullptr)
        {
            return false;
        }

        shm->mem = MapViewOfFile(shm->handle, FILE_MAP_READ|FILE_MAP_WRITE,0,0,0);

        if (shm->mem == nullptr)
        {
            CloseHandle(shm->handle);
            return false;
        }
#else
        key_t shmkey = ftok(path.c_str(), 'a');
        if(shmkey == -1)
        {
            return false;
        }

        struct stat st;
        if (stat(file, &st) == -1)
        {
            return false;
        }

        int id = shmget(shmkey, st.st_size, SHM_R | SHM_W);
        if (id == -1)
        {
            return false;
        }

        shm->mem = shmat(id, nullptr, 0);
        if(shm->mem == (void*)-1)
        {
            return false;
        }
        shm->handle = id;
#endif
        shm->file = path;

        return true;
    }

    static bool SDCreateShm(SShmem *shm, const char *file, uint32 size)
    {
        std::string path = file;
        ThansPath(path);

#if (defined(WIN32) || defined(WIN64))

        HANDLE filehandle = CreateFileA(file, GENERIC_READ | GENERIC_WRITE,
                                        FILE_SHARE_READ|FILE_SHARE_WRITE, nullptr, CREATE_ALWAYS,
                                        FILE_ATTRIBUTE_HIDDEN, nullptr);
        if (filehandle == INVALID_HANDLE_VALUE)
        {
            return false;
        }

        SetFilePointer(filehandle, size, nullptr, FILE_BEGIN);

        shm->handle = CreateFileMappingA(filehandle, nullptr,
                                         PAGE_READWRITE, 0, size, path.c_str());

        if (shm->handle == nullptr)
        {
            CloseHandle(filehandle);
            return false;
        }

        shm->mem = MapViewOfFile(shm->handle, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, 0);

        if (shm->mem == nullptr)
        {
            CloseHandle(filehandle);
            CloseHandle(shm->handle);
            return false;
        }

        CloseHandle(filehandle);

#else
        FILE *fp = fopen(path.c_str(), "w+");
        if (file == nullptr)
        {
            return false;
        }
        fclose(fp);

        size += 4;
        if (truncate(file, size) == -1)
        {
            return false;
        }

        key_t shmkey = ftok(file, 'a');
        if(shmkey == -1)
        {
            return false;
        }

        int id = shmget(shmkey, size, SHM_R | SHM_W | IPC_CREAT);
        if (id == -1)
        {
            return false;
        }

        shm->mem = shmat(id, nullptr, 0);
        if(shm->mem == (void*)-1)
        {
            shmctl(id, IPC_RMID, nullptr);
            return false;
        }
        shm->handle = id;

#endif

        shm->file = path;

        return true;
    }

    SShmem SDAPI SDOpenShmem( const char *file, bool /*bCreate*/,uint32 size  )
    {
        if (SDShmemExist(file))
        {
            SShmem handle;
            SDAttachShm(&handle, file);
            return handle;
        }
        else
        {
            return SDCreateShmem(file, size);
        }
    }

    SShmem SDAPI SDCreateShmem(const char *pFile, uint32 size )
    {
        SShmem handle;
        SDCreateShm(&handle, pFile, size);
        return handle;
    }

    bool SDAPI SDShmemExist(const char *file)
    {
        std::string path = file;
        ThansPath(path);
#if (defined(WIN32) || defined(WIN64))

        SDHANDLE handle= OpenFileMappingA(FILE_MAP_READ, false, path.c_str());
        bool ret = (handle!= nullptr);
        CloseHandle(handle);

        return ret;
#else
        FILE *fp = fopen(path.c_str(), "w+");
        bool ret = (fp !=nullptr);
        fclose(fp);
        return ret;
#endif // 

    }

    void SDAPI SDCloseShmem(SShmem *shm)
    {
        if (shm != nullptr)
        {

#if (defined(WIN32) || defined(WIN64))
            UnmapViewOfFile(shm->mem);
            CloseHandle(shm->handle);
            DeleteFileA(shm->file.c_str());
#else
            shmdt(shm->mem);
            shmctl(shm->handle, IPC_RMID, nullptr);
            unlink(shm->file.c_str());
#endif
        }
    }

    CSDShmem::CSDShmem()
    {
        m_shm.mem    = nullptr;
        m_shm.handle = SDINVALID_HANDLE;
    }

    void* SDAPI CSDShmem::Open(const char *file)
    {
        m_shm = SDOpenShmem(file);
        return m_shm.mem;
    }

    void* SDAPI CSDShmem::Create(const char *file, uint32 size )
    {
        m_shm = SDCreateShmem(file,size);
        return m_shm.mem;
    }

    void SDAPI CSDShmem::Close()
    {
        SDCloseShmem(&m_shm);
    }

    SDHANDLE CSDShmem::GetHandle()
    {
        return m_shm.handle;
    }

} // namespace Crown

