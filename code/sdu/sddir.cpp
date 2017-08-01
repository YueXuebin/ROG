#include "SDuPCH.h"
#include "sddir.h"
#include "sdfile.h"
#include "sdstring.h"

#if (defined(WIN32) || defined(WIN64))
#include <direct.h>
#include <errno.h>
#pragma warning(disable : 4996)
#else
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#endif

namespace Crown
{

    bool CSDDirectory::Open( const char* path )
    {
#if (defined(WIN32) || defined(WIN64))
        m_szPath = path;
        m_szPath += "\\*";
        m_fHandle = INVALID_HANDLE_VALUE;
#else
		SDStrncpy(m_curDir, path, sizeof(m_curDir)); 
        m_dir = opendir(path);
        if (m_dir == nullptr)
        {
            return false;
        }
#endif

        return true;
    }

    void CSDDirectory::Close()
    {
#if (defined(WIN32) || defined(WIN64))
        if (m_fHandle != INVALID_HANDLE_VALUE)
        {
            FindClose(m_fHandle);
            m_fHandle = INVALID_HANDLE_VALUE;
        }
#else
        if (m_dir != nullptr)
        {
            closedir(m_dir);
            m_dir = nullptr;
        }
#endif
    }

    bool CSDDirectory::Read( char* name, int32 buflen, SFileAttr* pAttr)
    {
#if (defined(WIN32) || defined(WIN64))
        WIN32_FIND_DATAA find_data;
        if(m_fHandle == INVALID_HANDLE_VALUE)
        {
            m_fHandle = FindFirstFileA(m_szPath.c_str(), &find_data);
            if (m_fHandle == INVALID_HANDLE_VALUE)
            {
                return false;
            }
        }
        else
        {
            if (FindNextFileA(m_fHandle, &find_data) == 0)
            {
                return false;
            }
        }

        if (strcmp(find_data.cFileName, ".") == 0 ||
                strcmp(find_data.cFileName, "..") == 0 )
        {
            return Read(name, buflen, pAttr);
        }

        _snprintf(name, buflen, "%s", find_data.cFileName);

        if (pAttr != nullptr)
        {
            pAttr->isDir = (find_data.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY) ? 1 : 0;
        }

#else
        if (m_dir == nullptr)
        {
            return false;
        }

        struct dirent* pdir = readdir(m_dir);
        if (pdir == nullptr)
        {
            return false;
        }

        if (strcmp(pdir->d_name, ".") == 0 ||
                strcmp(pdir->d_name, "..") == 0 )
        {
            return Read(name, buflen, pAttr);
        }

        snprintf(name, buflen, "%s", pdir->d_name);

        if (pAttr != nullptr )
        {
			char szFullPath[SD_MAX_PATH] ; 
			snprintf(szFullPath, buflen, "%s/%s", m_curDir, pdir->d_name);

            struct stat statbuf;
            if (lstat(szFullPath, &statbuf) < 0 )
            {
                return false;
            }

            pAttr->isDir = S_ISDIR(statbuf.st_mode) ;
        }

#endif

        return true;
    }


    bool SDAPI SDCreateDirectory(const char *dirName, bool bforce)
    {

#if (defined(WIN32) || defined(WIN64))
        int ret = _mkdir(dirName);
#else
        int ret = mkdir(dirName, S_IREAD | S_IWRITE);
#endif
        if (!bforce)
        {
            if (0 != ret)
            {
                return false;
            }
            return true;
        }

        if (0 != ret)
        {
            if(bforce)
            {
				char path[SD_MAX_PATH] ;
                char * pos = path;
                strncpy(path, dirName, sizeof(path));
                char * end = path + strlen(path);
                while(*pos != '\0')
                {
                    if(SDPATH_IS_DELIMITER(*pos))
                    {
                        *pos = '\0';
                    }
                    ++pos;
                }
                pos = path;
                bool bfirst = true;
                while(pos < end)
                {
                    //====检测第一段路径是否为盘符====
                    if(bfirst)
                    {
                        bool isdrive = false;
                        char * p = pos;
                        while(*p != '\0')
                        {
                            if(*p == ':')
                            {
                                isdrive = true;
                            }
                            ++p;
                        }
                        if(isdrive)
                        {
                            pos = path + strlen(path);
                            *pos = SDPATH_DELIMITER;
                            bfirst = false;
                            continue;
                        }
                        bfirst = false;
                    }
                    //====检测第一段路径是否为盘符完毕====

                    //====如果此段路径不存在，创建目录====
                    if(!SDDirectoryExists(path))
                    {
#if (defined(WIN32) || defined(WIN64))
                        if(0 != _mkdir(path))
#else
                        if(0 != mkdir(path, S_IREAD | S_IWRITE))
#endif
                        {
                            return false;
                        }
                    }
                    pos = path + strlen(path);
                    *pos = SDPATH_DELIMITER;
                }
            }
            else//bforce == false
            {
                return false;
            }
        }
        return true;
    }

