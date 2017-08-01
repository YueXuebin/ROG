#pragma once

/******************************************************************************
		Copyright (C) Shanda Corporation. All rights reserved.

sdfile.h - 文件操作API

******************************************************************************/


#ifndef SDFILE_H
#define SDFILE_H

#include "sdtype.h"
#include <stdio.h>
//#include <string>
#include <time.h>
#include <vector>

/**
* @file sdfile.h
* @author lw
* @brief 文件工具类
*
**/

namespace Crown
{
    /**
    * @defgroup groupfile 文件操作
    * @ingroup  Crown
    * @{
    */

    /* An arbitrary size that is digestable. True max is a bit less than 32000 */
#ifdef WIN32
#define SD_MAX_PATH  MAX_PATH
#define SDPATH_DELIMITER '\\'
#else /* !SD_HAS_UNICODE_FS */
#define SD_MAX_PATH 260
#define SDPATH_DELIMITER '/'
#endif

    /* Win32 .h ommissions we really need */
#define SDFILENO_STDIN   0
#define SDFILENO_STDOUT  1
#define SDFILENO_STDERR  2
#define SDPATH_IS_DELIMITER(x)  (x == '\\' || x == '/')
    /**
    *@brief 文件状态结构体
    */
    struct SFileStatus
    {
        uint32 mode;
        uint32 inode;
        uint32 dev;
        uint32 rdev;
        uint32 nlink;
        uint32 uid;
        uint32 gid;
        uint32 size;
        time_t atime;
        time_t mtime;
        time_t ctime;
    };


    /**
    *@brief 文件权限枚举类
    */
    enum AccessMode
    {
#if (defined(WIN32) || defined(WIN64))
        EXIST_OK = 0,
        RD_OK = 2,
        WD_OK = 4,
#else
        EXIST_OK = 0,
        RD_OK = 2,
        WD_OK = 4,
        EXECUTE_OK = 8,
#endif
    };



    /**
    *@brief 文件操作类，完成最基本的文件操作,或使用FileMapping方式读取文件(具体功能参照接口)
    */
    class CSDFile
    {

    public:
        CSDFile();
        ~CSDFile();

        /**
        *@brief seek位置枚举类
        */
        enum SeekOffset
        {
            SK_SET = SEEK_SET,
            SK_END = SEEK_END,
            SK_CUR = SEEK_CUR,
        };

        /**
        * @brief
        * Open a file, type参数同fopen接口
        * @param pszFileName : 文件名称
        * @param type : 文件访问权限
        * @return 成功返回true，失败返回false
        */
        bool SDAPI Open(const char* pszFileName, const char* type, bool bFileMapping = false);

        /**
        * @brief
        * 关闭一个文件
        * @return void
        */
        void SDAPI Close();

        /**
        * @brief
        * Flush刷新文件
        * @return void
        */
        void SDAPI Flush();

        /**
        * @brief
        * 从文件中读取数据
        * @param pBuf : 读取数据的存放位置
        * @param nLen : 能够存发数据的最大长度
        * @return 实际读取的数据长度
        */
        uint32 SDAPI Read(void* pBuf, uint32 nLen);

        /**
        * @brief
        * 向文件中写入数据
        * @param pBuf : 指向写入数据的指针
        * @param nLen : 最大可写的数据长度
        * @return 实际写入的数据长度
        */
        uint32 SDAPI Write(const void* pBuf, uint32 nLen);

        /**
        * @brief
        * 移动文件游标到特定的位置
        * @param offset : 相对初始位置移动的距离
        * @param whence : 初始位置，请输入enum SeekOffset中的值
        * @return 如果成功，返回0
        */
        int32 SDAPI Seek(int32 offset, CSDFile::SeekOffset whence);

        /**
        * @brief
        * 移动文件游标到当前文件的文件头
        * @return 返回－1代表失败
        */
        int32 SDAPI SeekToBegin();

        /**
        * @brief
        * 移动文件游标到当前文件的文件尾
        * @return 返回－1代表失败
        */
        int32 SDAPI SeekToEnd();

        /**
        * @brief
        * 获取当前文件游标距离文件头的距离，单位byte
        * @return 当前文件游标距离文件头的距离
        */
        int32 SDAPI GetPosition();

        /**
        * @brief
        * 更新文件的长度，单位为byte
        * @param newLen : 更新的文件长度，单位为byte
        * @return void
        */
        void SDAPI SetLength(int32 newLen);

        /**
        * @brief
        * 测试是否为文件尾部
        * @return 如果是文件尾部，返回true，否则返回false
        */
        bool SDAPI Eof();

        /**
        * @brief
        * 获取当前文件的属性
        * @param pStatus : 获取的文件属性结构体
        * @return 如果成功，返回true，否则返回false
        */
        bool SDAPI GetFileStatus(SFileStatus &pStatus);

    private:
        CSDFile(const CSDFile& origFile);              // no implementation
        void operator=(const CSDFile& origFile);       // no implementation

        void * m_pFileHandle;
        std::string m_filepath;
        bool m_bFileMapping; // 是否使用FileMapping方式读取文件
    };


