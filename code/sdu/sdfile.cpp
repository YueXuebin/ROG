#include "SDuPCH.h"
#include "sdtype.h"
#include "sdfile.h"
#include "sdstring.h"
#include "sddir.h"
#include "sdmath.h"
#include "sdfilemapping.h"

#include <sys/stat.h>
#if (defined(WIN32) || defined(WIN64))
#include <direct.h>
#include <io.h>
#pragma warning(disable : 4996)
#else
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>
#include <fcntl.h>
#endif

#define SDPATH_IS_DELIMITER(x)  (x == '\\' || x == '/')

namespace Crown
{

    CSDFile::CSDFile()
        :m_pFileHandle(nullptr), m_bFileMapping(false)
    {
    }

    CSDFile::~CSDFile()
    {
        Close();
    }


    bool SDAPI CSDFile::Open(const char* szfilename, const char* type, bool bFileMapping)
    {
        m_filepath = szfilename;
        m_bFileMapping = bFileMapping;
        if (m_bFileMapping)
        {
            m_pFileHandle = CnNew SFileMapping;
            return SDFileMapping(*(SFileMapping*)m_pFileHandle,szfilename);
        }
        else
        {
            m_pFileHandle = fopen(szfilename, type);
        }
        return m_pFileHandle != nullptr;
    }

    void SDAPI CSDFile::Close()
    {
        if (m_pFileHandle != nullptr)
        {
            if (m_bFileMapping)
            {
                SDFileUnMapping(*(SFileMapping*)m_pFileHandle);
                CnDelete (SFileMapping*)m_pFileHandle;
            }
            else
            {
                fclose((FILE*)m_pFileHandle);
            }
            m_pFileHandle = nullptr;
        }
    }

    void SDAPI CSDFile::Flush()
    {
        if (m_pFileHandle != nullptr)
        {
            if (m_bFileMapping)
            {
                SFileMapping* pFileMapping = (SFileMapping*) m_pFileHandle;
                SDFileFlushMapping(pFileMapping->mem, pFileMapping->size);
            }
            else
            {
                fflush((FILE*)m_pFileHandle);
            }
        }
    }

    uint32 SDAPI CSDFile::Read(void* pBuf, uint32 nLen)
    {
        if (m_pFileHandle == nullptr)
        {
            return 0;
        }

        if (m_bFileMapping)
        {
            SFileMapping* pFileMapping = (SFileMapping*) m_pFileHandle;
            uint32 left = pFileMapping->size -  ( (char*)pFileMapping->pos - (char*)pFileMapping->mem);
            uint32 readable =SDMin(left,nLen);
            memcpy(pBuf, pFileMapping->pos ,readable);
            pFileMapping->pos = (char*) pFileMapping->pos +  readable;
            return readable;
        }
        else
        {
            return fread(pBuf, 1, nLen, (FILE*)m_pFileHandle);
        }
    }

    uint32 SDAPI CSDFile::Write(const void* pBuf, uint32 nLen)
    {
        if (m_pFileHandle == nullptr)
        {
            return false;
        }

        if (m_bFileMapping)
        {
            SFileMapping* pFileMapping = (SFileMapping*) m_pFileHandle;
            uint32 left = pFileMapping->size -  ( (char*)pFileMapping->pos - (char*)pFileMapping->mem);
            uint32 writeable =SDMin(left,nLen);
            memcpy(pFileMapping->pos,pBuf,writeable);
            pFileMapping->pos = (char*) pFileMapping->pos +  writeable;
            return writeable;
        }
        else
        {
            return fwrite(pBuf, 1, nLen,(FILE*) m_pFileHandle);
        }
    }

