#include "SDuPCH.h"
#include "sdfilemapping.h"
#include "sdfile.h"

#if !(defined(WIN32) || defined(WIN64))
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#endif

namespace Crown
{

    bool SDAPI SDFileMapping(SFileMapping& fm, const char* file, uint32 beg, uint32 size)
    {
#if (defined(WIN32) || defined(WIN64))
        HANDLE filehandle = CreateFileA(file, GENERIC_READ | GENERIC_WRITE,
                                        FILE_SHARE_READ|FILE_SHARE_WRITE, nullptr, OPEN_EXISTING,
                                        FILE_ATTRIBUTE_HIDDEN, nullptr);
        if (filehandle == INVALID_HANDLE_VALUE)
        {
            return false;
        }

        fm.maphandle = CreateFileMappingA(filehandle, nullptr,
                                          PAGE_READWRITE, 0, size, file);

        if (fm.maphandle == nullptr)
        {
            CloseHandle(filehandle);
            return false;
        }

        fm.mem = MapViewOfFile(fm.maphandle, FILE_MAP_READ|FILE_MAP_WRITE, 0, beg, size);
        fm.pos = fm.mem;
        if (fm.mem == nullptr)
        {
            CloseHandle(filehandle);
            CloseHandle(fm.maphandle);

            return false;
        }

        if (size == 0)
        {
            SFileStatus filestatus;
            if (SDGetFileStatus(file, filestatus) == false)
            {
                return false;
            }
            size = filestatus.size - beg;
        }

        CloseHandle(filehandle);
#else
        if (size == 0)
        {
            SFileStatus filestatus;
            if (SDGetFileStatus(file, filestatus) == false)
            {
                return false;
            }
            size = filestatus.size - beg;
        }

        int fd = open(file, O_RDWR);
        if (fd < 0)
        {
            return false;
        }

        fm.mem = mmap(nullptr, size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, beg);
        if (fm.mem == MAP_FAILED)
        {
            printf("%d:%s\n", errno,strerror(errno));
            close(fd);
            return false;
        }

        close(fd);
#endif
        fm.size = size;

        return true;
    }

    void SDAPI SDFileUnMapping(SFileMapping& fm)
    {
#if (defined(WIN32) || defined(WIN64))
        UnmapViewOfFile(fm.mem);
        CloseHandle(fm.maphandle);
#else
        munmap(fm.mem, fm.size);
#endif
    }

    void SDAPI SDFileFlushMapping(void* mem, uint32 len)
    {
#if (defined(WIN32) || defined(WIN64))
        FlushViewOfFile(mem, len);
#else
        msync(mem, len, 0);
#endif
    }

}
