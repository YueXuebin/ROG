#pragma once

#ifndef SDCRC_H
#define SDCRC_H
/**
* @file sdcrc.h
* @author lw
* @brief crc check
*
**/
#include "sdtype.h"

namespace Crown
{
    /**
    * @defgroup groupsrc Crc校验
    * @ingroup  Crown
    * @{
    */

    /**
    * @brief
    * 得到一块缓冲区对应的crc校验码
    * @param pBuf : 输入的缓冲区
    * @param dwLen : 缓冲区长度
    * @param dwCrcVal : 必须大于等于0，小于256，编码位
    * @return crc校验码
    */
    uint32 SDAPI SDCRC(const void* pBuf, uint32 dwLen, uint32 dwCrcVal = 0);

    /**
    * @brief
    * 计算文件内容的crc校验码,注意计算CRC时使用了二进制方式读取,
    * 如果对同一个文件的crc值不一致,先确认文件的读取方式.
    * @param pszFileName : 文件名
    * @return 文件内容的crc校验码，失败返回0
    */
    uint32 SDAPI SDFileCRC(const char* pszFileName, bool bFileMapping = true);

}

#endif