    int32 SDAPI CSDFile::Seek( int32 offset, CSDFile::SeekOffset whence)
    {
        if (m_pFileHandle == nullptr)
        {
            return -1;
        }
        if (m_bFileMapping)
        {
            SFileMapping* pFileMapping = (SFileMapping*) m_pFileHandle;
            switch(whence)
            {
            case SK_SET:
                if ((uint32)abs(offset) <= pFileMapping->size)
                {
                    pFileMapping->pos = (char*)pFileMapping->mem +
					(offset >0 ? offset : int32(pFileMapping->size) + offset);
                    return 0;
                }
                break;

            case SK_END:
                if ((uint32)abs(offset) <= pFileMapping->size)
                {
                    pFileMapping->pos = (char*)pFileMapping->mem +
					(offset> 0 ? int32(pFileMapping->size) - offset: -offset) ;
                    return 0;
                }
                break;

            case SK_CUR:
            {
		int32 curPos =  (char*)pFileMapping->pos - (char*)pFileMapping->mem;
                if ( (offset >=0  && offset <= (int32)(pFileMapping->size - curPos))
			|| (offset < 0 && offset + curPos >=0 ))
                {
                    pFileMapping->pos = (char*)pFileMapping->pos + offset;
                    return 0;
                }
            }
            break;
            default:
            break;
            }

            return -1;
        }
        else
        {
            return fseek((FILE*)m_pFileHandle, offset, whence);
        }
    }

    int32 SDAPI CSDFile::SeekToBegin()
    {
        return this->Seek(0, SK_SET);
    }

    int32 SDAPI CSDFile::SeekToEnd()
    {
        return this->Seek(0, SK_END);
    }

    int32 SDAPI CSDFile::GetPosition()
    {
        if (m_pFileHandle == nullptr)
        {
            return -1;
        }
        if (m_bFileMapping)
        {
            SFileMapping* pFileMapping = (SFileMapping*) m_pFileHandle;
            return (char*)pFileMapping->pos - (char*)pFileMapping->mem;
        }
        else
        {
            return ftell((FILE*)m_pFileHandle);
        }

    }

    void SDAPI CSDFile::SetLength(int32 newLen)
    {
        if (m_pFileHandle == nullptr)
        {
            return;
        }
        if (m_bFileMapping)
        {
            SFileMapping* pFileMapping = (SFileMapping*) m_pFileHandle;
            SDFileUnMapping(*pFileMapping);
            SDFileMapping(*pFileMapping,m_filepath.c_str(),0,newLen);
        }
        else
        {

#if (defined(WIN32) || defined(WIN64))
            _chsize(_fileno((FILE*)m_pFileHandle), newLen);
#else
            ftruncate(fileno((FILE*)m_pFileHandle), newLen);
#endif
        }
    }

    bool SDAPI CSDFile::Eof()
    {
        if (m_pFileHandle == nullptr)
        {
            return false;
        }

        if (m_bFileMapping)
        {
            SFileMapping* pFileMapping = (SFileMapping*) m_pFileHandle;
            return pFileMapping->pos == ((char*)pFileMapping->mem + pFileMapping->size);
        }
        else
        {
            int32 thispos = GetPosition();
            SeekToEnd();
            int32 endpos = GetPosition();
            if(thispos != endpos)
            {
                Seek(thispos, SK_SET);
                return false;
            }
        }
        return true;
    }

    bool SDAPI CSDFile::GetFileStatus(SFileStatus &fStatus)
    {
        if (m_pFileHandle == nullptr)
        {
            return false;
        }

        return  SDGetFileStatus(m_filepath.c_str(), fStatus);
    }

    bool SDAPI SDGetFileStatus( const char* szfilename, SFileStatus& fStatus )
    {
        if(nullptr == szfilename)
        {
            return false;
        }
#if (defined(WIN32) || defined(WIN64))
        struct _stat st;
        if (_stat(szfilename, &st) != 0)
        {
            return false;
        }
#else
        struct stat st;
        if (stat(szfilename, &st) < 0)
        {
            return false;
        }
#endif

        fStatus.gid = st.st_gid;
        fStatus.atime = st.st_atime;
        fStatus.ctime = st.st_ctime;
        fStatus.dev = st.st_dev;
        fStatus.inode = st.st_ino;
        fStatus.mode = st.st_mode;
        fStatus.mtime = st.st_mtime;
        fStatus.nlink = st.st_nlink;
        fStatus.rdev = st.st_rdev;
        fStatus.size = st.st_size;
        fStatus.uid = st.st_uid;

        return true;
    }

    bool SDAPI SDFileRemove( const char* szfilename )
    {
#if (defined(WIN32) || defined(WIN64))
        return _unlink(szfilename) == 0;
#else
        return unlink(szfilename) == 0;
#endif
    }

