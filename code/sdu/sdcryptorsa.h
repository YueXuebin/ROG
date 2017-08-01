#pragma once

#ifndef SD_CRYPTORSA_H
#define SD_CRYPTORSA_H
/**
* @file sdcryptodes.h
* @author wangkui
* @brief 处理RSA编解码
*
**/
#include "sdbignum.h"
#include "sdrsa.h"




namespace Crown
{

#define ASN1_HASH_MDX                       \
	"\x30\x20\x30\x0C\x06\x08\x2A\x86\x48"  \
	"\x86\xF7\x0D\x02\x00\x05\x00\x04\x10"

#define ASN1_HASH_SHA1                      \
	"\x30\x21\x30\x09\x06\x05\x2B\x0E\x03"  \
	"\x02\x1A\x05\x00\x04\x14"




#ifdef __cplusplus
    extern "C" {
#endif

        //
        // @brief          Initialize an RSA context
        //
        // @param ctx      RSA context to be initialized
        // @param padding  RSA_PKCS_V15 or RSA_PKCS_V21
        // @param hash_id  RSA_PKCS_V21 hash identifier
        // @param f_rng    RNG function
        // @param p_rng    RNG parameter
        //
        // @note           The hash_id parameter is actually ignored
        //                 when using RSA_PKCS_V15 padding.
        //
        // @note           Currently (SDCRYPTO-0.8), RSA_PKCS_V21 padding
        //                 is not supported.
        //
        void RSAInit( SRSAContext *ctx,
                      int32 padding,
                      int32 hash_id,
                      int32 (*f_rng)(void *),
                      void *p_rng );

        //
        // @brief          Generate an RSA keypair
        //
        // @param ctx      RSA context that will hold the key
        // @param nbits    size of the public key in bits
        // @param exponent public exponent (e.g., 65537)
        //
        // @note           rsa_init() must be called beforehand to setup
        //                 the RSA context (especially f_rng and p_rng).
        //
        // @return         0 if successful, or an SDCRYPTO_ERR_RSA_XXX error code
        //
        int32 RSAGenKey( SRSAContext *ctx, int32 nbits, int32 exponent );

        //
        // @brief          Check a public RSA key
        //
        // @param ctx      RSA context to be checked
        //
        // @return         0 if successful, or an SDCRYPTO_ERR_RSA_XXX error code
        //
        int32 RSACheckPubKey( SRSAContext *ctx );

        //
        // @brief          Check a private RSA key
        //
        // @param ctx      RSA context to be checked
        //
        // @return         0 if successful, or an SDCRYPTO_ERR_RSA_XXX error code
        //
        int32 RSACheckPrivKey( SRSAContext *ctx );

        //
        // @brief          Do an RSA public key operation: decrypt or encrypt
        //
        // @param ctx      RSA context
        // @param input    input buffer
        // @param output   output buffer
        //
        // @return         0 if successful, or an SDCRYPTO_ERR_RSA_XXX error code
        //
        // @note           This function does NOT take care of message
        //                 padding. Also, be sure to set input[0] = 0.
        //
        // @note           The input and output buffers must be large
        //                 enough (eg. 128 bytes if RSA-1024 is used).
        //
        int32 RSAPublic( SRSAContext *ctx,
                         unsigned char *input,
                         unsigned char *output );

        //
        // @brief          Do an RSA private key operation
        //
        // @param ctx      RSA context
        // @param input    input buffer
        // @param output   output buffer
        //
        // @return         0 if successful, or an SDCRYPTO_ERR_RSA_XXX error code
        //
        // @note           The input and output buffers must be large
        //                 enough (eg. 128 bytes if RSA-1024 is used).
        //
        int32 RSAPrivate( SRSAContext *ctx,
                          unsigned char *input,
                          unsigned char *output );

        //
        // @brief          Add the message padding, then do an RSA operation
        //
        // @param ctx      RSA context
        // @param mode     RSA_PUBLIC or RSA_PRIVATE
        // @param ilen     contains the the plain text length
        // @param input    buffer holding the data to be encrypted
        // @param output   buffer that will hold the cipher text
        //
        // @return         0 if successful, or an SDCRYPTO_ERR_RSA_XXX error code
        //
        // @note           The output buffer must be as large as the size
        //                 of ctx->N (eg. 128 bytes if RSA-1024 is used).
        //
        int32 RSAPkcs1Encrypt( SRSAContext *ctx,
                               int32 mode, int32  ilen,
                               unsigned char *input,
                               unsigned char *output );

        //
        // @brief          Do an RSA operation, then remove the message padding
        //
        // @param ctx      RSA context
        // @param mode     RSA_PUBLIC or RSA_PRIVATE
        // @param input    buffer holding the encrypted data
        // @param output   buffer that will hold the plain text
        // @param olen     will contain the plain text length
        //
        // @return         0 if successful, or an SDCRYPTO_ERR_RSA_XXX error code
        //
        // @note           The output buffer must be as large as the size
        //                 of ctx->N (eg. 128 bytes if RSA-1024 is used).
        //
        int32 RSAPkcs1Decrypt( SRSAContext *ctx,
                               int32 mode, int32 *olen,
                               unsigned char *input,
                               unsigned char *output );

        //
        // @brief          Do a private RSA to sign a message digest
        //
        // @param ctx      RSA context
        // @param mode     RSA_PUBLIC or RSA_PRIVATE
        // @param hash_id  RSA_RAW, RSA_MD{2,4,5} or RSA_SHA{1,256}
        // @param hashlen  message digest length (for RSA_RAW only)
        // @param hash     buffer holding the message digest
        // @param sig      buffer that will hold the ciphertext
        //
        // @return         0 if the signing operation was successful,
        //                 or an SDCRYPTO_ERR_RSA_XXX error code
        //
        // @note           The "sig" buffer must be as large as the size
        //                 of ctx->N (eg. 128 bytes if RSA-1024 is used).
        //
        int32 RSAPkcs1Sign( SRSAContext *ctx,
                            int32 mode,
                            int32 hash_id,
                            int32 hashlen,
                            unsigned char *hash,
                            unsigned char *sig );

        //
        // @brief          Do a public RSA and check the message digest
        //
        // @param ctx      poINT32s to an RSA public key
        // @param mode     RSA_PUBLIC or RSA_PRIVATE
        // @param hash_id  RSA_RAW, RSA_MD{2,4,5} or RSA_SHA{1,256}
        // @param hashlen  message digest length (for RSA_RAW only)
        // @param hash     buffer holding the message digest
        // @param sig      buffer holding the ciphertext
        //
        // @return         0 if the verify operation was successful,
        //                 or an SDCRYPTO_ERR_RSA_XXX error code
        //
        // @note           The "sig" buffer must be as large as the size
        //                 of ctx->N (eg. 128 bytes if RSA-1024 is used).
        //
        int32 RSAPkcs1Verify( SRSAContext *ctx,
                              int32 mode,
                              int32 hash_id,
                              int32 hashlen,
                              unsigned char *hash,
                              unsigned char *sig );

        //
        // @brief          Free the components of an RSA key
        //
        void RSAFree( SRSAContext *ctx );

        //
        // @brief          Checkup routine
        //
        // @return         0 if successful, or 1 if the test failed
        //
        int32 RSASelfTest( int32 verbose );

#ifdef __cplusplus
    }
#endif

}




#endif //SD_CRYPTORSA_H
