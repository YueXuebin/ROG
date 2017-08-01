#pragma once

/******************************************************************************
Copyright (C) Shanda Corporation. All rights reserved.


******************************************************************************/
#ifndef SDRSA_H
#define SDRSA_H
/**
* @file sdrsa.h
* @author wangkui
* @brief 处理RSA编解码
*
**/

#include <stdarg.h>
#include <cstdlib>
#include <string.h>
#include <memory.h>
#include <stdio.h>
#include <time.h>
#include "sdtype.h"



namespace Crown
{
    /**
    * @defgroup groupcrypto 加解密操作
    * @ingroup  Crown
    * @{
    */

#define SDCRYPTO_HAVE_ASM 

    /**
    *@brief RSA error code macros
    */
#define SDCRYPTO_ERR_RSA_BAD_INPUT_DATA       -0x0400
#define SDCRYPTO_ERR_RSA_INVALID_PADDING      -0x0410
#define SDCRYPTO_ERR_RSA_KEY_GEN_FAILED       -0x0420
#define SDCRYPTO_ERR_RSA_KEY_CHECK_FAILED     -0x0430
#define SDCRYPTO_ERR_RSA_PUBLIC_FAILED        -0x0440
#define SDCRYPTO_ERR_RSA_PRIVATE_FAILED       -0x0450
#define SDCRYPTO_ERR_RSA_VERIFY_FAILED        -0x0460

    /**
    *@brief PKCS#1 constants
    */
#define RSA_RAW         0
#define RSA_MD2         2
#define RSA_MD4         3
#define RSA_MD5         4
#define RSA_SHA1        5
#define RSA_SHA256      6

#define RSA_PUBLIC      0
#define RSA_PRIVATE     1

#define RSA_PKCS_V15    0
#define RSA_PKCS_V21    1

#define RSA_SIGN        1
#define RSA_CRYPT       2

    /**
    * @brief typedef Big Number structure
    * be used to hold big integer more than 100 bits
    * 定义大数结构体
    */
    typedef struct stBigNum
    {
        int32		s;			
        uint32		n;			
        uint32*		p;			
    } BigNum;

    /**
    * @brief typedef RSA context structure
    * 定义RSA上下文结构体
    */
    typedef struct stRSAContext
    {
        int32 ver;				
        int32 len;				
        BigNum N;				
        BigNum E;				
        BigNum D;				
        BigNum P;				
        BigNum Q;				
        BigNum DP;				
        BigNum DQ;				
        BigNum QP;				
        BigNum RN;				
        BigNum RP;				
        BigNum RQ;				
        int32 padding;			
        int32 hash_id;			
        int32 (*f_rng)(void *);	
        void *p_rng;			
    } SRSAContext;

    /**
    *@brief rsa加解密封装类
    */
    class CSDRsa
    {
    public:

        /**
        * @brief
        * Initialize an RSA context 初始化一个RSA上下文
        * @param pCtx : RSA context to be initialized 将要被初始化的RSA上下文
        * @return void
        */
        void SDAPI RSAContextInit( SRSAContext *pCtx );

        /**
        * @brief
        * Free the components of an RSA key 释放一个RSA上下文
        * @param pCtx : RSA context to be freed 将要被释放的RSA上下文
        * @return void
        */
        void SDAPI RSAContextFree( SRSAContext *pCtx );

        /**
        * @brief
        * Get the length of RSA 获取RSA的长度
        * @param pCtx : RSA context 输入的RSA上下文
        * @return the length of RSA 返回RSA的长度
        */
        int32 SDAPI RSANSize( SRSAContext *pCtx );

        /**
        * @brief
        * Get a new RSA context 获取一个新的RSA上下文
        * @return pointer of new RSA context, return nullptr if failed
        * 返回一个新的RSA上下文的指针，如果返回NULL，代表失败
        */
        SRSAContext* SDAPI RSANew();

        /**
        * @brief
        * Generate an RSA keypair 生成一个RSA公钥密钥对
        * @param pKeyPair : RSA context that will hold the key
        * 将要持有此RSA公钥密钥对的RSA上下文
        * @param nNBits : size of the public key in bits
        * 公钥的bit位数
        * @param nExponent : public exponent 公共指数
        * @return success return true, else false 成功返回true，失败返回false
        */
        bool SDAPI RSAGenKeyPair(SRSAContext *pKeyPair, int32 nNBits = 1024, int32 nExponent = 65537);

        /**
        * @brief
        * Get public key length 获取一个公钥长度
        * @param pPubKey : RSA context that hold the key
        * 持有此RSA公钥的RSA上下文
        * @return the length of public key
        * 此RSA公钥的长度
        */
        int32 SDAPI RSAGetPubKeyLen(SRSAContext *pPubKey);

        /**
        * @brief
        * Get private key length 获取密钥的长度
        * @param pPrivKey : RSA context that hold the key
        * 持有此RSA密钥的RSA上下文
        * @return the length of private key
        * 此RSA密钥的长度
        */
        int32 SDAPI RSAGetPrivKeyLen(SRSAContext *pPrivKey);

        /**
        * @brief
        * Save public key to file 存储公钥到文件
        * @param pPubKey : RSA context that hold the key
        * 持有此RSA公钥的RSA上下文
        * @param pszFile : file name which save the public key
        * 存储的文件路径
        * @return sucess return true, else false 是否成功，true为成功，false为失败
        */
        bool  SDAPI RSASavePubKey(SRSAContext *pPubKey, const char *pszFile);

