#pragma once

/******************************************************************************
Copyright (C) Shanda Corporation. All rights reserved.


******************************************************************************/


#ifndef SDSTRING_H
#define SDSTRING_H
/**
* @file sdstring.h
* @author lw
* @brief string处理系列
*
**/
#include "sdtype.h"
#include <string>
#include <string.h>
#include <stdio.h>
#include <cstdlib>
#include <vector>
#include <algorithm>
#include <vector>


/**
* @file sdstring.h
* @author lw
* @brief some function for string process
*
**/

#if (defined(WIN32) || defined(WIN64))
#define SDSYMBOL_RT  "\r\n"
#else
#include <wchar.h>
#include <wctype.h>
#define SDSYMBOL_RT  "\n"
#endif

namespace Crown
{

    /**
    * @defgroup groupstring 字符串处理
    * @brief 字符串处理不做边界检测
    * @ingroup  Crown
    * @{
    */

    
#define SDIsAlNum(c) (isalnum(((byte)(c))))
    
#define SDIsAlpha(c) (isalpha(((byte)(c))))
    
#define SDIsCntrl(c) (iscntrl(((byte)(c))))

#define SDISGraph (c) (isgraph(((byte)(c))))
    
#define SDIsLower(c) (islower(((byte)(c))))
    
#ifdef isascii
#define SDIsAscii(c) (isascii(((byte)(c))))
#else
#define SDIsAscii(c) (((c) & ~0x7f)==0)
#endif
    
#define SDIsPrint(c) (isprint(((byte)(c))))
    
#define SDIsPunct(c) (ispunct(((byte)(c))))
    
#define SDIsSpace(c) (isspace(((byte)(c))))
    
#define SDIsUpper(c) (isupper(((byte)(c))))
    
#define SDIsXdigit(c) (isxdigit(((byte)(c))))
    
#define SDToLower(c) (tolower(((byte)(c))))
    
#define SDToUpper(c) (toupper(((byte)(c))))


    /**
    * @brief
    * 将字符串开始和结束处设置为0
    * @param string : 以0为结束符的字符串
    * @return:void
    */
    template <class T>
    inline void ZeroString(T& Destination) throw()
    {
#if ( defined(WIN32) || defined(WIN64))
        (static_cast<char[sizeof(Destination)]>(Destination));
#endif
        Destination[0] = '\0';
        Destination[sizeof(Destination)-1] = '\0';
    }

    inline bool SDIsDigit(const char* string)
    {
        for(int32 i=0; string[i]!=0; i++)
        {
            if(!isdigit(string[i]))
                return false;
        }
        return true;
    }

    /**
    * @brief
    * 获取字符串的长度
    * @param string : 以0为结束符的字符串
    * @return : 字符串的长度
    */
    inline uint32 SDAPI SDStrlen( const char *string )
    {
        return (uint32)strlen(string);
    }

    /**
    * @brief
    * 获取Unicode字符串的长度
    * @param string : 以0为结束符的字符串
    * @return : Unicode字符串的长度
    */
    inline uint32 SDAPI SDWcslen( const wchar_t *string )
    {
        return (uint32)wcslen(string);
    }

    /**
    * @brief
    * 获取字符串的长度
    * @param pszStr : 字符串
    * @param sizeInBytes :最大检测长度
    * @return 获取字符串的长度，如果字符串的长度超过sizeInBytes，返回sizeInBytes
    */
    inline uint32 SDAPI SDStrnlen(const char *pszStr, uint32 sizeInBytes )
    {
#ifdef APPLE
	for(uint32 i = 0; i < sizeInBytes; ++i)
	{
		if(pszStr[i] == '\0')
			return i;
	}
	return sizeInBytes;
#else
        return strnlen(pszStr, sizeInBytes);
#endif
    }

#ifndef APPLE
	/**
    * @brief
    * 获取Unicode字符串的长度
    * @param pszStr : Unicode字符串
    * @param sizeInBytes :最大检测长度
    * @return 获取Unicode字符串的长度，如果Unicode字符串的长度超过sizeInBytes，返回sizeInBytes
    */
    inline uint32 SDAPI SDWcsnlen(const wchar_t *pszStr, size_t sizeInBytes)
    {
        return wcsnlen(pszStr, sizeInBytes);
    }

