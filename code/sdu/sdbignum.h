#pragma once

#ifndef SD_BIGNUM_H
#define SD_BIGNUM_H
/**
* @file sdbignum.h
* @author wangkui
* @brief 处理大数的函数系列
*
**/
#include "sdrsa.h"
#include "sdbignummul.h"

#ifndef WIN64
namespace Crown
{

// typedef error number
    enum eBigNumErrNo
    {
        SDCRYPTO_BNERR_FILE_IO_ERROR	 = -0x0002,
        SDCRYPTO_BNERR_BAD_INPUT_DATA    = -0x0004,
        SDCRYPTO_BNERR_INVALID_CHARACTER = -0x0006,
        SDCRYPTO_BNERR_BUFFER_TOO_SMALL  = -0x0008,
        SDCRYPTO_BNERR_NEGATIVE_VALUE    = -0x000A,
        SDCRYPTO_BNERR_DIVISION_BY_ZERO  = -0x000C,
        SDCRYPTO_BNERR_NOT_ACCEPTABLE    = -0x000E
    };

// typedef check return macro
#define BN_CHK(f)  if ( ( nRet = f ) != 0 ) \
	{						\
	goto cleanup;		\
		}


#ifdef __cplusplus
    extern "C" {
#endif

        //
        // @brief		Initialize one or more BigNum
        //
        void BNInit( BigNum *X, ...);

        //
        // @brief		 Free one or more BigNum
        //
        void BNFree( BigNum *X, ... );

        //
        // @brief          Enlarge to the specified number of limbs
        //
        // @return         0 if successful,
        //                 1 if memory allocation failed
        //
        int32 BNGrow( BigNum *X, int32 nLimbs );

        //
        // @brief          Copy the contents of Y into X
        //
        // @return         0 if successful,
        //                 1 if memory allocation failed
        //
        int32 BNCopy( BigNum *X, BigNum *Y );

        //
        // @brief           Swap the contents of X and Y
        //
        void BNSwap( BigNum *X, BigNum *Y );

        //
        // @brief           Set value from integer
        //
        // @return          0 if successful,
        //                  1 if memory allocation failed
        //
        int32 BNSetValue( BigNum *X, int32 nZ );

        //
        // @brief           Return the number of least significant bits
        //
        int32 BNGetLSB( BigNum *X );

        //
        // @brief           Return the number of most significant bits
        //
        int32 BNGetMSB( BigNum *X );

        //
        // @brief           Return the total size in bytes
        //
        int32 BNGetTotalBytes( BigNum *X );

        //
        // @brief			Convert an ASCII character to digit value
        //
        // @param d			destination digital value
        // @param nRadix	input numeric base
        // @param c			charactor
        //
        // @return		    0 if successful, or a SDCRYPTO_BNERR_XXX error code
        //
        int32 BNGetDigit( uint32 *d, int32 nRadix, char c );

        //
        // @brief            Import from an ASCII string
        //
        // @param X          destination BigNum
        // @param nRadix     input numeric base
        // @param s          null-terminated string buffer
        //
        // @return           0 if successful, or a SDCRYPTO_BNERR_XXX error code
        //
	int32 BNReadString( BigNum *X, int32 nRadix, const char *s );

        //
        // @brief			Helper to write the digits high-order first
        //
        // @param X
        // @param nRadix
        // @param p
        //
        // @return
        //
        int32 BNWriteHlp( BigNum *X, int32 nRadix, char **p );

        //
        // @brief            Export into an ASCII string
        //
        // @param X          source BigNum
        // @param nRadix     output numeric base
        // @param s          string buffer
        // @param slen       string buffer size
        //
        // @return           0 if successful, or a SDCRYPTO_BNERR_XXX error code
        //
        // @note             Call this function with *slen = 0 to obtain the
        //                   minimum required buffer size in *slen.
        //
        int32 BNWriteString( BigNum *X, int32 nRadix, char *s, int32 *sLen );

        //
        // @brief            Read X from an opened file
        //
        // @param X          destination mpi
        // @param nRadix     input numeric base
        // @param fpIn       input file handle
        //
        // @return           0 if successful, or a SDCRYPTO_BNERR_XXX error code
        //
        int32 BNReadFile( BigNum *X, int32 nRadix, FILE *fpIn );

        //
        // @brief            Write X into an opened file, or stdout
        //
        // @param p          prefix, can be nullptr
        // @param X          source mpi
        // @param nRadix     output numeric base
        // @param fpOut      output file handle
        //
        // @return           0 if successful, or a SDCRYPTO_BNERR_XXX error code
        //
        // @note             Set fpOut == nullptr to print X on the console.
        //
        int32 BNWriteFile( char *p, BigNum *X, int32 nRadix, FILE *fpOut );

        //
        // @brief            Import X from unsigned binary data, big endian
        //
        // @param X          destination mpi
        // @param pBuf       input buffer
        // @param nBufLen    input buffer size
        //
        // @return           0 if successful,
        //                   1 if memory allocation failed
        //
        int32 BNReadBinary( BigNum *X, byte *pBuf, int32 nBufLen );

        //
        // @brief           Export X INT32o unsigned binary data, big endian
        //
        // @param X         source mpi
        // @param pBuf      output buffer
        // @param nBufLen   output buffer size
        //
        // @return          0 if successful,
        //                  SDCRYPTO_BNERR_BUFFER_TOO_SMALL if buf isn't large enough
        //
        // @note            Call this function with *buflen = 0 to obtain the
        //                  minimum required buffer size in *buflen.
        //
        int32 BNWriteBinary( BigNum *X, byte *pBuf, int32 nBufLen );

        //
        // @brief           Left-shift: X <<= nCount
        //
        // @return          0 if successful,
        //                  1 if memory allocation failed
        //
        int32 BNShiftLeft( BigNum *X, int32 nCount );

        //
        // @brief           Right-shift: X >>= nCount
        //
        // @return          0 if successful,
        //                  1 if memory allocation failed
        //
        int32 BNShiftRight( BigNum *X, int32 nCount );

        //
        // @brief           Compare unsigned values
        //
        // @return         1 if |X| is greater than |Y|,
        //                -1 if |X| is lesser  than |Y| or
        //                 0 if |X| is equal to |Y|
        //
        int32 BNCmpAbs( BigNum *X, BigNum *Y );

        //
        // @brief           Compare signed values
        //
        // @return         1 if X is greater than Y,
        //                -1 if X is lesser  than Y or
        //                 0 if X is equal to Y
        //
        int32 BNCmpMpi( BigNum *X, BigNum *Y );

        //
        // @brief           Compare signed values
        //
        // @return         1 if X is greater than z,
        //                -1 if X is lesser  than z or
        //                 0 if X is equal to z
        //
        int32 BNCmpInt( BigNum *X, int32 nZ );

        //
        // @brief           Unsigned addition: X = |A| + |B|
        //
        // @return         0 if successful,
        //                 1 if memory allocation failed
        //
        int32 BNAddAbs( BigNum *X, BigNum *A, BigNum *B );

        //
        // @brief          Helper for mpi substraction
        //
        void BNSubHlp( int32 n, uint32 *s, uint32 *d );

        //
        // @brief           Unsigned substraction: X = |A| - |B|
        //
        // @return         0 if successful,
        //                 SDCRYPTO_BNERR_NEGATIVE_VALUE if B is greater than A
        //
        int32 BNSubAbs( BigNum *X, BigNum *A, BigNum *B );

        //
        // @brief           Signed addition: X = A + B
        //
        // @return         0 if successful,
        //                 1 if memory allocation failed
        //
        int32 BNAddMpi( BigNum *X, BigNum *A, BigNum *B );

        //
        // @brief           Signed substraction: X = A - B
        //
        // @return         0 if successful,
        //                 1 if memory allocation failed
        //
        int32 BNSubMpi( BigNum *X, BigNum *A, BigNum *B );

        //
        // @brief           Signed addition: X = A + b
        //
        // @return         0 if successful,
        //                 1 if memory allocation failed
        //
        int32 BNAddInt( BigNum *X, BigNum *A, int32 b );

        //
        // @brief           Signed substraction: X = A - b
        //
        // @return         0 if successful,
        //                 1 if memory allocation failed
        //
        int32 BNSubInt( BigNum *X, BigNum *A, int32 b );

        //
        // @brief          Helper for mpi multiplication
        //
	void BNMulHlp( uint32 i, uint32 *s, uint32 *d, uint32 b );

        //
        // @brief           Baseline multiplication: X = A * B
        //
        // @return         0 if successful,
        //                 1 if memory allocation failed
        //
        int32 BNMulMpi( BigNum *X, BigNum *A, BigNum *B );

        //
        // @brief           Baseline multiplication: X = A * b
        //
        // @return         0 if successful,
        //                 1 if memory allocation failed
        //
        int32 BNMulInt( BigNum *X, BigNum *A, uint32 b );

        //
        // @brief           Division by mpi: A = Q * B + R
        //
        // @return         0 if successful,
        //                 1 if memory allocation failed,
        //                 SDCRYPTO_BNERR_DIVISION_BY_ZERO if B == 0
        //
        // @note           Either Q or R can be nullptr.
        //
        int32 BNDivMpi( BigNum *Q, BigNum *R, BigNum *A, BigNum *B );

        //
        // @brief           Division by int32: A = Q * b + R
        //
        // @return         0 if successful,
        //                 1 if memory allocation failed,
        //                 SDCRYPTO_BNERR_DIVISION_BY_ZERO if b == 0
        //
        // @note           Either Q or R can be nullptr.
        //
        int32 BNDivInt( BigNum *Q, BigNum *R, BigNum *A, int32 b );

        //
        // @brief           Modulo: R = A mod B
        //
        // @return         0 if successful,
        //                1 if memory allocation failed,
        //                SDCRYPTO_BNERR_DIVISION_BY_ZERO if B == 0
        //
        int32 BNModMpi( BigNum *R, BigNum *A, BigNum *B );

        //
        // @brief           Modulo: r = A mod b
        //
        // @return         0 if successful,
        //                1 if memory allocation failed,
        //                SDCRYPTO_BNERR_DIVISION_BY_ZERO if b == 0
        //
        int32 BNModInt( uint32 *r, BigNum *A, int32 b );

        //
        // @brief			Fast Montgomery initialization (thanks to Tom St Denis)
        //
        void BNMontgInit( uint32 *mm, BigNum *N );

        //
        // @brief			Montgomery multiplication: A = A * B * R^-1 mod N  (HAC 14.36)
        //
        void BNMontMul( BigNum *A, BigNum *B, BigNum *N, uint32 mm, BigNum *T );

        //
        // @brief			Montgomery reduction: A = A * R^-1 mod N
        //
        void BNMontRed( BigNum *A, BigNum *N, uint32 mm, BigNum *T );

        //
        // @brief           Sliding-window exponentiation: X = A^E mod N
        //
        // @return         0 if successful,
        //                1 if memory allocation failed,
        //                SDCRYPTO_BNERR_BAD_INPUT_DATA if N is negative or even
        //
        // @note           _RR is used to avoid re-computing R*R mod N across
        //                multiple calls, which speeds up things a bit. It can
        //                be set to nullptr if the extra performance is unneeded.
        //
        int32 BNExpMod( BigNum *X, BigNum *A, BigNum *E, BigNum *N, BigNum *_RR );

        //
        // @brief           Greatest common divisor: G = gcd(A, B)
        //
        // @return         0 if successful,
        //                 1 if memory allocation failed
        //
        int32 BNGcd( BigNum *G, BigNum *A, BigNum *B );

        //
        // @brief           Modular inverse: X = A^-1 mod N
        //
        // @return         0 if successful,
        //                 1 if memory allocation failed,
        //                 SDCRYPTO_BNERR_BAD_INPUT_DATA if N is negative or nil
        //                 SDCRYPTO_BNERR_NOT_ACCEPTABLE if A has no inverse mod N
        //
        int32 BNInvMod( BigNum *X, BigNum *A, BigNum *N );

        //
        // @brief           Miller-Rabin primality test
        //
        // @return         0 if successful (probably prime),
        //                 1 if memory allocation failed,
        //                 SDCRYPTO_BNERR_NOT_ACCEPTABLE if X is not prime
        //
        int32 BNIsPrime( BigNum *X, int32 (*fpRng)(void *), void *pRng );

        //
        // @brief           Prime number generation
        //
        // @param X        destination mpi
        // @param nBits    required size of X in bits
        // @param nDhFlag  if 1, then (X-1)/2 will be prime too
        // @param fpRng    RNG function
        // @param pRng     RNG parameter
        //
        // @return         0 if successful (probably prime),
        //                 1 if memory allocation failed,
        //                 SDCRYPTO_BNERR_BAD_INPUT_DATA if nBits is < 3
        //
        int32 BNGenPrime( BigNum *X, int32 nBits, int32 nDhFlag,
                          int32 (*fpRng)(void *), void *pRng );

        //
        // @brief           Checkup routine
        //
        // @return         0 if successful, or 1 if the test failed
        //
        int32 BNSelfTest( int32 nVerbose );


    };

}
#endif // 

#endif //SD_BIGNUM_H
