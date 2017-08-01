#pragma once

#ifndef SDMD5_H
#define SDMD5_H
/**
* @file sdmd5.h
* @author wangkui
* @brief 处理MD5编解码
*
**/
#include "sdtype.h"

namespace Crown
{

    typedef struct stMD5Context
    {
        uint32 state[4];
        uint32 count[2];
        byte buffer[64];
    } SMD5Context;


    /**
    * @brief 初始化MD5码上下文
    * @param pCtx : [输入输出参数]MD5码上下文,输入一个新的MD5码上下文
    * @return void
    */
    void SDMD5Init(SMD5Context *pCtx);

    /**
    * @brief 更新MD5码，继续一个MD5码消息摘要操作，处理另一个消息块，并更新上下文
    * @param pCtx : [输入输出参数]MD5码上下文
    * @param pInput : 输入消息块
    * @param dwInLen : 该消息块的长度
    * @return void
    */
    void SDMD5Update(SMD5Context *pCtx, byte *pInput, uint32 dwInLen);

    /**
    * @brief 完成一次MD5码消息摘要操作，写入消息摘要并清空上下文
    * @param acDigest : [输入输出参数]生成的MD5码
    * @param pCtx : [输入输出参数]MD5码上下文
    * @return void
    */
    void SDMD5Final(byte acDigest[16], SMD5Context *pCtx);

    /**
    * @brief
    * get md5 for a string buffer 获取一个字符串的md5码
    * @param acDigest : [in/out]array to store md5 val [输入输出参数]存储md5码的数组
    * @param pInput : input string 输入字符串
    * @param dwInLen : the length fo input string 输入字符串的长度
    * @return void
    */
    void SDAPI SDMD5(byte acDigest[16], byte* pInput, uint32 dwInLen);


	/**
    * @brief
    * 计算文件内容的md5校验码,注意计算MD5时使用了二进制方式读取,
    * 如果对同一个文件的md5值不一致,先确认文件的读取方式.
	* @param acDigest : [输入输出参数]存储md5码的数组
    * @param pszFileName : 文件名
	* @param bFileMapping : 是否使用文件映射
    * @return 是否成功，true为成功，false为不成功
    */
    bool SDAPI SDFileMD5(byte acDigest[16], const char* pszFileName, bool bFileMapping = true);

}

#endif