    /**
    * @brief
    * 在目标字符串后添加一个字符串
    * @param strDestination : 以0为结束符的目标字符串
    * @param strSource : 以0为结束符的源字符串
    * @return 结果字符串，使用的是目标字符串空间，请预留足够的空间
    */
    inline char* SDAPI SDStrcat(char *strDestination, const char *strSource )
    {
        return strcat(strDestination, strSource);
    }

    /**
    * @brief
    * 在目标字符串后添加一个字符串
    * @param strDestination : 以0为结束符的目标字符串
    * @param dstLen : 目标串内存的总长度,如果合并后的长度大于此长度,不会进行合并,返回空串,不修改目标串
    * @param strSource : 以0为结束符的源字符串
    * @return 结果字符串，使用的是目标字符串空间,如果合并后的长度大于此长度,不会进行合并,返回NULL,不修改目标串
    */
    inline char * SDAPI SDStrSafeCat(char * strDestination, uint32 dstLen, const char * strSource)
    {
        if (dstLen < strlen(strDestination) + strlen(strSource) + 1)
        {
            return nullptr;
        }
        return strcat(strDestination,strSource);
    }

    /**
    * @brief
    * 在目标字符串后添加一个字符串
    * @param strDestination : 以0为结束符的目标字符串数组
    * @param strSource : 以0为结束符的源字符串
    * @return 结果字符串，使用的是目标字符串空间
    */
    template <size_t len>
    char* SDAPI SDStrSafeCat( char (&strDestination)[len], const char *strSource )
    {
        return SDStrSafeCat(strDestination, len, strSource);
    }

    /**
    * @brief
    * 在目标Unicode字符串后添加一个Unicode字符串
    * @param strDestination : 以0为结束符的目标Unicode字符串
    * @param strSource : 以0为结束符的源Unicode字符串
    * @return 结果Unicode字符串，使用的是目标Unicode字符串空间，请预留足够的空间
    */
    inline wchar_t*  SDAPI SDWcscat(wchar_t *strDestination, const wchar_t *strSource )
    {
        return wcscat(strDestination, strSource);
    }

    /**
    * @brief
    * 在目标字符串后添加一个字符串
    * @param strDestination : 以0为结束符的目标字符串
    * @param strSource : 源字符串
    * @param len : 源字符串的最大添加长度
    * @return 结果字符串，使用的是目标字符串空间，请预留足够的空间
    */
    inline char* SDAPI SDStrncat(char *strDestination, const char *strSource, uint32 len)
    {
        return strncat(strDestination, strSource, len);
    }

    /**
    * @brief
    * 在目标Unicode字符串后添加一个Unicode字符串
    * @param strDestination : 以0为结束符的目标Unicode字符串
    * @param strSource : 源Unicode字符串
    * @param len : 源Unicode字符串的最大添加长度
    * @return 结果Unicode字符串，使用的是目标Unicode字符串空间，请预留足够的空间
    */
    inline wchar_t* SDAPI SDWcsncat(wchar_t *strDestination, const wchar_t *strSource, uint32 len)
    {
        return wcsncat(strDestination, strSource, len);
    }
#endif //ifndef APPLE

    /**
    * @brief
    * 拷贝字符串
    * @param strDestination : 以0为结束符的目标缓冲区，请保证有足够的空间
    * @param strSource : 以0为结束符的源字符串
    * @return 目标字符串
    */
    inline char* SDAPI SDStrcpy( char *strDestination, const char *strSource )
    {
        return strcpy(strDestination, strSource);
    }

    /**
    * @brief
    * 安全拷贝字符串
    * @param strDestination : 需要拷贝字符串的目标缓冲区
    * @param numberOfElements : 目标字符串最大长度
    * @param strSource : 以0结尾的源字符串
    * @return 返回目标串,如果目标缓冲区的长度小于源串,返回NULL
    */
    inline char* SDStrSafeCpy( char *strDestination, size_t numberOfElements, const char *strSource )
    {
        if (numberOfElements  < strlen(strSource) +1)
        {
            return nullptr;
        }

        return strncpy(strDestination,strSource,numberOfElements);
    }

