#pragma once

#ifndef SDRSAHLP_H
#define SDRSAHLP_H
/**
* @file sdrsahlp.h
* @author wangkui
* @brief 处理RSA编解码
*
**/
#include "sdmacros.h"
#include "sdtype.h"

#if SDU_WITH_OPENSSL

#include <openssl/rsa.h>
#include <openssl/crypto.h>
namespace Crown
{
    /**
    * @defgroup groupcrypto 加解密操作
    * @ingroup  Crown
    * @{
    */

    /**
    *@brief RSA辅助操作，必须包含openssl库
    */

    /**
    * @brief
    * 产生rsa key
    * @param nBit : 密钥的位数
    * @param pstPublicKey : 生成的公钥
    * @param pstPrivateKey : 生成的私钥
    * @return 成功返回true，失败返回false
    */
    bool GenerateRsaKey(int32 nBit, RSA* pstPublicKey, RSA* pstPrivateKey);

    /**
    * @brief
    * 保存公钥到文件
    * @param pstPublicKey : 对应的公钥指针
    * @param pszFile : 保存的文件名
    * @return 成功返回true，失败返回false
    */
    bool SavePublicKey(const RSA* pstPublicKey, const char* pszFile);

    /**
    * @brief
    * 保存私钥到文件
    * @param pstPrivateKey : 对应的私钥指针
    * @param pszFile : 保存的文件名
    * @return 成功返回true，失败返回false
    */
    bool SavePrivateKey(const RSA* pstPrivateKey, const char* pszFile);

    /**
    * @brief
    * 从文件中载入公钥
    * @param pstPublicKey : 对应的公钥指针
    * @param pszFile : 公钥对应的文件名
    * @return 成功返回true，失败返回false
    */
    bool LoadPublicKey(RSA*& pstPublicKey, const char* pszFile);

    /**
    * @brief
    * 从文件中载入公钥
    * @param pstPrivateKey : 对应的公钥指针
    * @param pszFile : 公钥对应的文件名
    * @return 成功返回true，失败返回false
    */
    bool LoadPrivateKey(RSA*& pstPrivateKey, const char* pszFile);

    /**
    * @brief
    * 从内存中载入公钥
    * @param pstKey : 公钥指针
    * @param pbyKeyBuf : 内存的地址
    * @param wKeyLen : 内存的长度
    * @return void
    */
    void LoadPublicKeyFromMem(RSA* &pstKey, byte* pbyKeyBuf, uint16 wKeyLen);

    /**
    * @brief
    * 从内存中载入私钥
    * @param pstKey : 私钥指针
    * @param pbyKeyBuf : 内存的地址
    * @param wKeyLen : 内存的长度
    * @return void
    */
    void LoadPrivateKeyFromMem(RSA* &pstKey, byte* pbyKeyBuf, uint16 wKeyLen);

    /**
    * @brief
    * 对数据进行加密
    * @param from : 需要解密的缓冲区地址
    * @param dwFromSize : 加密缓冲区长度
    * @param to : 加密数据存储位置
    * @param pPubKey : 加密用的公钥
    * @return void
    */
    uint32 PubEncrypt(byte* from, uint32 dwFromSize, byte* to, RSA* pPubKey);

    /**
    * @brief
    * 对数据进行解密
    * @param from : 需要解密的缓冲区地址
    * @param dwFromSize : 需要解密的缓冲区长度
    * @param to : 解密数据存储位置
    * @param pPriKey : 解密用的私钥
    * @return void
    */
    uint32 PriDecrypt(byte* from, uint32 dwFromSize, byte* to, RSA* pPriKey);

    /**
    * @brief
    * 释放公钥资源
    * @param pstPublicKey : 公钥指针
    * @return void
    */
    void FreePublicKey(RSA* &pstPublicKey);

    /**
    * @brief
    * 释放私钥资源
    * @param pstPrivateKey : 私钥指针
    * @return void
    */
    void FreePrivateKey(RSA* &pstPrivateKey);

	/**
    * @brief
    * 释放Crypto加密模式全局内存
    * @return void
    */
    void FreeCrypto();

    

}

#endif // #if SDU_WITH_OPENSSL 

#endif /*#ifndef RSAHLP_H__ */