    bool SDAPI SDDeleteDirectory( const char *dirName, bool bforce/* = false*/ )
    {
        if(nullptr == dirName)
        {
            return false;
        }
        if(!bforce)
        {
#if (defined(WIN32) || defined(WIN64))
            return _rmdir(dirName) == 0;
#else
            return rmdir(dirName) == 0;
#endif
        }
        else
        {
            CSDDirectory dir;
            if(dir.Open(dirName))
            {
                char _filename[SD_MAX_PATH];
                SFileAttr attr = {0};
                while(dir.Read(_filename, SD_MAX_PATH, &attr))
                {
                    char _subfilepath[SD_MAX_PATH];
                    strncpy(_subfilepath, dirName, SD_MAX_PATH);
#if (defined(WIN32) || defined(WIN64))
                    strncat(_subfilepath, "\\", 1);
#else 
                    strncat(_subfilepath, "/", 1);
#endif // 
                    strncat(_subfilepath, _filename, SD_MAX_PATH);
                    if(attr.isDir)
                    {

                        if(!SDDeleteDirectory(_subfilepath, bforce))
                        {
                            return false;
                        }
                    }
                    else
                    {
#if (defined(WIN32) || defined(WIN64))
                        if(_unlink(_subfilepath) != 0)
#else
                        if(unlink(_subfilepath) != 0)
#endif
                        {
                            return false;
                        }
                    }
                }
                dir.Close();
#if (defined(WIN32) || defined(WIN64))
                return _rmdir(dirName) == 0;
#else
                return rmdir(dirName) == 0;
#endif
            }
            return false;
        }
    }

    bool SDAPI SDDirectoryExists(const char *dirName)
    {
#if (defined(WIN32) || defined(WIN64))
        int nCode = GetFileAttributesA(dirName);
        if (nCode == -1)
        {
            return false;
        }
		return (FILE_ATTRIBUTE_DIRECTORY & nCode) ? true : false;
#else
        struct stat fileStat;
        int ret = stat(dirName, &fileStat);
        if ( ret == 0)
        {
            return S_ISDIR(fileStat.st_mode);
        }
        return false;
#endif
    }

    bool SDAPI SDIsDirectory(const char * dirName)
    {
#if (defined(WIN32) || defined(WIN64))
        int nCode = GetFileAttributesA(dirName);
        if (nCode == -1)
        {
            return false;
        }
		return (FILE_ATTRIBUTE_DIRECTORY & nCode) ? true : false;
#else
        struct stat fileStat;
        int ret = stat(dirName, &fileStat);
        if ( ret == 0)
        {
            return S_ISDIR(fileStat.st_mode);
        }
        return false;
#endif
    }

    bool SDAPI SDIsEmptyDirectory(const char* dirName)
    {
        if (SDIsDirectory(dirName))
        {
#if (defined(WIN32) || defined(WIN64))
            HANDLE hFind = INVALID_HANDLE_VALUE;

            std::string dirFile = SDFileIncludeTrailingDelimiter(dirName);

            bool isEmpty = true;
            dirFile+= "*";
            WIN32_FIND_DATAA find_data;
            hFind = FindFirstFileA(dirFile.c_str(), &find_data);
            if (hFind)
            {
                while( FindNextFileA(hFind,&find_data))
                {
                    if (strcmp(find_data.cFileName,".") == 0 || strcmp(find_data.cFileName,"..") == 0)
                    {
                        continue;
                    }
                    isEmpty = false;
                    break;
                }

                FindClose(hFind);

                return isEmpty;
            }
#else
			DIR * pFd = opendir(dirName);
			if (pFd == nullptr)
			{
				return false; 
			}

			struct dirent* pdir = readdir(pFd);
			if (pdir == nullptr)
			{
				closedir(pFd); 
				return false;
			}

			while (pdir != nullptr)
			{
				if (strcmp(pdir->d_name, ".") == 0 ||
					strcmp(pdir->d_name, "..") == 0 )
				{
					pdir = readdir(pFd); 
					continue; 
				}
				else 
				{
					closedir(pFd); 
					return false; 
				}
			}
			
			closedir(pFd); 
#endif // 
        }
        return true;
    }
    
    bool SDAPI SDGetCurrentDirectory(char *pszName, int32 nBufLen)
    {
#ifdef WIN
        return GetCurrentDirectory(nBufLen, pszName);
#else
        return getcwd(pszName, nBufLen);
#endif

    }


} // namespace Crown
