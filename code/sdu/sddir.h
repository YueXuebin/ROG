#pragma once

/******************************************************************************
			Copyright (C) Shanda Corporation. All rights reserved.

sddir.h - 目录操作

******************************************************************************/
#ifndef SDDIR_H
#define SDDIR_H
/**
* @file sddir.h
* @author lw
* @brief dir utility
*
**/
#if (defined(WIN32) || defined(WIN64))
#else
#include <sys/types.h>
#include <dirent.h>
#endif
#include "sdtype.h"
#include "sdfile.h"
namespace Crown
{
    /**
    * @defgroup groupdir 目录操作
    * @ingroup  Crown
    * @{
    */

    struct SFileAttr
    {
        bool isDir; //是否为目录
    };
    /**
    *@brief 目录操作类，可用于获取该目录下所有的文件以文件夹的名称
    */
    class CSDDirectory
    {
    public:
        /**
        * @brief
        * 打开一个目录，参数和fopen一致
        * @param pszPath : 目录的路径
        * @return false 代表打开失败，true代表打开成功
        */
        bool SDAPI Open(const char* pszPath);

        /**
        * @brief
        * 关闭此目录
        * @return void
        */
        void SDAPI Close();

        /**
        * @brief
        * 读取此目录中下一个文件的名称，并可知道该文件是否为目录
        * @param pszName : [输出参数]从该目录中获取的文件名
        * @param nBufLen : [输入参数]能够赋值的最大长度
        * @param pAttr : [输出参数]返回文件属性, isDir如果为1，则为目录；否则为0；若输入是为NULL，则无返回值
        * @return 成功返回true；读取失败，或者到达目录中文件的结尾返回false
        */
        bool SDAPI Read(char *pszName, int32 nBufLen, SFileAttr *pAttr = nullptr);

    private:
#if (defined(WIN32) || defined(WIN64))
        std::string m_szPath;
        HANDLE m_fHandle;
#else
        DIR * m_dir;
		char m_curDir[SD_MAX_PATH]; 
#endif
    };


    /**
    * @brief
    * 创建目录
    * @param pszDirName : 被创建的目录路径及名称
    * @param bforce : 当没有上级目录时，是否强制创建目录
    * @return 成功返回true，失败返回false
    */
    bool SDAPI SDCreateDirectory(const char *pszDirName, bool bForce= false);

    /**
    * @brief
    * 删除一个目录
    * @param pszDirName : 被删除的目录路径及名称
    * @return 成功返回true，失败返回false(文件夹中有内容或者没有删除权限)
    */
    bool SDAPI SDDeleteDirectory(const char *pszDirName, bool bForce = false);

    /**
    * @brief
    * 删除一个目录
    * @param pszDirName : 被删除的目录路径及名称
    * @return 成功返回true，失败返回false
    */
    bool SDAPI SDDirectoryExists(const char *pszDirName);

	/**
	* @brief
	* 判断一个文件是否为文件夹
	* @param pszFileName : 文件名
	* @return 成功返回true，失败返回false
	*/
    bool SDAPI SDIsDirectory(const char * pszFileName);

	/**
	* @brief
	* 判断一个文件夹是否为空
	* @param pszFileName :  文件名
	* @return 成功返回true，失败返回false
	*/
    bool SDAPI SDIsEmptyDirectory(const char* pszFileName);

	/*
	 *	获取当前目录
	 */
    bool SDAPI SDGetCurrentDirectory(char *pszName, int32 nBufLen);



}

#endif

