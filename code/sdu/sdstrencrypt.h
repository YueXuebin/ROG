#pragma once

#ifndef SDSTRENCRYPT_H
#define SDSTRENCRYPT_H
/**
* @file sdstrencrypt.h
* @author lw
* @brief 字符串加密解密系列
*
**/
#include "sdtype.h"
#include <string>


namespace Crown
{
    /**
    * @defgroup groupencrypt 字符串加密
    * @ingroup  Crown
    * @{
    */

    /**
    *@brief 字符串加密处理类
    */
    class CSDStrEncrypt
    {
    public:
        CSDStrEncrypt(void) {}
        ~CSDStrEncrypt(void) {}
        static const byte CODE_KEY = 0x96;	

        //对每个字节高低4bit互换，与KEY进行异或运算

        /**
        * @brief
        * static, encode a string
        * @param sContent : string to be encoded
        * @return void
        */
        static void SDAPI Encode(std::string& sContent);

        /**
        * @brief
        * static, encode a string
        * @param pBuf : pointer to the buf to be encode
        * @param nLen : buf len
        * @return void
        */
        static void SDAPI Encode(byte *pBuf, int32 nLen);

        /**
        * @brief
        * static, decode a string
        * @param sContent : string to be decode
        * @return void
        */
        static void SDAPI Decode(std::string &sContent);

        /**
        * @brief
        * static, decode a string
        * @param pBuf : pointer to the buf to be encode
        * @param nLen : buf len
        * @return void
        */
        static void SDAPI Decode(byte *pBuf, int32 nLen);
    };

    

} // namespace Crown



#endif //

