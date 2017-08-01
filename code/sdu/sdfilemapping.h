#pragma once

#ifndef SD_FILEMAPPING_H
#define SD_FILEMAPPING_H

//#include <string>

#if ( defined(WIN32) || defined(WIN64) )
//#include <Windows.h>
#else
#include <sys/types.h>
#include <sys/mman.h>
#include <stddef.h>
#include <unistd.h>
#endif

#include "sdtype.h"

namespace Crown
{
    struct SFileMapping
    {
        void*	mem;
        void*	pos;
        uint32	size;
        SDHANDLE maphandle;
    };

    /**
    * @brief
    * 映射一个文件段到内存中
    * @param stFileMapping : 文件映射结构体
    * @param pszFileName : 文件路径
    * @param dwBeg : 映射起始的文件位置，此位置必须是文件分配粒度的整数倍
    * @param dwSize : 内存大小，如果为0，整个文件都被映射
    * @return true为映射成功，false为映射失败
    */
    bool SDAPI SDFileMapping(SFileMapping& stFileMapping, const char* pszFileName, uint32 dwBeg =0 , uint32 dwSize = 0 );

    /**
    * @brief
    * 取消文件与内存的映射关系
    * @param stFileMapping : 文件映射结构体
    * @return void
    */
    void SDAPI SDFileUnMapping(SFileMapping& stFileMapping);

    /**
    * @brief
    * 将内存中的内容写入内存映射的文件中
    * @param pMem : 指向写入映射文件的内存起始地址
    * @param len : 需要写入的大小，单位byte
    * @return void
    */
    void SDAPI SDFileFlushMapping(void* pMem, uint32 len);

}

#endif	// SD_FILEMAPPING_H