    /**
    * @brief
    * 安全拷贝字符串
    * @param strDestination : 需要拷贝字符串的目标字符串数组
    * @param strSource : 以0结尾的源字符串
    * @return 返回目标串,如果目标缓冲区的长度小于源串,返回空串
    */
    template <size_t len>
    char* SDAPI SDStrSafeCpy( char (&strDestination)[len], const char *strSource )
    {
        return SDStrSafeCpy(strDestination,len,strSource);
    }

    /**
    * @brief
    * 拷贝Unicode字符串
    * @param strDestination : 以0结尾的目标缓冲区，请保证有足够的空间
    * @param strSource : 以0结尾的源字符串
    * @return 目标字符串
    */
    inline wchar_t* SDAPI SDWcscpy( wchar_t *strDestination, const wchar_t *strSource )
    {
        return wcscpy(strDestination, strSource);
    }

#ifndef APPLE
    /**
    * @brief
    * 安全拷贝Unicode字符串
    * @param strDestination : 需要拷贝Unicode字符串的目标缓冲区
    * @param numberOfElements : 目标字Unicode符串最大长度
    * @param strSource : 以0结尾的源Unicode字符串
    * @return 返回目标串,如果目标缓冲区的长度小于源串,返回NULL
    */
    inline wchar_t * SDWcsSafeCpy( wchar_t *strDestination, size_t numberOfElements, const wchar_t *strSource )
    {
        if (numberOfElements  < wcslen(strSource) +1)
        {
            return nullptr;
        }

        return wcsncpy(strDestination,strSource,numberOfElements);
    }

    /**
    * @brief
    * 安全拷贝Unicode字符串
    * @param strDestination : 需要拷贝Unicode字符串的目标字符串数组
    * @param strSource : 以0结尾的源Unicode字符串
    * @return 返回目标串,如果目标缓冲区的长度小于源串,返回空串
    */
    template <size_t len>
    inline wchar_t* SDAPI SDWcsSafecpy( wchar_t (&strDestination)[len], const wchar_t *strSource )
    {
        return SDWcsSafecpy(strDestination, len, strSource);
    }
#endif //ifndef APPLE

    /**
    * @brief
    * 拷贝字符串
    * @param strDestination : 以0结尾的目标字符串
    * @param strSource : 以0结尾的源字符串
    * @param len : 最大拷贝长度
    * @return 返回目标字符串
    */
    inline char* SDAPI SDStrncpy( char *strDestination, const char *strSource, uint32 len )
    {
        return strncpy(strDestination, strSource, len);
    }

#ifndef APPLE
    /**
    * @brief
    * 拷贝Unicode字符串
    * @param strDestination : 以0结尾的目标Unicode字符串
    * @param strSource : 以0结尾的源Unicode字符串
    * @param len : 最大拷贝长度
    * @return 返回目标Unicode字符串
    */
    inline wchar_t* SDAPI SDWcsncpy( wchar_t *strDestination, const wchar_t *strSource, uint32 len )
    {
        return wcsncpy(strDestination, strSource, len);
    }

    /**
    * @brief
    * 比较字符串
    * @param string1 : 以0结束的字符串1
    * @param string2 : 以0结束的字符串2
    * @return 比较结果
    * < 0 字符串1小于字符串2
    * 0 字符串1等于字符串2
    * > 0 字符串1大于字符串2
    */
    inline int32 SDAPI SDStrcmp( const char *string1, const char *string2 )
    {
        return strcmp(string1, string2);
    }

    /**
    * @brief
    * 比较Unicode字符串
    * @param string1 : 以0结束的Unicode字符串1
    * @param string2 : 以0结束的Unicode字符串2
    * @return 比较结果
    * < 0 字符串1小于字符串2
    * 0 字符串1等于字符串2
    * > 0 字符串1大于字符串2
    */
    inline int32 SDAPI SDWcscmp( const wchar_t *string1, const wchar_t *string2 )
    {
        return wcscmp(string1, string2);
    }
#endif  // ifndef APPLE