    int32 SDAPI SDFileRename( const char* oldname, const char* newname ,bool bForce)
    {
        if (bForce)
        {
            if (SDFileExists(newname))
            {
                SDFileRemove(newname);
            }
            return rename(oldname, newname);
        }
        else
        {
            return rename(oldname, newname);
        }
    }
// 增加强制创建目标目录  是否覆盖目标文件 增加文档注释
    int32 SDAPI SDFileMove( const char* oldname, const char* newname ,bool bForce   )
    {
        return SDFileRename(oldname,newname,bForce);

        //TODO 实现文件跨目录拷贝,或移动.
        if ( SDIsDirectory(oldname))
        {
            uint32 newNameLen = strlen(newname);
            if (SDPATH_IS_DELIMITER(newname[newNameLen-1]))
            {

            }
            return SDFileRename(oldname,newname,bForce);
        }
        else
        {
            return SDFileRename(oldname,newname,bForce);
        }
    }

    int32 SDAPI SDFileAccess( const char *path, AccessMode mode )
    {
        if(nullptr == path)
        {
            return -1;
        }
#if (defined(WIN32) || defined(WIN64))
        return _access(path, mode);
#else
        return access(path, mode);
#endif
    }


    const char* SDAPI SDGetModulePath(void)
    {
    	// FIXME: 这里的实现有误，Linux下，得到的是当前工作目录，而不是模块所在目录。
		static char szPath[SD_MAX_PATH];

		static bool bFirstTime = true;

		if (bFirstTime)
		{
#if (defined(WIN32) || defined(WIN64))
			bFirstTime = false;
			GetModuleFileNameA(nullptr, (char*)szPath, sizeof(szPath));
			char *p = strrchr(szPath, '\\');
			*p = '\0';
			strcat(szPath, "\\");
#else
			char szTmp[SD_MAX_PATH];
			getcwd(szTmp,sizeof(szTmp));
			sprintf(szPath,"%s//",szTmp);
#endif //
		}
		return szPath ;
    }

    const char* SDAPI SDGetModuleName(void)
    {
#if (defined(WIN32) || defined(WIN64))
        static char szPath[MAX_PATH];
        static bool bFirstTime = true;

        if(bFirstTime)
        {
            bFirstTime = false;
            GetModuleFileNameA(nullptr, (char*)szPath, sizeof(szPath));
        }

        return szPath;
#else
        return "./";
#endif
    }

    const char* SDAPI SDGetWorkPath()
    {
#if (defined(WIN32) || defined(WIN64))
        return _getcwd(nullptr,0);
#else
        static char workPath[SD_MAX_PATH] ;
        getcwd(workPath,sizeof(workPath));
        return workPath;
#endif
    }

    std::string  SDAPI SDFileExtractPath(const char* strFileName)
    {
        if(nullptr == strFileName)
        {
            return "";
        }
        std::string::size_type pos = std::string(strFileName).rfind('\\');
        if (pos == std::string::npos)
        {
            return "";
        }

        return std::string(strFileName).substr(0, pos);
    }

    std::string  SDAPI SDFileExtractName(const char* strFileName)
    {
        if(nullptr == strFileName)
        {
            return "";
        }
        std::string resFileName = strFileName;
        std::string::size_type pos = resFileName.rfind('\\');
        if (pos != std::string::npos)
        {
            resFileName = resFileName.substr(pos + 1);
        }
        pos = resFileName.rfind('/');
        if (pos != std::string::npos)
        {
            resFileName = resFileName.substr(pos + 1);
        }

        return resFileName;
    }

    std::string  SDAPI SDFileExtractExt(const char* strFileName)
    {
        if(nullptr == strFileName)
        {
            return "";
        }
        std::string::size_type pos = std::string(strFileName).find_last_of(":.\\");
        if (pos == std::string::npos || strFileName[pos] != '.')
        {
            return "";
        }
        return std::string(strFileName).substr(pos + 1);
    }

    std::string  SDAPI SDFileExcludeTrailingDelimiter(const char* strPath)
    {
        if(nullptr == strPath)
        {
            return "";
        }
        int len = strlen(strPath);

        if (len == 0)
        {
            return "";
        }

        if (SDPATH_IS_DELIMITER(strPath[len - 1]))
        {
            return std::string(strPath).substr(0, len - 1);
        }
        else
        {
            return strPath;
        }
    }

