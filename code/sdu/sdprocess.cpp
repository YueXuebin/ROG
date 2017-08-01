#include "SDuPCH.h"
#include "sdprocess.h"
#include "sdtime.h"
#include "sdthread.h"
#include "sdindexer.h"
#include "sdfile.h"
#if (defined(WIN32) || defined(WIN64))
#include <Windows.h>
#include <Tlhelp32.h>
#else
#include<sys/types.h>
#include<sys/wait.h>
#include<unistd.h>
#endif

#ifdef APPLE
#include <signal.h>
#endif

namespace Crown
{

#if (defined(WIN32) || defined(WIN64))
	template <> 
	struct DefaultValue <PROCESS_INFORMATION>
	{
		void operator() (PROCESS_INFORMATION & data)
		{
			data.hProcess = SDINVALID_HANDLE; 
			data.hThread  = SDINVALID_HANDLE; 
			data.dwProcessId = 0; 
			data.dwThreadId  = 0; 
		}
	};
	struct SProcessFinder
	{
		bool operator ()(const PROCESS_INFORMATION& left, const PROCESS_INFORMATION& right) const 
		{
			if (left.hProcess != SDINVALID_HANDLE && right.hProcess != SDINVALID_HANDLE)
			{
				return left.hProcess == right.hProcess; 
			}
			return (left.dwProcessId == right.dwProcessId); 
		}

	};

	typedef SDIndexer<PROCESS_INFORMATION, 1024*10, SProcessFinder> CSDProcessIndexer; 
#else 
	typedef SDIndexer<pid_t, 1024*10 > CSDProcessIndexer; 
#endif
	static CSDProcessIndexer  s_processIndexer; 


    SDHANDLE SDAPI SDCreateProcess(const char * progName, const char * pCmdLine ,const char * pWorkDir, SProcessAttr * pAttr )
    {
#if (defined(WIN32) || defined(WIN64))
        STARTUPINFOA m_sinfo;
        PROCESS_INFORMATION m_pinfo;
        void * pEnv = NULL;
        bool inherithandle = false;
        uint32 createFlags =0;
        if (pAttr != NULL)
        {
            pEnv = (LPVOID)pAttr->environment.c_str();
            inherithandle = pAttr->inherithandle;
            createFlags = pAttr->createFlags;
        }

        memset((void*)&m_pinfo, 0, sizeof(PROCESS_INFORMATION));
        memset((void*)&m_sinfo, 0, sizeof(STARTUPINFO));
        m_sinfo.cb = sizeof(STARTUPINFO);
        if( !CreateProcessA( progName,
                             (LPSTR)pCmdLine,
                             NULL,
                             NULL,
                             inherithandle,
                             createFlags,
                             (LPVOID)pEnv,
                             pWorkDir,
                             &m_sinfo,
                             &m_pinfo)
          )
        {
            return SDINVALID_HANDLE;
        }
		return (SDHANDLE)s_processIndexer.Alloc(m_pinfo); 
#else
        pid_t m_pid;
        if ( (m_pid = fork()) < 0 )
        {
			return s_processIndexer.Alloc(m_pid); 
        }
        else if(m_pid == 0)
        {
            execv(pWorkDir, NULL);
            _exit(-1);
        }
		return s_processIndexer.Alloc(m_pid); 
#endif

    }

	SDPROCESSID SDAPI SDGetProcessId(SDHANDLE handle)
	{
#if (defined(WIN32) || defined(WIN64))
		PROCESS_INFORMATION & pInfo = s_processIndexer.Get((uint32)handle); 
		return GetProcessId(pInfo.hProcess); 
#else 
		return  s_processIndexer.Get(handle); 
#endif //
	}

#if (defined(WIN32) || defined(WIN64))
	static SDHANDLE FindProcessHandle(SDPROCESSID pid )
	{
		PROCESS_INFORMATION toFind; 
		toFind.dwProcessId = pid; 
		toFind.hProcess = SDINVALID_HANDLE; 
		return (SDHANDLE)s_processIndexer.Find(toFind); 
	}
	
#endif 
 