    /**
    * @brief
    * 比较字符串
    * @param string1 : 以0结束的字符串1
    * @param string2 : 以0结束的字符串2
    * @param count : 最大比较长度
    * @return 比较结果
    * < 0 字符串1小于字符串2
    * 0 字符串1等于字符串2
    * > 0 字符串1大于字符串2
    */
    inline int32 SDAPI SDStrncmp( const char *string1, const char *string2, uint32 count )
    {
        return strncmp( string1, string2, count );
    }

    /**
    * @brief
    * 比较Unicode字符串
    * @param string1 : 以0结束的Unicode字符串1
    * @param string2 : 以0结束的Unicode字符串2
    * @param count : 最大比较长度
    * @return 比较结果
    * < 0 字符串1小于字符串2
    * 0 字符串1等于字符串2
    * > 0 字符串1大于字符串2
    */
    inline int32 SDAPI SDWcsncmp( const wchar_t *string1, const wchar_t *string2, size_t count )
    {
        return wcsncmp( string1, string2, count );
    }

    /**
    * @brief
    * 在一个字符串中查找某个字符的位置
    * @param string : 以0结束的字符串
    * @param c : 需要被查找的字符字符
    * @return 指向查找的该字符的第一次出现的位置，或者没有找到返回NULL
    */
    inline char* SDAPI SDStrchr( const char *string, int32 c )
    {
        return (char*)strchr(string, c);
    }

#ifndef APPLE
    /**
    * @brief
    * 在一个Unicode字符串中查找某个字符的位置
    * @param string : 以0结束的Unicode字符串
    * @param c : 需要被查找的字符字符
    * @return 指向查找的该字符的第一次出现的位置，或者没有找到返回NULL
    */
    inline wchar_t* SDAPI SDWcschr( const wchar_t *string, wchar_t c )
    {
        return (wchar_t*)wcschr(string, c);
    }
#endif // ifndef APPLE

    /**
    * @brief
    * 在一个字符串中逆向查找某个字符的位置
    * @param string : 以0结束的字符串
    * @param c : 需要被查找的字符字符
    * @return 指向查找的该字符的最后一次出现的位置，或者没有找到返回NULL
    */
    inline char* SDAPI SDStrrchr( const char *string, int32 c )
    {
        return (char*)strrchr(string ,c);
    }

#ifndef APPLE
	/**
    * @brief
    * 在一个Unicode字符串中逆向查找某个字符的位置
    * @param string : 以0结束的Unicode字符串
    * @param c : 需要被查找的字符字符
    * @return 指向查找的该字符的最后一次出现的位置，或者没有找到返回NULL
    */
    inline wchar_t* SDAPI SDWcsrchr( const wchar_t *string, wchar_t c )
    {
        return (wchar_t*)wcsrchr(string, c);
    }

    /**
    * @brief
    * 无视字符大小写，比较字符串
    * @param string1 : 以0结束的字符串1
    * @param string2 : 以0结束的字符串2
    * @return 比较结果
    * < 0 字符串1小于字符串2
    * 0 字符串1等于字符串2
    * > 0 字符串1大于字符串2
    */
    inline int32 SDAPI SDStrcasecmp(const char *s1, const char *s2)
    {
#if (defined(WIN32) || defined(WIN64))
        return _stricmp(s1, s2);
#else
        return strcasecmp(s1, s2);
#endif
    }

    /**
    * @brief
    * 无视字符大小写，比较字符串
    * @param string1 : 以0结束的字符串1
    * @param string2 : 以0结束的字符串2
    * @param count : 最大比较长度
    * @return 比较结果
    * < 0 字符串1小于字符串2
    * 0 字符串1等于字符串2
    * > 0 字符串1大于字符串2
    */
    inline int32 SDAPI SDStrncasecmp(const char *s1, const char *s2, uint32 count)
    {
#if (defined(WIN32) || defined(WIN64))
        return _strnicmp(s1, s2, count);
#else
        return strncasecmp(s1, s2, count);
#endif
    }