    std::string  SDAPI SDFileExcludeLeadingDelimiter(const char* strPath)
    {
        if(nullptr == strPath)
        {
            return "";
        }
        size_t pos = 0;
        size_t len = strlen(strPath);
        while (pos < len )
        {
            if (!SDPATH_IS_DELIMITER(strPath[pos]))
            {
                break;
            }
            pos++;
        }
        return std::string(strPath).substr(pos);
    }

    std::string SDAPI SDFileIncludeTrailingDelimiter(const char* strPath)
    {
        if(nullptr == strPath)
        {
            return "";
        }
        int len = strlen(strPath);
        if (len == 0 )
        {
            return "";
        }
        if (!SDPATH_IS_DELIMITER(strPath[len - 1]))
        {
            return std::string(strPath) + '\\';
        }
        else
        {
            return strPath;
        }
    }

    std::string  SDAPI SDFileIncludeLeadingDelimiter(const char* strPath)
    {
        if(nullptr == strPath)
        {
            return "";
        }
        if (strlen(strPath) == 0)
        {
            return "\\";
        }
        if (!SDPATH_IS_DELIMITER(strPath[0]))
        {
            return std::string("\\") + strPath;
        }
        else
        {
            return strPath;
        }
    }

    std::string SDAPI SDFileReplaceDelimiter(const char * strPath,char delimiter)
    {
        std::string path = strPath;
        for (std::string::iterator itr = path.begin(); itr != path.end(); ++itr)
        {
            if (SDPATH_IS_DELIMITER(*itr))
            {
                *itr = delimiter;
            }
        }
        return path;
    }

    bool SDAPI SDFileExists(const char* strFileName)
    {
        FILE* fp = fopen(strFileName, "r");
        if (fp == nullptr)
        {
            return false;
        }

        fclose(fp);

        return true;
    }

    int32 SDAPI SDFileGetSize(const char* strFileName)
    {
#if (defined(WIN32) || defined(WIN64))
        struct _stat st;
        if (_stat(strFileName, &st) < 0)
        {
            return -1;
        }
#else
        struct stat st;
        if (stat(strFileName, &st) < 0)
        {
            return -1;
        }
#endif

        return st.st_size;
    }

    bool SDAPI SDFileCreate(const char* strFileName)
    {
        if (SDFileExists(strFileName))
        {
            return false;
        }

        std::string strPath = SDFileExtractPath(strFileName);
        if (!SDCreateDirectory(strPath.c_str()))
        {
            return false;
        }

        FILE* fp = fopen(strFileName, "a+");
        if (nullptr == fp)
        {
            return false;
        }

        fclose(fp);

        return true;
    }

	bool SDAPI SDFileCopy(const char * pszExistFile, const char * pNewFile ,bool bFailIfExists)
	{
#if (defined(WIN32) || defined(WIN64))
		return CopyFileA(pszExistFile,pNewFile,bFailIfExists) ? true : false; 
#else 
		int fps, fpd;
		struct stat statbuf;
		void *bufs, *bufd;

		if (SDFileExists(pNewFile))
		{
			if (bFailIfExists)
			{
				return false; 
			}
			else 
			{
				SDFileRemove(pNewFile); 
			}
		}

		if((fps = open(pszExistFile, O_RDONLY)) == -1) 
		{
			return false; 
		}

		if((fpd = open(pNewFile, O_RDWR | O_CREAT | O_TRUNC, 0644)) == -1) 
		{
			return false; 
		}

		if(fstat(fps, &statbuf) == -1) 
		{
			return false; 
		}

		if(pwrite(fpd, "", 1, statbuf.st_size - 1) != 1) 
		{
			return false; 
		}

		if((bufs = mmap(0, statbuf.st_size, PROT_READ, MAP_SHARED, fps, 0)) == MAP_FAILED) 
		{
			return false; 
		}

		if((bufd = mmap(0, statbuf.st_size, PROT_WRITE, MAP_SHARED, fpd, 0)) == MAP_FAILED) 
		{
			return false; 
		}

		memcpy (bufd, bufs, statbuf.st_size);

		munmap(bufd, statbuf.st_size);
		munmap(bufs, statbuf.st_size);

		close(fpd);
		close(fps);

		return true; 
#endif //		
	}
}