        /**
        * @brief
        * Save public key to memory 存储公钥到到内存
        * @param pPubKey : RSA context that hold the key
        * 持有此RSA公钥的RSA上下文
        * @param pBuffer : buffer to save the key
        * 存储此RSA公钥的缓冲区
        * @param nBufLen : buffer length
        * 此缓冲区的长度
        * @return sucess return true, else false 是否成功，true为成功，false为失败
        */
        bool  SDAPI RSASavePubKeyToMem(SRSAContext *pPubKey, byte* pBuffer, int32 nBufLen);

        /**
        * @brief
        * load public key from file 从文件中载入公钥
        * @param pPubKey : RSA context that hold the key
        * 将要持有此RSA公钥的RSA上下文
        * @param pszFile : file name which save the public key
        * 需要载入公钥的文件路径
        * @return sucess return true, else false 是否成功，true为成功，false为失败
        */
        bool  SDAPI RSALoadPubKey(SRSAContext *&pPubKey, const char *pszFile);

        /**
        * @brief
        * load public key from memory 从内存中载入公钥
        * @param pPubKey : RSA context that will hold the key
        * 将要持有此RSA公钥的RSA上下文
        * @param pBuffer : buffer to save the key
        * 需要载入公钥的缓冲区
        * @param nBufLen : buffer length 缓冲区长度
        * @return sucess return true, else false 是否成功，true为成功，false为失败
        */
        bool  SDAPI RSALoadPubKeyFromMem(SRSAContext *&pPubKey, const byte* pBuffer, int32 nBufLen);

        /**
        * @brief
        * Save private key to file 存储密钥到文件
        * @param pPrivKey : RSA context that will hold the key
        * 持有此RSA密钥的RSA上下文
        * @param pszFile : file name which save the public key
        * 存储密钥的文件路径
        * @return sucess return true, else false 是否成功，true为成功，false为失败
        */
        bool  SDAPI RSASavePrivKey(SRSAContext *pPrivKey, const char *pszFile);

        /**
        * @brief
        * Save private key to memory 存储密钥到到内存
        * @param pPrivKey : RSA context that will hold the key
        * 持有此RSA密钥的RSA上下文
        * @param pBuffer : buffer to save the key
        * 存储此RSA密钥的缓冲区
        * @param nBufLen : buffer length
        * 此缓冲区的长度
        * @return sucess return true, else false 是否成功，true为成功，false为失败
        */
        bool  SDAPI RSASavePrivKeyToMem(SRSAContext *pPrivKey, byte* pBuffer, int32 nBufLen);

        /**
        * @brief
        * load private key from file 从文件中载入密钥
        * @param pPrivKey : RSA context that will hold the key
        * 将要持有此RSA密钥的RSA上下文
        * @param pszFile : file name which save the public key
        * 需要载入密钥的文件路径
        * @return sucess return true, else false 是否成功，true为成功，false为失败
        */
        bool  SDAPI RSALoadPrivKey(SRSAContext *&pPrivKey, const char *pszFile);

        /**
        * @brief
        * load private key from memory 从内存中载入密钥
        * @param pPrivKey : RSA context that will hold the key
        * 将要持有此RSA密钥的RSA上下文
        * @param pBuffer : buffer to save the key
        * 需要载入密钥的缓冲区
        * @param nBufLen : buffer length
        * 此缓冲区的长度
        * @return sucess return true, else false 是否成功，true为成功，false为失败
        */
        bool  SDAPI RSALoadPrivKeyFromMem(SRSAContext *&pPrivKey, const byte* pBuffer, int32 nBufLen);

        /**
        * @brief
        * encrypt from public key 从公钥编码
        * @param pPubKey : RSA context that will hold the key
        * 持有此RSA公钥的RSA上下文
        * @param nInLen : input buffer size 输入缓冲区的大小
        * @param pInput : input buffer 输入的缓冲区
        * @param pOutput : output buffer 输出的缓冲区
        * @return length of encrypt buffer 编码的长度
        */
        int32 SDAPI RSAPubEncrypt(SRSAContext *pPubKey,
                                  int32		  nInLen,
                                  byte *pInput,
                                  byte *pOutput);

        /**
        * @brief
        * decrypt from private key 从密钥解码
        * @param pPrivKey : RSA context that will hold the key
        * 持有此RSA密钥的RSA上下文
        * @param nInLen : input buffer size 输入缓冲区的大小
        * @param pInput : input buffer 输入的缓冲区
        * @param pOutput : output buffer 输出的缓冲区
        * @return length of decrypt buffer 解码的长度
        */
        int32 SDAPI RSAPrivDecrypt(SRSAContext *pPrivKey,
                                   int32		  nInLen,
                                   byte *pInput,
                                   byte *pOutput);

    private:

        bool RSAReadPubKeySub(SRSAContext* pPubKey, byte* pBuf, int32 nRet);

        bool RSAReadPrivKeySub(SRSAContext* pPrivKey, byte* pBuf, int32 nRet);

        bool RSAWritePubKeySub(SRSAContext* pPubKey, byte* pBuf, int32 nRet);

        bool RSAWritePrivKeySub(SRSAContext* pPrivKey, byte* pBuf, int32 nRet);

        int32 RSAGetPubKeyLenSub(SRSAContext *pPubKey);

        int32 RSAGetPrivKeyLenSub(SRSAContext *pPrivKey);
        //
    };


    

}


#endif //SD_RSA_H

