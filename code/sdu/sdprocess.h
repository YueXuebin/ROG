#pragma once

#ifndef SDPROCESS_H
#define SDPROCESS_H
/**
* @file sdprocess.h
* @author lw
* @brief 进程处理
*
**/
#include "sdtype.h"
#include <string>
#if (defined(WIN32) || defined(WIN64))
#include "Aclapi.h"
#endif // 


namespace Crown
{
    /**
    * @defgroup groupthread 多进程多线程(包括线程池)
    * @ingroup  Crown
    * @{
    */
#if (defined(WIN32) || defined(WIN64))
	typedef struct tagProcessSecInfo 
	{
		bool bSet; // 是否设置此安全信息
		SE_OBJECT_TYPE objectType;
		SECURITY_INFORMATION securityInfo;
		PSID psidOwner;
		PSID psidGroup;
		PACL pDacl;
		PACL pSacl;
		PSECURITY_DESCRIPTOR pSecurityDescriptor; // 获取时使用
	} SProcessSecInfo; 
#endif 

    struct SProcessAttr
    {
        std::string  workDir;  //进程执行的工作目录
        std::string  environment;
        uint32       createFlags;  // 进程创建标识 fow windows.
        bool         inherithandle;// for Windows
#if (defined(WIN32) || defined(WIN64))
		SProcessAttr()
		{
			secInfo.bSet = false; 
		}
		SProcessSecInfo secInfo; 
#endif // 
    };


#if (defined(WIN32) || defined(WIN64))
typedef uint32 SDPROCESSID; 
#else 
typedef pid_t  SDPROCESSID; 
#endif // 

    /**
    * @brief
    * 创建一个进程
    * @param progName : 该进程的执行文件
    * @return 成功返回true，失败返回false
    */
    SDHANDLE SDAPI SDCreateProcess(
        const char *progName,
        const char *pCmdLine = NULL,
        const char *pWorkDir = NULL,
        SProcessAttr *pAttr = NULL);

    /**
    * @brief
    * 终止一个进程
    * @param handle : 进程句柄
    * @param err  :指定进程退出时的错误码信息
    * @return void
    */
    void SDAPI SDTerminateProcess(SDHANDLE handle, int32 err = 0);

    /**
    * @brief
    * 关闭进程的句柄,对于进程不要使用通用的SDCloseHandle来关闭句柄,否则会导致资源未释放
    * @param handle : 进程句柄
    * @return void
    */
    void SDAPI SDCloseProcess(SDHANDLE handle);

	/**
	* @brief
	* 获取进程ID，进程必须设置属性
	* @param handle : 进程句柄
	* @return void
	*/
	SDPROCESSID SDAPI SDGetProcessId(SDHANDLE handle);

	/**
	* @brief
	* 通过进程ID获取进程handle，
	* @param pid: 进程实际ID
	* @return void
	*/
	SDHANDLE SDAPI SDOpenProcess(SDPROCESSID pid); 

	/**
	* @brief
	* 获取当前的进程
	* @param handle : 进程,线程句柄
	* @return void
	*/
	SDHANDLE SDAPI SDGetCurrentProcess();

    /*
    *   获取当前进程对应的可执行文件的绝对路径
    */
    std::string SDAPI SDGetCurrentProcessFullName();

	/**
	* @brief
	* 获取当前的进程id
	* @param handle : 进程,线程句柄
	* @return void
	*/
	SDPROCESSID SDAPI SDGetCurrentProcessId(); 

    /**
    * @brief
    * 等待进程终止
    * @param handle : 进程句柄
    * @return void
    */
    void SDAPI SDWaitProcess(SDHANDLE handle);

	/**
	* @brief
	* 设置进程属性
	* @param handle : 进程句柄
	* @return 设置结果
	*/
    bool SDAPI SDSetProcessAttr(SDHANDLE handle, const SProcessAttr &pAttr);

	/**
	* @brief
	* 获取进程属性
	* @param handle : 进程句柄
	* @param procAttr : 进程属性
	* @return 获取是否成功
	*/
	bool SDAPI SDGetProcessAttr(SDHANDLE handle, SProcessAttr & procAttr); 

    /**
    * @brief 进程操作类
    */
    class CSDProcess
    {
    public:

        CSDProcess();
        ~CSDProcess();

        /**
        * @brief
        * 创建进程
        * @param path : 该进程的执行文件
        * @return 如果成功返回true, 失败返回false
        */
        bool SDAPI Create(
            const char *pProgName,
            const char *pArgs = NULL,
            const char *pEnvs = NULL,
            SProcessAttr *pAttr = NULL);

        /**
        * @brief
        * 终止进程
        * @return void
        */
        void SDAPI Terminate();

        /**
        * @brief
        * 等待进程终止或者执行结束
        * @return void
        */
        void SDAPI Wait();

		/**
		* @brief
		* 获取进程ID
		* @return SDHANDLE
		*/
		uint32 SDAPI GetProcessID(); 

        /**
        * @brief
        * 获取进程句柄
        * @return SDHANDLE
        */
        SDHANDLE SDAPI GetHandle();

        /**
        * @brief
        * 设置进程属性
        * @return 成功返回true，失败返回false
        */
        bool SDAPI SetAttribute(const SProcessAttr &pAttr);

        /**
        * @brief
        * 获取进程属性
        * @return 返回的进程属性结构体
        */
        bool SDAPI GetAttribute(SDHANDLE handle, SProcessAttr & procAttr);

    private:
        CSDProcess (const CSDProcess&);
        CSDProcess& operator = (const CSDProcess&);
        SDHANDLE m_handle;
    };

}

#endif