    /**
    * @brief
    * 获取文件的属性
    * @param pszFileName : 需要获取文件属性的文件路径
    * @param pStatus : 获取的文件属性结构体
    * @return 如果成功，返回true，否则返回false
    */
    bool SDAPI SDGetFileStatus(const char* pszFileName, SFileStatus& stStatus);

    /**
    * @brief
    * 删除一个文件
    * @param pszFileName : 需要删除的文件的文件路径
    * @return 是否成功，返回true，否则返回false
    */
    bool SDAPI SDFileRemove(const char* pszFileName);

    /**
    * @brief
    * 更改一个文件的文件名（或者是文件路径）
    * @param pszOldName : 原先的文件路径
    * @param pszNewName : 新的文件路径
    * @return 成功返回0
    */
    int32 SDAPI SDFileRename(const char *pszOldName, const char *pszNewName, bool bForce = false);

    /**
    * @brief
    * 更改一个文件的文件路径（或者是文件名）
    * @param pszOldName : 原先的文件路径
    * @param pszNewName : 新的文件路径
    * @return 成功返回0
    */
    int32 SDAPI SDFileMove(const char *pszOldName, const char *pszNewName, bool bForce = false);

    /**
    * @brief
    * 确定文件的读写权限
    * @param pszPath : 需要确定读写权限的文件路径
    * @param mode : 需要确认的可访问模式
    * 有如下几种模式：EXIST_OK(是否存在), RD_OK(是否可读), WD_OK(是否可写)
    * @return 如果拥有该模式，返回0
    */
    int32 SDAPI SDFileAccess(const char *pszPath, AccessMode mode);

    /**
    * @brief
    * 获取当前应用的完整路径,包括执行文件的文件名
    * 需要注意的是，此函数即便在外面加所也无法确保线程安全
    * @return 获取的当前应用的路径
    */
    const char* SDAPI SDGetModuleName(void);

    /**
    * @brief
    * 获取当前应用的路径
    * 需要注意的是，此函数即便在外面加所也无法确保线程安全
    * @return 当前应用的路径
    */
    const char* SDAPI SDGetModulePath();

	/**
	* @brief
	* 获取当前应用的工作路径(current work directory) 
	* 需要注意的是，此函数即便在外面加所也无法确保线程安全
	* @return 当前应用的工作路径
	*/
    const char* SDAPI SDGetWorkPath();

    /**
    * @brief
    * 获取文件的文件路径
    * @param pszFileName : 文件路径＋文件名称
    * @return 文件路径
    */
    std::string SDAPI SDFileExtractPath(const char *pszFileName);

    /**
    * @brief
    * 获取文件的文件名
    * @param pszFileName : 文件路径＋文件名称
    * @return 文件名
    */
    std::string SDAPI SDFileExtractName(const char *pszFileName);

    /**
    * @brief
    * 获取文件的文件扩展名
    * @param pszFileName : 文件路径＋文件名称
    * @return 文件扩展名
    */
    std::string SDAPI SDFileExtractExt(const char *pszFileName);

    /**
    * @brief
    * 去除文件路径最后的"\"
    * @param pszPath : 文件路径
    * @return 去除最后的"\"后的文件路径
    */
    std::string SDAPI SDFileExcludeTrailingDelimiter(const char *pszPath);

    /**
    * @brief
    * 添加"\"到文件路径最后，若文件路径后本有"\"，则不添加
    * @param pszPath : 文件路径
    * @return 添加"\"后的文件路径
    */
    std::string SDAPI SDFileIncludeTrailingDelimiter(const char *pszPath);

    /**
    * @brief
    * 添加"\"到文件路径前，若文件路径前本有"\"，则不添加
    * @param pszPath : 文件路径
    * @return 添加"\"后的文件路径
    */
    std::string SDAPI SDFileIncludeLeadingDelimiter(const char *pszPath);

    /**
    * @brief
    * 去除文件路径前的"\"
    * @param pszPath : 文件路径
    * @return 去除"\"后的文件路径
    */
    std::string SDAPI SDFileExcludeLeadingDelimiter(const char *pszPath);

    /**
    * @brief
    * 将文件路径中的"\"和"//"替换为特定的字符delimiter
    * @param pszPath : 文件路径
    * @param delimiter : 替换字符
    * @return 替换后的文件路径
    */
    std::string SDAPI SDFileReplaceDelimiter(const char *pszPath, char delimiter);

    /**
    * @brief
    * 检测文件是否存在
    * @param pszFileName : 文件路径
    * @return 存在则返回true，不存在则返回false
    */
    bool SDAPI SDFileExists(const char *pszFileName);

    /**
    * @brief
    * 获取文件大小
    * @param pszFileName : 文件路径
    * @return 文件大小
    */
    int32 SDAPI SDFileGetSize(const char *pszFileName);

    /**
    * @brief
    * 创建文件
    * @param pszFileName : 文件路径
    * @return 创建成功返回true，否则返回false
    */
    bool SDAPI SDFileCreate(const char *pszFileName);


	
	/**
	* @brief
	* 拷贝文件
	* @param pszExistFile :源文件路径 
	* @param pNewFile :   目标文件路径
	* @param bFailIfExists : 如果目标存在是否返回失败
	* @return 拷贝成功返回true，否则返回false
	*/
	bool SDAPI SDFileCopy(const char *pszExistFile, const char *pNewFile, bool bFailIfExists); 


}

#endif