	SDHANDLE SDAPI SDOpenProcess(SDPROCESSID pid)
	{
		SDHANDLE handle; 
#if (defined(WIN32) || defined(WIN64))
		PROCESS_INFORMATION procInfo = {0}; 
		procInfo.hProcess  = SDINVALID_HANDLE; 
		procInfo.dwProcessId = pid; //根据进程ID查找对应的Handle 及其他信息
		handle = FindProcessHandle(pid); 
		if (handle == SDINVALID_HANDLE)
		{
			procInfo.hProcess =  OpenProcess(PROCESS_ALL_ACCESS , false, pid); 
			//procInfo.dwProcessId = GetProcessId(procInfo.hProcess); 
			HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD,pid); 
			if (hSnap != SDINVALID_HANDLE)
			{
				THREADENTRY32 thrdEnt   = {0}; 
				thrdEnt.dwSize = sizeof(thrdEnt); 
				bool ret = Thread32First(hSnap,&thrdEnt);
				if (ret )
				{
					if (thrdEnt.th32OwnerProcessID == pid)
					{
						procInfo.dwThreadId = thrdEnt.th32ThreadID; 
						procInfo.hThread = OpenThread(THREAD_ALL_ACCESS,false,procInfo.dwThreadId); 
					}
					else 
					{
						while(Thread32Next(hSnap,&thrdEnt))
						{
							if (thrdEnt.th32OwnerProcessID == pid )
							{
								procInfo.dwThreadId = thrdEnt.th32ThreadID; 
								procInfo.hThread = OpenThread(THREAD_ALL_ACCESS,false,procInfo.dwThreadId); 
								break; 
							}
						}
					}
					
				}
				CloseHandle(hSnap); 
			}
			return (SDHANDLE)s_processIndexer.Alloc(procInfo); 
		}
#else 
		handle = s_processIndexer.Find(getpid()); 
		if (handle == SDINVALID_HANDLE)
		{
			pid_t curPid = getpid(); 
			return (SDHANDLE)s_processIndexer.Alloc(curPid); 
		}
#endif //
		return handle; 
	}

	SDHANDLE SDAPI  SDGetCurrentProcess()
	{
		SDHANDLE handle ; 
#if (defined(WIN32) || defined(WIN64))
		PROCESS_INFORMATION procInfo; 
		procInfo.hProcess = GetCurrentProcess(); 
		procInfo.dwProcessId = GetProcessId(procInfo.hProcess); 
		procInfo.hThread  = GetCurrentThread(); 
		procInfo.dwThreadId = SDGetThreadId(); 
		handle = FindProcessHandle(procInfo.dwProcessId);
		if (handle == SDINVALID_HANDLE)
		{
			handle = (SDHANDLE)s_processIndexer.Alloc(procInfo); 
		}
#else 
		pid_t curPid = getpid(); 
		handle = s_processIndexer.Find(curPid); 
		if (handle == SDINVALID_HANDLE)
		{
			handle = s_processIndexer.Alloc(curPid); 
		}
#endif // 
		return handle; 
	}

	SDPROCESSID SDAPI SDGetCurrentProcessId()
	{
#if (defined(WIN32) || defined(WIN64))
		return GetCurrentProcessId(); 
#else 
		return getpid(); 
#endif // 
	}

    std::string SDAPI SDGetCurrentProcessFullName()
    {
#ifdef WIN
        char path[SD_MAX_PATH];
        GetModuleFileName(NULL, path, SD_MAX_PATH);
        return path;
#else
        char link[SD_MAX_PATH], path[SD_MAX_PATH];
        sprintf(link, "/proc/%d/exe", SDGetCurrentProcessId());
        readlink(link, path, sizeof(path));
        return path;
#endif
    }

    void SDAPI SDCloseProcess(SDHANDLE handle)
    {
#if (defined(WIN32) || defined(WIN64))
		PROCESS_INFORMATION & pInfo = s_processIndexer.Free((uint32)handle); 
        CloseHandle( pInfo.hThread );
        CloseHandle( pInfo.hProcess );
#endif
    }

    void SDAPI SDTerminateProcess(SDHANDLE handle,int32 err )
    {
#if (defined(WIN32) || defined(WIN64))
		PROCESS_INFORMATION & pInfo = s_processIndexer.Get((uint32)handle); 
        TerminateProcess(pInfo.hProcess,err);
#else
		pid_t pid = s_processIndexer.Get(handle); 
        kill(pid,SIGKILL);
#endif
    }

    void SDAPI SDWaitProcess(SDHANDLE handle)
    {
#if (defined(WIN32) || defined(WIN64))
		PROCESS_INFORMATION & pInfo = s_processIndexer.Get((uint32)handle); 
        WaitForSingleObject(pInfo.hProcess, INFINITE);
#else
        int status;
		pid_t pid = s_processIndexer.Get(handle); 
        waitpid(pid, &status, WUNTRACED);
#endif
    }


    bool SDAPI SDSetProcessAttr(SDHANDLE handle, const SProcessAttr &pAttr)
    {
#if (defined(WIN32) || defined(WIN64))
		if (pAttr.secInfo.bSet)
		{
			PROCESS_INFORMATION & pInfo = s_processIndexer.Get((uint32)handle); 
			uint32 ret = SetSecurityInfo(pInfo.hProcess,
				pAttr.secInfo.objectType,
				pAttr.secInfo.securityInfo,
				pAttr.secInfo.psidOwner,
				pAttr.secInfo.psidGroup,
				pAttr.secInfo.pDacl,
				pAttr.secInfo.pSacl
				);
			if (ret != ERROR_SUCCESS)
			{
				return false; 
			}
		}
#else 
#endif // 
        return false;
    }

	bool SDAPI SDGetProcessAttr(SDHANDLE handle, SProcessAttr & procAttr)
	{
#if (defined(WIN32) || defined(WIN64))
		if (procAttr.secInfo.bSet)
		{
			PROCESS_INFORMATION & pInfo = s_processIndexer.Get((uint32)handle); 
			uint32 ret = GetSecurityInfo(pInfo.hProcess,
				procAttr.secInfo.objectType, 
				procAttr.secInfo.securityInfo,
				&procAttr.secInfo.psidOwner,
				&procAttr.secInfo.psidGroup,
				&procAttr.secInfo.pDacl,
				&procAttr.secInfo.pSacl,
				&procAttr.secInfo.pSecurityDescriptor
				);
			if (ret != ERROR_SUCCESS)
			{
				return false; 
			}
		}
#endif 

		return true;
	}

    CSDProcess::CSDProcess()
    {
        m_handle = SDINVALID_HANDLE;
    }

    CSDProcess::~CSDProcess()
    {
        if (m_handle != SDINVALID_HANDLE)
        {
            this->Terminate();
        }
    }

    bool SDAPI CSDProcess::Create(const char * progName, const char * pArgs, const char * pEnvs, SProcessAttr * pAttr)
    {
        m_handle = SDCreateProcess(progName, pArgs, pEnvs, pAttr);
        return SDINVALID_HANDLE != m_handle;
    }

    void SDAPI CSDProcess::Terminate()
    {
        SDTerminateProcess(m_handle);
        m_handle = SDINVALID_HANDLE;
    }

    void SDAPI CSDProcess::Wait()
    {
        SDWaitProcess(m_handle);
    }

	uint32 SDAPI CSDProcess::GetProcessID()
	{
		return SDGetProcessId(m_handle); 
	}

    SDHANDLE SDAPI CSDProcess::GetHandle()
    {
        return m_handle;
    }

    bool SDAPI CSDProcess::SetAttribute(const SProcessAttr &pAttr)
    {
        return SDSetProcessAttr(m_handle,pAttr);
    }

    bool SDAPI CSDProcess::GetAttribute(SDHANDLE handle, SProcessAttr & procAttr)
    {
        return SDGetProcessAttr(m_handle, procAttr);
    }

}
