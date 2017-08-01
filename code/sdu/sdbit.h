#pragma once


#ifndef SDBIT_H
#define SDBIT_H

/**
* @file sdstring.h
* @author littlefeet
* @brief bit处理系列
*
**/

#include <stdio.h>

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
	* @
	* 按位保存数据到一个整数中
	* @param bit : 多少位
	* @param sdata : 要存储的数
	* @return : 返回存储过的数
	*/
    uint32 SDBitSaveOne(uint32 bit, uint32 sdata);

    uint32 SDBitSaveZero(uint32 bit, uint32 sdata);

	/**
	* @
	* 读取一个整数中的bit位的值
	* @param bit : 第几位
	* @param sdata : 要读取的整数
	* @return : 返回第bit位的值
	*/
    uint32 SDBitRead(uint32 bit, uint32 sdata);

	/**
	* @
	* 按位保存数据到一个整数中
	* @param bit : 多少位
	* @param sdata : 要存储的数
	* @return : 返回存储过的数
	*/
    uint16 SDBitSaveOne(uint16 bit, uint16 sdata);

    uint16 SDBitSaveZero(uint16 bit, uint16 sdata);

	/**
	* @
	* 读取一个整数中的bit位的值
	* @param bit : 第几位
	* @param sdata : 要读取的整数
	* @return : 返回第bit位的值
	*/
    uint16 SDBitRead(uint16 bit, uint16 sdata);
}
#endif  // SDBIT_H