    /**
    * @brief
    * 无视字符大小写，比较Unicode字符串
    * @param string1 : 以0结束的Unicode字符串1
    * @param string2 : 以0结束的Unicode字符串2
    * @return 比较结果
    * < 0 字符串1小于字符串2
    * 0 字符串1等于字符串2
    * > 0 字符串1大于字符串2
    */
    inline int32 SDAPI SDWcscasecmp(const wchar_t *s1, const wchar_t *s2)
    {
#if (defined(WIN32) || defined(WIN64))
        return _wcsicmp(s1, s2);
#else
        return wcscasecmp(s1, s2);
#endif
    }

    /**
    * @brief
    * 无视字符大小写，比较Unicode字符串
    * @param string1 : 以0结束的Unicode字符串1
    * @param string2 : 以0结束的Unicode字符串2
    * @param count : 最大比较长度
    * @return 比较结果
    * < 0 字符串1小于字符串2
    * 0 字符串1等于字符串2
    * > 0 字符串1大于字符串2
    */
    inline int32 SDAPI SDWcsncasecmp(const wchar_t *s1, const wchar_t *s2, uint32 count)
    {
#if (defined(WIN32) || defined(WIN64))
        return _wcsnicmp(s1, s2, count);
#else
        return wcsncasecmp(s1, s2, count);
#endif
    }

    /**
    * @brief
    * 将某个字符串切分为多个字符串
    * @param pszStr : 第一次输入被切分的字符串，之后输入NULL
    * @param delim : 分割字符串，如“:”，“,”，“ ”等
    * @return 依次返回被切割的子字符串，如果没有新的子字符串，返回NULL
    */
    inline char* SDAPI SDStrtok(char *pszStr, const char *delim)
    {
        return strtok(pszStr, delim);
    }

    /**
    * @brief
    * 将某个Unicode字符串切分为多个Unicode字符串
    * @param pszStr : 第一次输入被切分的Unicode字符串，之后输入NULL
    * @param delim : 分割Unicode字符串，如“:”，“,”，“ ”等
    * @return 依次返回被切割的子Unicode字符串，如果没有新的子Unicode字符串，返回NULL
    */
    inline wchar_t* SDAPI SDWcstok(wchar_t *pszStr, const wchar_t *delim)
    {
#if (defined(WIN32) || defined(WIN64))
        return wcstok(pszStr, delim);
#else
        return wcstok(pszStr, delim, nullptr);
#endif
    }


    /**
    * @brief
    * 将字符串中的所有ANSI字符转化为小写
    * @param pszStr : 以0结尾的字符串
    * @return 被转化的字符串，使用原有字符串的空间，原有字符串将被破坏
    */
    char* SDAPI SDStrlwr(char* pszStr);


    /**
    * @brief
    * 将字符串中的所有ANSI字符转化为大写
    * @param pszStr : 以0结尾的字符串
    * @return 被转化的字符串，使用原有字符串的空间，原有字符串将被破坏
    */
    char* SDAPI SDStrupr(char* pszStr);

    /**
    * @brief
    * 将Unicode字符串中的所有字符转化为小写
    * @param pszStr : 以0结尾的Unicode字符串
    * @return 被转化的Unicode字符串，使用原有Unicode字符串的空间，原有Unicode字符串将被破坏
    */
    wchar_t* SDAPI SDWcslwr(wchar_t* pszStr);

    /**
    * @brief
    * 将Unicode字符串中的所有字符转化为大写
    * @param pszStr : 以0结尾的Unicode字符串
    * @return 被转化的Unicode字符串，使用原有Unicode字符串的空间，原有Unicode字符串将被破坏
    */
    wchar_t* SDAPI SDWcsupr(wchar_t* pszStr);

    /**
    * @brief
    * 将MultiBytes的字符串转化为Unicode字符串
    * @param dest : 以0结尾的MultiBytes的字符串
    * @param src : 存放转化后的Unicode字符串
    * @param n : src的最大空间长度
    * @return 转化后的Unicode的实际长度
    */
    inline uint32 SDAPI SDMbsToWcs(wchar_t *dest, const char *src, uint32 n)
    {
#if (defined(WIN32) || defined(WIN64))
        return MultiByteToWideChar(CP_ACP, 0, src, n, dest, n);
#else
        return mbstowcs(dest, src, n);
#endif
    }

