#pragma once

#ifndef SDCRYPTODES_H
#define SDCRYPTODES_H
/**
* @file sdcryptodes.h
* @author wangkui
* @brief 处理DES编解码
*
**/
#include "sddes.h"

namespace Crown
{

	//
	// @brief          DES key schedule (56-bit, encryption)
	//
	// @param pCtx     DES context to be initialized
	// @param acKey    8-byte secret key
	//
	void DESSetEncKey( SDESContext *pCtx, byte acKey[8] );

	//
	// @brief          DES key schedule (56-bit, decryption)
	//
	// @param pCtx     DES context to be initialized
	// @param acKey    8-byte secret key
	//
	void DESSetDecKey( SDESContext *pCtx, byte acKey[8] );

	//
	// @brief		  DES initialization vector generator
	//
	//
	//
	void DESRandomKey( SDESBlock *pKey );

	//
	// @brief          DES-ECB block encryption/decryption
	//
	// @param pCtx     DES context
	// @param acInput  64-bit input block
	// @param acOutput 64-bit output block
	//
	void DESCryptECB( SDESContext	*pCtx,
		byte acInput[8],
		byte acOutput[8] );

	//
	// @brief          DES-CBC buffer encryption/decryption
	//
	// @param pCtx        DES context
	// @param nMode       DES_ENCRYPT or DES_DECRYPT
	// @param nLength     length of the input data
	// @param pIV         initialization vector (updated after use)
	// @param pacInput    buffer holding the input data
	// @param pacOutput   buffer holding the output data
	//
	void DESCryptCBC( SDESContext	*pCtx,
		int32			nMode,
		int32			nLength,
		SDESBlock		*pIV,
		byte *pacInput,
		byte *pacOutput );



	//
	// @brief		Generate 3-DES key
	// @param pKey	3DES key
	//
	void GenDES3Key( SDES3Block  *pKey );

	//
	// @brief		Generate 3-DES key schedule table for encryption
	// @param pCtx	3-DES Context which stored key schedule table
	// @param pKey  3-DES key
	//
	void DES3SetEncKey( SDES3Context *pCtx, SDES3Block *pKey );

	//
	// @brief		Generate 3-DES key schedule table for decryption
	// @param pCtx 	3-DES Context which stored key schedule table
	// @param pKey  3-DES key
	//
	void DES3SetDecKey( SDES3Context *pCtx, SDES3Block *pKey );

	//
	// @brief		   Crypto in 3-DES ECB mode
	// @param pCtx	   3-DES context
	// @param acInput  input plain text buffer
	// @param acOutput output cipher text buffer
	//
	void DES3CryptECB( SDES3Context *pCtx,
		byte acInput[8],
		byte acOutput[8] );

	//
	// @brief			Crypto in 3-DES CBC mode
	// @param pCtx		3-DES context
	// @param nMode		encrypt/decrypt mode
	// @param nLength	input length
	// @param pIv		3-DES key
	// @param pacInput  input text buffer
	// @param pacOutput output text buffer
	//
	void DES3CryptCBC ( SDES3Context *pCtx,
		int32		nMode,
		int32		nLength,
		SDES3Block	*pIV,
		byte *pacInput,
		byte *pacOutput );



	//
	// @brief          Checkup routine
	//
	// @return         0 if successful, or 1 if the test failed
	//
	int32 DESSelfTest( int32 nVerbose );


}

#endif //SD_CRYPTODES_H
