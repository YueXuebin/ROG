#pragma once

#ifndef __SDRSA_H__
#define __SDRSA_H__

/**
* @file sdrsahlp.h
* @author wangkui
* @brief 处理RSA编解码
*
**/
#include "sdmacros.h"
#include "sdtype.h"

#if SDU_WITH_OPENSSL

namespace Crown
{
	typedef void* RSAKEY;

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
    * @param pPublicKey : 生成的公钥
    * @param pPrivateKey : 生成的私钥
    * @return 成功返回true，失败返回false
    */
    bool SDAPI GenerateRsaKey(int32 nBit, RSAKEY *pPublicKey, RSAKEY *pPrivateKey);

    /**
    * @brief
    * 保存公钥到文件
    * @param PublicKey : 对应的公钥指针
    * @param pszFile : 保存的文件名
    * @return 成功返回true，失败返回false
    */
    bool SDAPI SavePublicKey(const RSAKEY PublicKey, const char *pszFile);

    /**
    * @brief
    * 保存私钥到文件
    * @param PrivateKey : 对应的私钥指针
    * @param pszFile : 保存的文件名
    * @return 成功返回true，失败返回false
    */
    bool SDAPI SavePrivateKey(const RSAKEY PrivateKey, const char* pszFile);

    /**
    * @brief
    * 从文件中载入公钥
    * @param pPublicKey : 对应的公钥指针
    * @param pszFile : 公钥对应的文件名
    * @return 成功返回true，失败返回false
    */
    bool SDAPI LoadPublicKey(RSAKEY* pPublicKey, const char* pszFile);

    /**
    * @brief
    * 从文件中载入公钥
    * @param pPrivateKey : 对应的公钥指针
    * @param pszFile : 公钥对应的文件名
    * @return 成功返回true，失败返回false
    */
    bool SDAPI LoadPrivateKey(RSAKEY* pPrivateKey, const char* pszFile);

	/**
    * @brief
    * 保存公钥到内存
    * @param PublicKey : 对应的公钥指针
	* @param pbyKeyBuf : 内存的地址
    * @param wKeyLen : 内存的长度
    * @return 成功返回true，失败返回false
    */
    uint16 SDAPI SavePublicKeyToMem(const RSAKEY PublicKey, byte *pbyKeyBuf, uint16 wKeyLen);

    /**
    * @brief
    * 保存私钥到内存
    * @param PrivateKey : 对应的私钥指针
	* @param pbyKeyBuf : 内存的地址
    * @param wKeyLen : 内存的长度
    * @return 成功返回true，失败返回false
    */
    uint16 SDAPI SavePrivateKeyToMem(const RSAKEY PrivateKey, byte *pbyKeyBuf, uint16 wKeyLen);

    /**
    * @brief
    * 从内存中载入公钥
    * @param PublicKey : 公钥指针
    * @param pbyKeyBuf : 内存的地址
    * @param wKeyLen : 内存的长度
    * @return void
    */
    void SDAPI LoadPublicKeyFromMem(RSAKEY PublicKey, byte *pbyKeyBuf, uint16 wKeyLen);

    /**
    * @brief
    * 从内存中载入私钥
    * @param PrivateKey : 私钥指针
    * @param pbyKeyBuf : 内存的地址
    * @param wKeyLen : 内存的长度
    * @return void
    */
    void SDAPI LoadPrivateKeyFromMem(RSAKEY PrivateKey, byte *pbyKeyBuf, uint16 wKeyLen);

    /**
    * @brief
    * 对数据进行加密
    * @param from : 需要解密的缓冲区地址
    * @param dwFromSize : 加密缓冲区长度
    * @param to : 加密数据存储位置
    * @param PublicKey : 加密用的公钥
    * @return 返回的加密后的长度
    */
    uint32 SDAPI PubEncrypt(RSAKEY PublicKey, byte *from, uint32 dwFromSize, byte *to);

    /**
    * @brief
    * 对数据进行解密
    * @param from : 需要解密的缓冲区地址
    * @param dwFromSize : 需要解密的缓冲区长度
    * @param to : 解密数据存储位置
    * @param PrivateKey : 解密用的私钥
    * @return 返回的解密后的长度
    */
    uint32 SDAPI PriDecrypt(RSAKEY PrivateKey, byte *from, uint32 dwFromSize, byte *to);

	enum ERsaSign {
		NID_undef = 0,
		NID_md5 = 4,
		NID_sha = 41,
		NID_sha1 = 64,
		NID_md5_sha = 114,
	};

	/**
	* @brief
	* 用密钥获取信息的签名，需要注意的是，msglen最好不要超过100
	* @param type : 使用摘要的算法
	* @param m : 需要签名的字节串
	* @param msglen : 字节串长度
	* @param sigret : [输入输出参数]返回的签名
	* @param siglen : [输入输出参数]返回的签名长度
	* @param PrivateKey : 签名的密钥
	* @return 是否获取签名成功，true为成功，false为失败
	*/
	bool SDAPI PriSign(RSAKEY PrivateKey, const byte *msg, uint32 msglen, byte *sigret, uint32 *siglen, int32 type = NID_md5);

	/**
	* @brief
	* 用公钥验证签名，需要注意的是，msglen最好不要超过100
	* @param type : 使用摘要的算法
	* @param m : 需要验证的字符串
	* @param msglen : 字节串长度
	* @param sigret : [输入输出参数]返回的签名
	* @param siglen : [输入输出参数]返回的签名长度
	* @param PublicKey : 验证的公钥
	* @return 是否验证成功，true为成功，false为失败
	*/
	bool SDAPI PubVerify(RSAKEY PublicKey, const byte *msg, uint32 msglen, byte *sigbuf, uint32 siglen, int32 type = NID_md5);

    /**
    * @brief
    * 释放公钥资源
    * @param pstPublicKey : 公钥指针
    * @return void
    */
    void SDAPI FreePublicKey(RSAKEY* pPublicKey);

    /**
    * @brief
    * 释放私钥资源
    * @param pstPrivateKey : 私钥指针
    * @return void
    */
    void SDAPI FreePrivateKey(RSAKEY* pPrivateKey);


    
}

#endif // #if SDU_WITH_OPENSSL

#endif