    /**
    * @brief
    * 将Unicode的字符串转化为MultiBytes字符串
    * @param dest : 以0结尾的Unicode的字符串
    * @param src : 存放转化后的MultiBytes字符串
    * @param n : src的最大空间长度
    * @return 转化后的MultiBytes的实际长度
    */
    inline uint32 SDAPI SDWcsToMbs(char *dest, const wchar_t *src, uint32 n)
    {
#if (defined(WIN32) || defined(WIN64))
        return WideCharToMultiByte(CP_ACP, 0, src, n, dest, n, nullptr, nullptr);
#else
        return wcstombs(dest, src, n);
#endif
    }

#endif // ifndef APPLE

///////////////////////////////////////////////////////////////////////////
#define SDSprintf sprintf
#define SDSwprintf swprintf

#if (defined(WIN32) || defined(WIN64))
#define SDSnprintf _snprintf
#else
#define SDSnprintf snprintf
#endif

#if (defined(WIN32) || defined(WIN64))
#define SDSnwprintf _snwprintf
#else
#define SDSnwprintf swprintf
#endif


///////////////////////////////////////////////////////////////////////
#define SDAtoi atoi

#if (defined(WIN32) || defined(WIN64))
#define SDAtoi64 _atoi64
#else
#define SDAtoi64 atoll
#endif

	/**
	* @brief
	* 将字符串转换为32位无符号数字
	* @param pBuf : 转化后的字符存储空间
	* @param buflen : 字符存储空间的最大长度
	* @param dwNum : 将被转化的数字
	* @return 转化后字符串使用的空间长度
	*/
    uint32 SDAPI SDAtou(const char* pStr);

	/**
	* @brief
	* 将字符串转换为64位无符号数字
	* @param pBuf : 转化后的字符存储空间
	* @param buflen : 字符存储空间的最大长度
	* @param dwNum : 将被转化的数字
	* @return 转化后字符串使用的空间长度
	*/
	uint64 SDAPI SDAtou64(const char* pStr); 

    /**
    * @brief
    * 将一个数字转化为字符串
    * @param pBuf : 转化后的字符存储空间
    * @param buflen : 字符存储空间的最大长度
    * @param dwNum : 将被转化的数字
    * @return 转化后字符串使用的空间长度
    */
    inline int32 SDAPI SDItoa(char* pBuf, uint32 buflen, int32 dwNum)
    {
        return SDSnprintf(pBuf, buflen, "%d", dwNum);
    }

	/**
	* @brief
	* 将一个数字转化为字符串
	* @param pBuf : 转化后的字符存储空间
	* @param buflen : 字符存储空间的最大长度
	* @param dwNum : 将被转化的数字
	* @return 转化后字符串使用的空间长度
	*/
    template <uint32 bufLen>
    inline int32 SDAPI SDItoa(char (&buf)[bufLen],int32 dwNum)
    {
        return SDSnprintf(buf, bufLen, "%d", dwNum);
    }

	/**
	* @brief
	* 将有64位符号整型转换为字符串
	* @param pBuf : 转化后的字符存储空间
	* @param buflen : 字符存储空间的最大长度
	* @param dwNum : 将被转化的数字
	* @return 转化后字符串使用的空间长度
	*/
	inline int32 SDAPI SDItoa64(char *pBuf, uint32 buflen, int64 dqNum)
	{
		return SDSnprintf(pBuf, buflen, SDFMT_I64 , dqNum);
	}

	/**
	* @brief
	* 将无符号整型转换为字符串
	* @param pBuf : 转化后的字符存储空间
	* @param buflen : 字符存储空间的最大长度
	* @param dwNum : 将被转化的数字
	* @return 转化后字符串使用的空间长度
	*/
	inline int32 SDAPI SDUtoa(char *pBuf, uint32 buflen, uint32 dwNum)
	{
		return SDSnprintf(pBuf, buflen, "%u", dwNum);
	}

	/**
	* @brief
	* 将无符号整型转换为字符串
	* @param pBuf : 转化后的字符存储空间
	* @param buflen : 字符存储空间的最大长度
	* @param dwNum : 将被转化的数字
	* @return 转化后字符串使用的空间长度
	*/
    template <uint32 bufLen>
    inline int32 SDAPI SDUtoa(char (&buf)[bufLen],int32 dwNum)
    {
        return SDSnprintf(buf, bufLen, "%u", dwNum);
    }

