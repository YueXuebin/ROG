#pragma once

#ifndef SDNETUTILS_H
#define SDNETUTILS_H
/**
* @file sdnetutils.h
* @author lw
* @brief 网络工具类
*
**/
#if (defined(WIN32) || defined(WIN64))
#include <WinSock2.h>
#else
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <netinet/in.h>
#include <sys/resource.h>
#endif

#include <string>
#include "sdtype.h"

namespace Crown
{

    /**
    * @defgroup groupnetutil 网络操作
    * @ingroup  Crown
    * @{
    */


    typedef  struct hostent SDHostent;

    /**
    * @brief
    * IPv4 将整型的ip地址转换为点分隔的地址表示 0.0.0.0
    * 或将IPv6地址转换为字符串表示方式
    * @param af : address family, AF_INET/AF_INET6
    * @param pSrc : 需要被转化的网络地址,
    * @param pDst : 转化后的网络地址存放的空间
    * @param size : 网络地址存放空间的最大长度
    * @return 转化后的网络地址存放的空间(`pDst'), 或者失败返回NULL
    */
    const char* SDAPI SDNetInetNtop(int32 af, const void *pSrc, char *pDst, size_t size);

    /**
    * @brief
    *  将字符串表示的IP地址转换为整型或IPv6结构表示
    * @param af : address family, AF_INET/AF_INET6
    * @param pSrc : 需要被转化的网络地址
    * @param pDst : 转化后的网络地址存放的空间
    * @  正确返回true ,如果src所表示的地址无效,或与地址族不匹配,则返回false
    */
    bool SDAPI SDNetInetPton(int32 af, const char *pSrc, void *pDst);

    /**
    * @brief
    * 将uint32类型数据从网络顺序转化为主机顺序
    * @param hostlong : uint32类型网络顺序数据
    * @return uint32类型主机顺序数据
    */
    inline uint32 SDAPI SDNtohl(uint32 netlong)
    {
        return ntohl(netlong);
    }

    /**
    * @brief
    * 将uint16类型数据从网络顺序转化为主机顺序
    * @param hostlong : uint16类型网络顺序数据
    * @return uint16类型主机顺序数据
    */
    inline uint16 SDAPI SDNtohs(uint16 netshort)
    {
        return ntohs(netshort);
    }

    /**
    * @brief
    * 将uint32类型数据从主机顺序转化为网络顺序
    * @param hostlong : uint32类型主机顺序数据
    * @return uint32类型网络顺序数据
    */
    inline uint32 SDAPI SDHtonl(uint32 hostlong)
    {
        return htonl(hostlong);
    }


    inline uint64 SDAPI SDHtonll(uint64 number)
    {
        return ( htonl( uint32((number >> 32) & 0xFFFFFFFF)) |
                 (uint64(htonl(uint32(number & 0xFFFFFFFF))) << 32));
    }

    inline uint64 SDAPI SDNtohll(uint64 number)
    {
        return ( ntohl( uint32((number >> 32) & 0xFFFFFFFF) ) |
                 (uint64 (ntohl(uint32(number & 0xFFFFFFFF)))  << 32));
    }


    /**
    * @brief
    * 将uint16类型数据从主机顺序转化为网络顺序
    * @param hostlong : uint16类型主机顺序数据
    * @return uint16类型网络顺序数据
    */
    inline uint16  SDAPI SDHtons(uint16 hostlong)
    {
        return htons(hostlong);
    }

    /**
    * @brief
    * 转化一个字符串型的IPv4的地址为一个数字型的IP地址
    * @param pszAddr : 字符串型的IPv4的地址
    * @return 数字型的IP地址
    */
    inline uint32 SDAPI SDInetAddr(const char* pszAddr)
    {
        return inet_addr(pszAddr);
    }

    /**
    * @brief
    * 转化一个数字型的IPv4的地址为一个字符串型的IP地址
    * @param lIp : 数字型的IPv4的地址
    * @return 字符串型的IP地址，如果出错，返回NULL
    */
    inline char*  SDAPI SDInetNtoa(int32 lIp)
    {
        struct in_addr addr;
#if (defined(WIN32) || defined(WIN64))
        addr.S_un.S_addr = lIp;
#else
        addr.s_addr = lIp;
#endif
        return inet_ntoa(addr);
    }

    /**
    * @brief
    * 获取机器名对应的IP地址
    * @param name : 机器名
    * @return 机器的IP地址
    */
    int32 SDAPI SDGetIpFromName(const char * pszName);

	/**
	* @brief
	* 获取主机名	
	* @return 机器的主机名
	*/
    std::string SDAPI SDGetHostName();

	/**
	* @brief
	* 获取主机名获取主机信息	
	* @return 机器的主机信息
	*/
    SDHostent* SDAPI SDGetHostByName(const char * pszName); // 域名解析

    /**
    * @brief
    * 获取当前机器的本地IP地址
    * @param ip :  用于获取本地的IP地址的数组
    * @param count : 数组长度
    * @return 本地IP地址的数量
    */
    int32 SDAPI SDGetLocalIp(uint32 dwIp[], uint32 dwCount);

	//TODO get mac address
    

	// 设置文件句柄数限制
	// 返回值如下：
	//  0: success
	// -1: getrlimit failed
	// -2: setrlimit failed
	int32 SetFileHandleLimit(const uint32 dwSetLimit);


    // 整数ip转字串
    std::string GetIpStr(uint32 ip);
}//

#endif /// 