	/**
	* @brief
	* 将有64位无符号整型转换为字符串
	* @param pBuf : 转化后的字符存储空间
	* @param buflen : 字符存储空间的最大长度
	* @param dwNum : 将被转化的数字
	* @return 转化后字符串使用的空间长度
	*/
	template <uint32 bufLen>
	inline int32 SDAPI SDUtoa64(char (&buf)[bufLen], uint64 dqNum)
	{
		return SDSnprintf(buf, bufLen, SDFMT_U64, dqNum);
	}

    /**
    * @brief
    * 将字符串转化为浮点数
    * @param nptr : 将被转化的以0结尾的字符串
    * @param endptr : [输出参数]若不为NULL，则遇到无法转化的字符从这里返回其指针
    * @return 获取的浮点数
    */
    inline double SDAPI SDStrtod(const char *nptr, char **endptr)
    {
        return strtod(nptr, endptr);
    }

    /**
    * @brief
    * split a string将字符串分割为一个字符串的vector
    * @param sSource : 被分割的字符串
    * @param delim : 分隔符，如:“:”，“,”，“ ”等
    * @return a vector to store strings splited from sSource
    */
    std::vector<std::string> SDAPI SDSplitStrings(const std::string &sSource, char delim);

    /**
    * @brief
    * 将字符串转化为大写
    * @param pszStr : 将被转化的字符串
    * @return 转化后的字符串
    */
    std::string SDAPI SDStrupr(std::string &pszStr);

    /**
    * @brief
    * 将字符串转化为小写
    * @param pszStr : 将被转化的字符串
    * @return 转化后的字符串
    */
    std::string SDAPI SDStrlwr(std::string &pszStr);

    /**
    * @brief
    * 将一个字符串置空
    * @param Destination : 需要置空的类型
    * @return void
    */
    template <class T>
    inline void SDAPI SDZeroString(T &Destination) throw()
    {
        (static_cast<char[sizeof(Destination)]>(Destination));
        Destination[0] = '\0';
        Destination[sizeof(Destination)-1] = '\0';
    }


    /**
    * @brief
    * 将二进制Buffer转换为字符串,
    * @param pBuf    二进制Buffer指针
    * @param bufLen  二进制Buffer长度
    * @param pSplitter 输出的二进制数据间隔符
	* @param lineLen 源字符串分割多行的每行长度,该长度不包含分隔符的长度
    * @return void
    */
    std::string SDAPI SDBufferToHex(const char *pBuf, uint32 bufLen, const char *pSplitter = "", int32 lineLen = -1);


    std::string SDAPI SDIntToHex(int32 dwNum);

    /**
    *   去掉字符串中的字符
    */
    std::string SDAPI SDStrTrim(const std::string& str, const char* trimStr = " \r\t\n");


     /**
    * @brief
    * 将str转换为时间戳
    * @param pszStr 时间字符串
    * @return 转化后的时间戳
    */
    uint32 SDAPI SDStrTimeStamp(std::string& pszStr);

     /**
    * @littlefeet
    * 将整数转换为string
    * @number 整数
    * @return 转化后的string
    */
    std::string ToString(int32 number);
    std::string ToString(uint32 number);
    std::string ToString(int64 number);
    std::string ToString(uint64 number);
    std::string ToString(double number);

#if (defined(WIN32) || defined(WIN64))
    // utf8 转为 unicode
    std::wstring Utf8ToUnicode(const std::string& utf8string);

    //unicode 转为 ascii
    std::string WideByteToAcsii(std::wstring& wstrcode);

    //utf-8 转 ascii
    std::string Utf8ToAscii(std::string& strUtf8Code);

    //ascii 转 Unicode
    std::wstring AcsiToWideByte(std::string& strascii);

    // Unicode 转 Utf8
    std::string UnicodeToUtf8(const std::wstring& widestring);

    // ascii 转 Utf8
    std::string AsciiToUtf8(std::string& strAsciiCode);
#endif

}//namespace Crown

#endif

