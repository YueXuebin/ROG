#include "SDuPCH.h"
#include "sdbignum.h"

#ifndef WIN64 
namespace Crown
{

// chars in limb
#define ciL    ((int32) sizeof(uint32))
// bits  in limb
#define biL    (ciL << 3)
// half limb size
#define biH    (ciL << 2)

// Convert between bits/chars and number of limbs
#define BITS_TO_LIMBS(i)  (((i) + biL - 1) / biL)
#define CHARS_TO_LIMBS(i) (((i) + ciL - 1) / ciL)


    void BNInit( BigNum *X, ...)
    {
        va_list args;

        va_start( args, X );

        while ( X != nullptr )
        {
            X->s = 1;
            X->n = 0;
            X->p = nullptr;

            X = va_arg( args, BigNum* );
        }

        va_end( args );
    }

    void BNFree( BigNum *X, ... )
    {
        va_list args;

        va_start( args, X );

        while ( X != nullptr )
        {
            if ( X->p != nullptr )
            {
                memset( X->p, 0, X->n * ciL );
                free( X->p );
            }

            X->s = 1;
            X->n = 0;
            X->p = nullptr;

            X = va_arg( args, BigNum* );
        }

        va_end( args );
    }


    int32 BNGrow( BigNum *X, int32 nLimbs )
    {
        uint32 *p = nullptr;

        if ( X->n < (uint32)nLimbs )
        {

#if 0
            if ( ( p = (uint32 *) malloc( nLimbs * ciL ) ) == nullptr )
            {
                return 1;
            }
#else
            p = new uint32[nLimbs];
            if (nullptr == p)
            {
                return 1;
            }
#endif
            memset( p, 0, nLimbs * ciL );

            if ( X->p != nullptr )
            {
                memcpy( p, X->p, X->n * ciL );
                memset( X->p, 0, X->n * ciL );
#if 0
                free( X->p );
#else
                delete X->p;
#endif
            }

            X->n = nLimbs;
            X->p = p;
        }

        return 0;
    }


    int32 BNCopy( BigNum *X, BigNum *Y )
    {
        int32 nRet, i;

        if ( X == Y )
        {
            return 0;
        }

        for ( i = Y->n - 1; i > 0; --i )
        {
            if ( Y->p[i] != 0 )
            {
                break;
            }
        }

        i++;

        X->s = Y->s;

        BN_CHK( BNGrow( X, i ) );

        memset( X->p, 0, X->n * ciL );
        memcpy( X->p, Y->p, i * ciL );

cleanup:

        return nRet;
    }


    void BNSwap( BigNum *X, BigNum *Y )
    {
        BigNum T;

        memcpy( &T,  X, sizeof( BigNum ) );
        memcpy(  X,  Y, sizeof( BigNum ) );
        memcpy(  Y, &T, sizeof( BigNum ) );
    }


    int32 BNSetValue( BigNum *X, int32 nZ )
    {
        int32 nRet;

        BN_CHK( BNGrow( X, 1 ) );
        memset( X->p, 0, X->n * ciL );

        X->p[0] = ( nZ < 0 ) ? -nZ : nZ;
        X->s    = ( nZ < 0 ) ? -1  : 1;

cleanup:

        return nRet;
    }


    int32 BNGetLSB( BigNum *X )
    {
        int32 i, j, nCount = 0;

        for ( i = 0; i < (int32)X->n; ++i )
        {
            for ( j = 0; j < (int32) biL; ++j, ++nCount )
            {
                if ( ( ( X->p[i] >> j ) & 1 ) != 0 )
                {
                    return nCount;
                }
            }
        }

        return 0;
    }


    int32 BNGetMSB( BigNum *X )
    {
        int32 i, j;

        for ( i = X->n - 1; i > 0; --i )
        {
            if ( X->p[i] != 0 )
            {
                break;
            }
        }

        for ( j = biL - 1; j >= 0; --j )
        {
            if ( ( ( X->p[i] >> j ) & 1 ) != 0 )
            {
                break;
            }
        }

        return( ( i * biL ) + j + 1 );
    }

    int32 BNGetTotalBytes( BigNum *X )
    {
        return( ( BNGetMSB( X ) + 7 ) >> 3 );
    }

    int32 BNGetDigit( uint32 *d, int32 nRadix, char c )
    {
        *d = 255;

        if ( c >= 0x30 && c <= 0x39 )
        {
            *d = c - 0x30;
        }

        if ( c >= 0x41 && c <= 0x46 )
        {
            *d = c - 0x37;
        }

        if ( c >= 0x61 && c <= 0x66 )
        {
            *d = c - 0x57;
        }

        if ( *d >= (uint32) nRadix )
        {
            return SDCRYPTO_BNERR_INVALID_CHARACTER;
        }

        return 0;
    }

    int32 BNReadString( BigNum *X, int32 nRadix, const char *s )
    {
        int32  nRet, i, j, n;
        uint32 d;
        BigNum T;

        if ( nRadix < 2 || nRadix > 16 )
        {
            return SDCRYPTO_BNERR_BAD_INPUT_DATA;
        }

        BNInit( &T, nullptr );

        if ( 16 == nRadix )
        {
            n = BITS_TO_LIMBS( (int32)( strlen( s ) << 2 ) );

            BN_CHK( BNGrow( X, n ) );
            BN_CHK( BNSetValue( X, 0 ) );

            for ( i = (int32)strlen( s ) - 1, j = 0; i >= 0; --i, ++j )
            {
                if( 0 == i && s[i] == '-' )
                {
                    X->s = -1;
                    break;
                }

                BN_CHK( BNGetDigit( &d, nRadix, s[i] ) );
                X->p[j / (2 * ciL)] |= d << ( (j % (2 * ciL)) << 2 );
            }
        }
        else
        {
            BN_CHK( BNSetValue( X, 0 ) );

            for ( i = 0; i < (int32) strlen( s ); i++ )
            {
                if( 0 == i && s[i] == '-' )
                {
                    X->s = -1;
                    continue;
                }

                BN_CHK( BNGetDigit( &d, nRadix, s[i] ) );
                BN_CHK( BNMulInt( &T, X, nRadix ) );
                BN_CHK( BNAddInt( X, &T, d ) );
            }
        }

cleanup:

        BNFree( &T, nullptr );

        return nRet;

    }

    int32 BNWriteHlp( BigNum *X, int32 nRadix, char **p )
    {
        int32  nRet;
        uint32 r;

        if ( nRadix < 2 || nRadix > 16 )
        {
            return SDCRYPTO_BNERR_BAD_INPUT_DATA;
        }

        BN_CHK( BNModInt( &r, X, nRadix ) );
        BN_CHK( BNDivInt( X, nullptr, X, nRadix ) );

        if ( BNCmpInt( X, 0 ) != 0 )
        {
            BN_CHK( BNWriteHlp( X, nRadix, p ) );
        }

        if ( r < 10 )
        {
            *(*p)++ = (char)( r + 0x30 );
        }
        else
        {
            *(*p)++ = (char)( r + 0x37 );
        }

cleanup:

        return nRet;
    }

    int32 BNWriteString( BigNum *X, int32 nRadix, char *s, int32 *sLen )
    {
        int nRet = 0, n;
        char *p;
        BigNum T;

        if ( nRadix < 2 || nRadix > 16 )
        {
            return SDCRYPTO_BNERR_BAD_INPUT_DATA;
        }

        n = BNGetMSB( X );
        if ( nRadix >=  4 )
        {
            n >>= 1;
        }
        if ( nRadix >= 16 )
        {
            n >>= 1;
        }
        n += 3;

        if ( *sLen < n )
        {
            *sLen = n;
            return SDCRYPTO_BNERR_BUFFER_TOO_SMALL;
        }

        p = s;
        BNInit( &T, nullptr );

        if ( X->s == -1 )
        {
            *p++ = '-';
        }

        if ( nRadix == 16 )
        {
            int32 c, i, j, k;

            for( i = X->n - 1, k = 0; i >= 0; i-- )
            {
                for( j = ciL - 1; j >= 0; j-- )
                {
                    c = ( X->p[i] >> (j << 3) ) & 0xFF;

                    if ( c == 0 && k == 0 && (i + j) != 0 )
                    {
                        continue;
                    }

                    p += sprintf( p, "%02X", c );
                    k = 1;
                }
            }
        }
        else
        {
            BN_CHK( BNCopy( &T, X ) );
            BN_CHK( BNWriteHlp( &T, nRadix, &p ) );
        }

        *p++ = '\0';
        *sLen = (int32)(p - s);

cleanup:

        BNFree( &T, nullptr );

        return( nRet );
    }

    int32 BNReadFile( BigNum *X, int32 nRadix, FILE *fpIn )
    {
        uint32 d;
        int32 slen;
        char  *p;
        char  s[1024];

        memset( s, 0, sizeof( s ) );
        if ( fgets( s, sizeof( s ) - 1, fpIn ) == nullptr )
        {
            return SDCRYPTO_BNERR_FILE_IO_ERROR;
        }

        slen = (int32)strlen( s );
        if ( s[slen - 1] == '\n' )
        {
            --slen;
            s[slen] = '\0';
        }

        if ( s[slen - 1] == '\r' )
        {
            --slen;
            s[slen] = '\0';
        }

        p = s + slen;
        while ( --p >= s )
        {
            if ( BNGetDigit( &d, nRadix, *p ) != 0 )
            {
                break;
            }
        }

        return( BNReadString( X, nRadix, p + 1 ) );
    }

    int32 BNWriteFile(const char *p, BigNum *X, int32 nRadix, FILE *fpOut )
    {
        int32  n, nRet;
        size_t slen;
        size_t plen;
        char   s[1024];

        n = sizeof( s );
        memset( s, 0, n );
        n -= 2;

        BN_CHK( BNWriteString( X, nRadix, s, (int *) &n ) );

        if ( p == nullptr )
        {
            p = "";
        }

        plen = strlen( p );
        slen = strlen( s );
        s[slen++] = '\r';
        s[slen++] = '\n';

        if ( fpOut != nullptr )
        {
            if ( fwrite( p, 1, plen, fpOut ) != plen ||
                    fwrite( s, 1, slen, fpOut ) != slen )
            {
                return SDCRYPTO_BNERR_FILE_IO_ERROR;
            }
        }
        else
        {
            printf( "%s%s", p, s );
        }

cleanup:

        return nRet;
    }

    int32 BNReadBinary( BigNum *X, byte *pBuf, int32 nBufLen )
    {
        int32 nRet, i, j, n;

        for ( n = 0; n < nBufLen; n++ )
        {
            if ( pBuf[n] != 0 )
            {
                break;
            }
        }

        BN_CHK( BNGrow( X, CHARS_TO_LIMBS( nBufLen - n ) ) );
        BN_CHK( BNSetValue( X, 0 ) );

        for ( i = nBufLen - 1, j = 0; i >= n; i--, j++ )
        {
            X->p[j / ciL] |= ((uint32) pBuf[i]) << ((j % ciL) << 3);
        }

cleanup:

        return( nRet );

    }

    int32 BNWriteBinary( BigNum *X, byte *pBuf, int32 nBufLen )
    {
        int32 i, j, n;

        n = BNGetTotalBytes( X );

        if ( nBufLen < n )
        {
            return SDCRYPTO_BNERR_BUFFER_TOO_SMALL;
        }

        memset( pBuf, 0, nBufLen );

        for ( i = nBufLen - 1, j = 0; n > 0; --i, ++j, --n)
        {
            pBuf[i] = (byte)( X->p[j / ciL] >> ((j % ciL) << 3) );
        }

        return 0;
    }

    int32 BNShiftLeft( BigNum *X, int32 nCount )
    {
        int32  nRet, i, v0, t1;
        uint32 r0 = 0, r1;

        v0 = nCount / (biL    );
        t1 = nCount & (biL - 1);

        i = BNGetMSB( X ) + nCount;

        if ( (int32)(X->n * biL) < i )
        {
            BN_CHK( BNGrow( X, BITS_TO_LIMBS( i ) ) );
        }

        nRet = 0;


        // shift by count / limb_size
        if ( v0 > 0 )
        {
            for ( i = X->n - 1; i >= v0; --i )
            {
                X->p[i] = X->p[i - v0];
            }

            for ( ; i >= 0; --i )
            {
                X->p[i] = 0;
            }
        }

        // shift by count % limb_size
        if ( t1 > 0 )
        {
            for ( i = v0; i < (int32)X->n; ++i )
            {
                r1 = X->p[i] >> (biL - t1);
                X->p[i] <<= t1;
                X->p[i] |= r0;
                r0 = r1;
            }
        }

cleanup:

        return nRet;
    }


    int32 BNShiftRight( BigNum *X, int32 nCount )
    {
        int32  i, v0, v1;
        uint32 r0 = 0, r1;

        v0 = nCount /  biL;
        v1 = nCount & (biL - 1);

        // shift by count / limb_size
        if ( v0 > 0 )
        {
            for ( i = 0; i < (int32)(X->n - v0); ++i )
            {
                X->p[i] = X->p[i + v0];
            }

            for ( ; i < (int32)X->n; ++i )
            {
                X->p[i] = 0;
            }
        }


        // shift by count % limb_size
        if ( v1 > 0 )
        {
            for ( i = X->n - 1; i >= 0; --i )
            {
                r1 = X->p[i] << (biL - v1);
                X->p[i] >>= v1;
                X->p[i] |= r0;
                r0 = r1;
            }
        }

        return 0;
    }


    int32 BNCmpAbs( BigNum *X, BigNum *Y )
    {
        int32 i, j;

        for ( i = X->n - 1; i >= 0; i-- )
        {
            if ( X->p[i] != 0 )
            {
                break;
            }
        }

        for ( j = Y->n - 1; j >= 0; j-- )
        {
            if ( Y->p[j] != 0 )
            {
                break;
            }
        }

        if ( i < 0 && j < 0 )
        {
            return 0;
        }

        if ( i > j )
        {
            return 1;
        }
        if ( j > i )
        {
            return -1;
        }

        for ( ; i >= 0; i-- )
        {
            if ( X->p[i] > Y->p[i] )
            {
                return 1;
            }
            if ( X->p[i] < Y->p[i] )
            {
                return -1;
            }
        }

        return 0;
    }

    int32 BNCmpMpi( BigNum *X, BigNum *Y )
    {
        int32 i, j;

        for ( i = X->n - 1; i >= 0; --i )
        {
            if ( X->p[i] != 0 )
            {
                break;
            }
        }

        for ( j = Y->n - 1; j >= 0; --j )
        {
            if ( Y->p[j] != 0 )
            {
                break;
            }
        }

        if ( i < 0 && j < 0 )
        {
            return 0;
        }

        if ( i > j )
        {
            return(  X->s );
        }
        if ( j > i )
        {
            return( -X->s );
        }

        if ( X->s > 0 && Y->s < 0 )
        {
            return  1;
        }
        if ( Y->s > 0 && X->s < 0 )
        {
            return -1;
        }

        for ( ; i >= 0; i-- )
        {
            if ( X->p[i] > Y->p[i] )
            {
                return(  X->s );
            }
            if ( X->p[i] < Y->p[i] )
            {
                return( -X->s );
            }
        }

        return 0;
    }


    int32 BNCmpInt( BigNum *X, int32 nZ )
    {
        BigNum Y;
        uint32 p[1];

        *p  = ( nZ < 0 ) ? -nZ : nZ;
        Y.s = ( nZ < 0 ) ? -1  : 1;
        Y.n = 1;
        Y.p = p;

        return( BNCmpMpi( X, &Y ) );
    }

    int32 BNAddAbs( BigNum *X, BigNum *A, BigNum *B )
    {
        int32  nRet, i, j;
        uint32 *o, *p, c;

        if ( X == B )
        {
            BigNum *T = A;
            A = X;
            B = T;
        }

        if( X != A )
        {
            BN_CHK( BNCopy( X, A ) );
        }

        for ( j = B->n - 1; j >= 0; j-- )
        {
            if ( B->p[j] != 0 )
            {
                break;
            }
        }

        BN_CHK( BNGrow( X, j + 1 ) );

        o = B->p;
        p = X->p;
        c = 0;

        for( i = 0; i <= j; i++, o++, p++ )
        {
            *p +=  c;
            c  = ( *p <  c );
            *p += *o;
            c += ( *p < *o );
        }

        while( c != 0 )
        {
            if( i >= (int32)X->n )
            {
                BN_CHK( BNGrow( X, i + 1 ) );
                p = X->p + i;
            }

            *p += c;
            c = ( *p < c );
            ++i;
        }

cleanup:

        return nRet;
    }

    void BNSubHlp( int32 n, uint32 *s, uint32 *d )
    {
        int32  i;
        uint32 c, z;

        for( i = c = 0; i < n; ++i, ++s, ++d )
        {
            z = ( *d <  c );
            *d -=  c;
            c = ( *d < *s ) + z;
            *d -= *s;
        }

        while( c != 0 )
        {
            z = ( *d < c );
            *d -= c;
            c = z;
            ++i;
            ++d;
        }
    }

    int32 BNSubAbs( BigNum *X, BigNum *A, BigNum *B )
    {
        BigNum TB;
        int32 nRet, n;

        if ( BNCmpAbs( A, B ) < 0 )
        {
            return SDCRYPTO_BNERR_NEGATIVE_VALUE;
        }

        BNInit( &TB, nullptr );

        if ( X == B )
        {
            BN_CHK( BNCopy( &TB, B ) );
            B = &TB;
        }

        if ( X != A )
        {
            BN_CHK( BNCopy( X, A ) );
        }

        nRet = 0;

        for ( n = B->n - 1; n >= 0; --n )
        {
            if ( B->p[n] != 0 )
            {
                break;
            }
        }

        BNSubHlp( n + 1, B->p, X->p );

cleanup:

        BNFree( &TB, nullptr );

        return nRet;
    }


    int32 BNAddMpi( BigNum *X, BigNum *A, BigNum *B )
    {
        int32 nRet, s = A->s;

        if( A->s * B->s < 0 )
        {
            if( BNCmpAbs( A, B ) >= 0 )
            {
                BN_CHK( BNSubAbs( X, A, B ) );
                X->s =  s;
            }
            else
            {
                BN_CHK( BNSubAbs( X, B, A ) );
                X->s = -s;
            }
        }
        else
        {
            BN_CHK( BNAddAbs( X, A, B ) );
            X->s = s;
        }

cleanup:

        return nRet;
    }


    int32 BNSubMpi( BigNum *X, BigNum *A, BigNum *B )
    {
        int32 nRet, s = A->s;

        if( A->s * B->s > 0 )
        {
            if( BNCmpAbs( A, B ) >= 0 )
            {
                BN_CHK( BNSubAbs( X, A, B ) );
                X->s =  s;
            }
            else
            {
                BN_CHK( BNSubAbs( X, B, A ) );
                X->s = -s;
            }
        }
        else
        {
            BN_CHK( BNAddAbs( X, A, B ) );
            X->s = s;
        }

cleanup:

        return nRet;
    }

    int32 BNAddInt( BigNum *X, BigNum *A, int32 b )
    {
        BigNum _B;
        uint32 p[1];

        p[0] = ( b < 0 ) ? -b : b;
        _B.s = ( b < 0 ) ? -1 : 1;
        _B.n = 1;
        _B.p = p;

        return( BNAddMpi( X, A, &_B ) );
    }


    int32 BNSubInt( BigNum *X, BigNum *A, int32 b )
    {
        BigNum _B;
        uint32 p[1];

        p[0] = ( b < 0 ) ? -b : b;
        _B.s = ( b < 0 ) ? -1 : 1;
        _B.n = 1;
        _B.p = p;

        return( BNSubMpi( X, A, &_B ) );
    }

    void BNMulHlp( uint32 i, uint32 *s, uint32 *d, uint32 b )
    {
        uint32 c = 0, t = 0;
#if 1

#if defined(MULADDC_HUIT)
        for( ; i >= 8; i -= 8 )
        {
            MULADDC_INIT
            MULADDC_HUIT
            MULADDC_STOP
        }

        for( ; i > 0; i-- )
        {
            MULADDC_INIT
            MULADDC_CORE
            MULADDC_STOP
        }
#else
        for( ; i >= 16; i -= 16 )
        {
            MULADDC_INIT
            MULADDC_CORE   MULADDC_CORE
            MULADDC_CORE   MULADDC_CORE
            MULADDC_CORE   MULADDC_CORE
            MULADDC_CORE   MULADDC_CORE

            MULADDC_CORE   MULADDC_CORE
            MULADDC_CORE   MULADDC_CORE
            MULADDC_CORE   MULADDC_CORE
            MULADDC_CORE   MULADDC_CORE
            MULADDC_STOP
        }

        for( ; i >= 8; i -= 8 )
        {
            MULADDC_INIT
            MULADDC_CORE   MULADDC_CORE
            MULADDC_CORE   MULADDC_CORE

            MULADDC_CORE   MULADDC_CORE
            MULADDC_CORE   MULADDC_CORE
            MULADDC_STOP
        }

        for( ; i > 0; i-- )
        {
            MULADDC_INIT
            MULADDC_CORE
            MULADDC_STOP
        }
#endif

#endif

        t++;

        do
        {
            *d += c;
            c = ( *d < c );
            d++;
        }
        while( c != 0 );
    }

    int32 BNMulMpi( BigNum *X, BigNum *A, BigNum *B )
    {
        int32  nRet, i, j;
        BigNum TA, TB;

        BNInit( &TA, &TB, nullptr );

        if( X == A )
        {
            BN_CHK( BNCopy( &TA, A ) );
            A = &TA;
        }

        if( X == B )
        {
            BN_CHK( BNCopy( &TB, B ) );
            B = &TB;
        }


        for( i = A->n - 1; i >= 0; i-- )
            if( A->p[i] != 0 )
                break;

        for( j = B->n - 1; j >= 0; j-- )
            if( B->p[j] != 0 )
                break;

        BN_CHK( BNGrow( X, i + j + 2 ) );
        BN_CHK( BNSetValue( X, 0 ) );

        for ( i++; j >= 0; j-- )
        {
            BNMulHlp( i, A->p, X->p + j, B->p[j] );
        }

        X->s = A->s * B->s;

cleanup:

        BNFree( &TB, &TA, nullptr );

        return nRet;
    }


    int32 BNMulInt( BigNum *X, BigNum *A, uint32 b )
    {
        BigNum _B;
        uint32 p[1];

        _B.s = 1;
        _B.n = 1;
        _B.p = p;
        p[0] = b;

        return( BNMulMpi( X, A, &_B ) );
    }


    int32 BNDivMpi( BigNum *Q, BigNum *R, BigNum *A, BigNum *B )
    {
        int32 nRet, i, n, t, k;
        BigNum X, Y, Z, T1, T2;

        if( BNCmpInt( B, 0 ) == 0 )
            return SDCRYPTO_BNERR_DIVISION_BY_ZERO;

        BNInit( &X, &Y, &Z, &T1, &T2, nullptr );

        if( BNCmpAbs( A, B ) < 0 )
        {
            if( Q != nullptr ) BN_CHK( BNSetValue( Q, 0 ) );
            if( R != nullptr ) BN_CHK( BNCopy( R, A ) );
            return( 0 );
        }

        BN_CHK( BNCopy( &X, A ) );
        BN_CHK( BNCopy( &Y, B ) );
        X.s = Y.s = 1;

        BN_CHK( BNGrow( &Z, A->n + 2 ) );
        BN_CHK( BNSetValue( &Z,  0 ) );
        BN_CHK( BNGrow( &T1, 2 ) );
        BN_CHK( BNGrow( &T2, 3 ) );

        k = BNGetMSB( &Y ) % biL;
        if( k < (int32) biL - 1 )
        {
            k = biL - 1 - k;
            BN_CHK( BNShiftLeft( &X, k ) );
            BN_CHK( BNShiftLeft( &Y, k ) );
        }
        else k = 0;

        n = X.n - 1;
        t = Y.n - 1;
        BNShiftLeft( &Y, biL * (n - t) );

        while( BNCmpMpi( &X, &Y ) >= 0 )
        {
            Z.p[n - t]++;
            BNSubMpi( &X, &X, &Y );
        }
        BNShiftRight( &Y, biL * (n - t) );

        for( i = n; i > t ; i-- )
        {
            if( X.p[i] >= Y.p[t] )
                Z.p[i - t - 1] = ~0;
            else
            {
#if defined(SDCRYPTO_HAVE_LONGLONG)
                uint64 r;

                r  = (uint64) X.p[i] << biL;
                r |= (uint64) X.p[i - 1];
                r /= Y.p[t];
                if( r > ((uint64) 1 << biL) - 1)
                    r = ((uint64) 1 << biL) - 1;

                Z.p[i - t - 1] = (uint32) r;
#else
                /*
                * __udiv_qrnnd_c, from gmp/longlong.h
                */
                uint32 q0, q1, r0, r1;
                uint32 d0, d1, d, m;

                d  = Y.p[t];
                d0 = ( d << biH ) >> biH;
                d1 = ( d >> biH );

                q1 = X.p[i] / d1;
                r1 = X.p[i] - d1 * q1;
                r1 <<= biH;
                r1 |= ( X.p[i - 1] >> biH );

                m = q1 * d0;
                if( r1 < m )
                {
                    q1--, r1 += d;
                    while( r1 >= d && r1 < m )
                        q1--, r1 += d;
                }
                r1 -= m;

                q0 = r1 / d1;
                r0 = r1 - d1 * q0;
                r0 <<= biH;
                r0 |= ( X.p[i - 1] << biH ) >> biH;

                m = q0 * d0;
                if( r0 < m )
                {
                    q0--, r0 += d;
                    while( r0 >= d && r0 < m )
                        q0--, r0 += d;
                }
                r0 -= m;

                Z.p[i - t - 1] = ( q1 << biH ) | q0;
#endif
            }

            Z.p[i - t - 1]++;
            do
            {
                Z.p[i - t - 1]--;

                BN_CHK( BNSetValue( &T1, 0 ) );
                T1.p[0] = (t < 1) ? 0 : Y.p[t - 1];
                T1.p[1] = Y.p[t];
                BN_CHK( BNMulInt( &T1, &T1, Z.p[i - t - 1] ) );

                BN_CHK( BNSetValue( &T2, 0 ) );
                T2.p[0] = (i < 2) ? 0 : X.p[i - 2];
                T2.p[1] = (i < 1) ? 0 : X.p[i - 1];
                T2.p[2] = X.p[i];
            }
            while( BNCmpMpi( &T1, &T2 ) > 0 );

            BN_CHK( BNMulInt( &T1, &Y, Z.p[i - t - 1] ) );
            BN_CHK( BNShiftLeft( &T1,  biL * (i - t - 1) ) );
            BN_CHK( BNSubMpi( &X, &X, &T1 ) );

            if( BNCmpInt( &X, 0 ) < 0 )
            {
                BN_CHK( BNCopy( &T1, &Y ) );
                BN_CHK( BNShiftLeft( &T1, biL * (i - t - 1) ) );
                BN_CHK( BNAddMpi( &X, &X, &T1 ) );
                Z.p[i - t - 1]--;
            }
        }

        if( Q != nullptr )
        {
            BNCopy( Q, &Z );
            Q->s = A->s * B->s;
        }

        if( R != nullptr )
        {
            BNShiftRight( &X, k );
            BNCopy( R, &X );

            R->s = A->s;
            if( BNCmpInt( R, 0 ) == 0 )
                R->s = 1;
        }

cleanup:

        BNFree( &X, &Y, &Z, &T1, &T2, nullptr );

        return nRet;
    }


    int32 BNDivInt( BigNum *Q, BigNum *R, BigNum *A, int32 b )
    {
        BigNum _B;
        uint32 p[1];

        p[0] = ( b < 0 ) ? -b : b;
        _B.s = ( b < 0 ) ? -1 : 1;
        _B.n = 1;
        _B.p = p;

        return( BNDivMpi( Q, R, A, &_B ) );
    }


    int32 BNModMpi( BigNum *R, BigNum *A, BigNum *B )
    {
        int32 nRet;

        BN_CHK( BNDivMpi( nullptr, R, A, B ) );

        while( BNCmpInt( R, 0 ) < 0 )
            BN_CHK( BNAddMpi( R, R, B ) );

        while( BNCmpMpi( R, B ) >= 0 )
            BN_CHK( BNSubMpi( R, R, B ) );

cleanup:

        return( nRet );
    }


    int32 BNModInt( uint32 *r, BigNum *A, int32 b )
    {
        int32 i;
        uint32 x, y, z;

        if( b == 0 )
            return( SDCRYPTO_BNERR_DIVISION_BY_ZERO );

        if( b < 0 )
            b = -b;

        /*
        * handle trivial cases
        */
        if( b == 1 )
        {
            *r = 0;
            return( 0 );
        }

        if( b == 2 )
        {
            *r = A->p[0] & 1;
            return( 0 );
        }

        /*
        * general case
        */
        for( i = A->n - 1, y = 0; i >= 0; i-- )
        {
            x  = A->p[i];
            y  = ( y << biH ) | ( x >> biH );
            z  = y / b;
            y -= z * b;

            x <<= biH;
            y  = ( y << biH ) | ( x >> biH );
            z  = y / b;
            y -= z * b;
        }

        *r = y;

        return( 0 );
    }



    void BNMontgInit( uint32 *mm, BigNum *N )
    {
        uint32 x, m0 = N->p[0];

        x  = m0;
        x += ( ( m0 + 2 ) & 4 ) << 1;
        x *= ( 2 - ( m0 * x ) );

        if ( biL >= 16 )
        {
            x *= ( 2 - ( m0 * x ) );
        }

        if ( biL >= 32 )
        {
            x *= ( 2 - ( m0 * x ) );
        }

        if ( biL >= 64 )
        {
            x *= ( 2 - ( m0 * x ) );
        }

        *mm = ~x + 1;
    }

    void BNMontMul( BigNum *A, BigNum *B, BigNum *N, uint32 mm, BigNum *T )
    {
	uint32 i, n, m;
	uint32 u0, u1, *d;

	memset( T->p, 0, T->n * ciL );

	d = T->p;
	n = N->n;
	m = ( B->n < n ) ? B->n : n;

        for ( i = 0; i < n; i++ )
        {
            /*
            * T = (T + u0*B + u1*N) / 2^biL
            */
            u0 = A->p[i];
            u1 = ( d[0] + u0 * B->p[0] ) * mm;

            BNMulHlp( m, B->p, d, u0 );
            BNMulHlp( n, N->p, d, u1 );

            *d++ = u0;
            d[n + 1] = 0;
        }

        memcpy( A->p, d, (n + 1) * ciL );

        if ( BNCmpAbs( A, N ) >= 0 )
        {
            BNSubHlp( n, N->p, A->p );
        }
        else
        {
            /* prevent timing attacks */
            BNSubHlp( n, A->p, T->p );
        }
    }

    void BNMontRed( BigNum *A, BigNum *N, uint32 mm, BigNum *T )
    {
        uint32 z = 1;
        BigNum U;

        U.n = U.s = z;
        U.p = &z;

        BNMontMul( A, &U, N, mm, T );
    }


    int32 BNExpMod( BigNum *X, BigNum *A, BigNum *E, BigNum *N, BigNum *_RR )
    {
        int32  nRet, i, j, wsize, wbits;
        int32  bufsize, nblimbs, nBits;
        uint32 ei, mm, state;
        BigNum RR, T, W[64];

        if( BNCmpInt( N, 0 ) < 0 || ( N->p[0] & 1 ) == 0 )
        {
            return( SDCRYPTO_BNERR_BAD_INPUT_DATA );
        }

        /*
        * Init temps and window size
        */
        BNMontgInit( &mm, N );
        BNInit( &RR, &T, nullptr );
        memset( W, 0, sizeof( W ) );

        i = BNGetMSB( E );

        wsize = ( i > 671 ) ? 6 : ( i > 239 ) ? 5 :
                ( i >  79 ) ? 4 : ( i >  23 ) ? 3 : 1;

        j = N->n + 1;
        BN_CHK( BNGrow( X, j ) );
        BN_CHK( BNGrow( &W[1],  j ) );
        BN_CHK( BNGrow( &T, j * 2 ) );

        /*
        * If 1st call, pre-compute R^2 mod N
        */
        if( _RR == nullptr || _RR->p == nullptr )
        {
            BN_CHK( BNSetValue( &RR, 1 ) );
            BN_CHK( BNShiftLeft( &RR, N->n * 2 * biL ) );
            BN_CHK( BNModMpi( &RR, &RR, N ) );

            if( _RR != nullptr )
                memcpy( _RR, &RR, sizeof( BigNum ) );
        }
        else
            memcpy( &RR, _RR, sizeof( BigNum ) );

        /*
        * W[1] = A * R^2 * R^-1 mod N = A * R mod N
        */
        if( BNCmpMpi( A, N ) >= 0 )
            BNModMpi( &W[1], A, N );
        else
            BNCopy( &W[1], A );

        BNMontMul( &W[1], &RR, N, mm, &T );

        /*
        * X = R^2 * R^-1 mod N = R mod N
        */
        BN_CHK( BNCopy( X, &RR ) );
        BNMontRed( X, N, mm, &T );

        if( wsize > 1 )
        {
            /*
            * W[1 << (wsize - 1)] = W[1] ^ (wsize - 1)
            */
            j =  1 << (wsize - 1);

            BN_CHK( BNGrow( &W[j], N->n + 1 ) );
            BN_CHK( BNCopy( &W[j], &W[1]    ) );

            for( i = 0; i < wsize - 1; i++ )
                BNMontMul( &W[j], &W[j], N, mm, &T );

            /*
            * W[i] = W[i - 1] * W[1]
            */
            for( i = j + 1; i < (1 << wsize); i++ )
            {
                BN_CHK( BNGrow( &W[i], N->n + 1 ) );
                BN_CHK( BNCopy( &W[i], &W[i - 1] ) );

                BNMontMul( &W[i], &W[1], N, mm, &T );
            }
        }

        nblimbs = E->n;
        bufsize = 0;
        nBits   = 0;
        wbits   = 0;
        state   = 0;

        while( 1 )
        {
            if( bufsize == 0 )
            {
                if( nblimbs-- == 0 )
                    break;

                bufsize = sizeof( uint32 ) << 3;
            }

            bufsize--;

            ei = (E->p[nblimbs] >> bufsize) & 1;

            /*
            * skip leading 0s
            */
            if( ei == 0 && state == 0 )
                continue;

            if( ei == 0 && state == 1 )
            {
                /*
                * out of window, square X
                */
                BNMontMul( X, X, N, mm, &T );
                continue;
            }

            /*
            * add ei to current window
            */
            state = 2;

            nBits++;
            wbits |= (ei << (wsize - nBits));

            if( nBits == wsize )
            {
                /*
                * X = X^wsize R^-1 mod N
                */
                for( i = 0; i < wsize; i++ )
                    BNMontMul( X, X, N, mm, &T );

                /*
                * X = X * W[wbits] R^-1 mod N
                */
                BNMontMul( X, &W[wbits], N, mm, &T );

                state--;
                nBits = 0;
                wbits = 0;
            }
        }

        /*
        * process the remaining bits
        */
        for( i = 0; i < nBits; i++ )
        {
            BNMontMul( X, X, N, mm, &T );

            wbits <<= 1;

            if( (wbits & (1 << wsize)) != 0 )
                BNMontMul( X, &W[1], N, mm, &T );
        }

        /*
        * X = A^E * R * R^-1 mod N = A^E mod N
        */
        BNMontRed( X, N, mm, &T );

cleanup:

        for( i = (1 << (wsize - 1)); i < (1 << wsize); i++ )
            BNFree( &W[i], nullptr );

        if( _RR != nullptr )
            BNFree( &W[1], &T, nullptr );
        else BNFree( &W[1], &T, &RR, nullptr );

        return( nRet );
    }


    int32 BNGcd( BigNum *G, BigNum *A, BigNum *B )
    {
        int32 nRet;
        BigNum TG, TA, TB;

        BNInit( &TG, &TA, &TB, nullptr );

        BN_CHK( BNSetValue( &TG, 1 ) );
        BN_CHK( BNCopy( &TA, A ) );
        BN_CHK( BNCopy( &TB, B ) );

        TA.s = TB.s = 1;

        while( BNCmpInt( &TA, 0 ) != 0 )
        {
            while( ( TA.p[0] & 1 ) == 0 ) BN_CHK( BNShiftRight( &TA, 1 ) );
            while( ( TB.p[0] & 1 ) == 0 ) BN_CHK( BNShiftRight( &TB, 1 ) );

            if( BNCmpMpi( &TA, &TB ) >= 0 )
            {
                BN_CHK( BNSubAbs( &TA, &TA, &TB ) );
                BN_CHK( BNShiftRight( &TA, 1 ) );
            }
            else
            {
                BN_CHK( BNSubAbs( &TB, &TB, &TA ) );
                BN_CHK( BNShiftRight( &TB, 1 ) );
            }
        }

        BN_CHK( BNMulMpi( G, &TG, &TB ) );

cleanup:

        BNFree( &TB, &TA, &TG, nullptr );

        return( nRet );
    }


    int32 BNInvMod( BigNum *X, BigNum *A, BigNum *N )
    {
        int32 nRet;
        BigNum G, TA, TU, U1, U2, TB, TV, V1, V2;

        if( BNCmpInt( N, 0 ) <= 0 )
            return( SDCRYPTO_BNERR_BAD_INPUT_DATA );

        BNInit( &TA, &TU, &U1, &U2, &G,
                &TB, &TV, &V1, &V2, nullptr );

        BN_CHK( BNGcd( &G, A, N ) );

        if( BNCmpInt( &G, 1 ) != 0 )
        {
            nRet = SDCRYPTO_BNERR_NOT_ACCEPTABLE;
            goto cleanup;
        }

        BN_CHK( BNModMpi( &TA, A, N ) );
        BN_CHK( BNCopy( &TU, &TA ) );
        BN_CHK( BNCopy( &TB, N ) );
        BN_CHK( BNCopy( &TV, N ) );

        BN_CHK( BNSetValue( &U1, 1 ) );
        BN_CHK( BNSetValue( &U2, 0 ) );
        BN_CHK( BNSetValue( &V1, 0 ) );
        BN_CHK( BNSetValue( &V2, 1 ) );

        do
        {
            while( ( TU.p[0] & 1 ) == 0 )
            {
                BN_CHK( BNShiftRight( &TU, 1 ) );

                if( ( U1.p[0] & 1 ) != 0 || ( U2.p[0] & 1 ) != 0 )
                {
                    BN_CHK( BNAddMpi( &U1, &U1, &TB ) );
                    BN_CHK( BNSubMpi( &U2, &U2, &TA ) );
                }

                BN_CHK( BNShiftRight( &U1, 1 ) );
                BN_CHK( BNShiftRight( &U2, 1 ) );
            }

            while( ( TV.p[0] & 1 ) == 0 )
            {
                BN_CHK( BNShiftRight( &TV, 1 ) );

                if( ( V1.p[0] & 1 ) != 0 || ( V2.p[0] & 1 ) != 0 )
                {
                    BN_CHK( BNAddMpi( &V1, &V1, &TB ) );
                    BN_CHK( BNSubMpi( &V2, &V2, &TA ) );
                }

                BN_CHK( BNShiftRight( &V1, 1 ) );
                BN_CHK( BNShiftRight( &V2, 1 ) );
            }

            if( BNCmpMpi( &TU, &TV ) >= 0 )
            {
                BN_CHK( BNSubMpi( &TU, &TU, &TV ) );
                BN_CHK( BNSubMpi( &U1, &U1, &V1 ) );
                BN_CHK( BNSubMpi( &U2, &U2, &V2 ) );
            }
            else
            {
                BN_CHK( BNSubMpi( &TV, &TV, &TU ) );
                BN_CHK( BNSubMpi( &V1, &V1, &U1 ) );
                BN_CHK( BNSubMpi( &V2, &V2, &U2 ) );
            }
        }
        while( BNCmpInt( &TU, 0 ) != 0 );

        while( BNCmpInt( &V1, 0 ) < 0 )
            BN_CHK( BNAddMpi( &V1, &V1, N ) );

        while( BNCmpMpi( &V1, N ) >= 0 )
            BN_CHK( BNSubMpi( &V1, &V1, N ) );

        BN_CHK( BNCopy( X, &V1 ) );

cleanup:

        BNFree( &V2, &V1, &TV, &TB, &G,
                &U2, &U1, &TU, &TA, nullptr );

        return( nRet );
    }

    static const int32 small_prime[] =
    {
        3,    5,    7,   11,   13,   17,   19,   23,
        29,   31,   37,   41,   43,   47,   53,   59,
        61,   67,   71,   73,   79,   83,   89,   97,
        101,  103,  107,  109,  113,  127,  131,  137,
        139,  149,  151,  157,  163,  167,  173,  179,
        181,  191,  193,  197,  199,  211,  223,  227,
        229,  233,  239,  241,  251,  257,  263,  269,
        271,  277,  281,  283,  293,  307,  311,  313,
        317,  331,  337,  347,  349,  353,  359,  367,
        373,  379,  383,  389,  397,  401,  409,  419,
        421,  431,  433,  439,  443,  449,  457,  461,
        463,  467,  479,  487,  491,  499,  503,  509,
        521,  523,  541,  547,  557,  563,  569,  571,
        577,  587,  593,  599,  601,  607,  613,  617,
        619,  631,  641,  643,  647,  653,  659,  661,
        673,  677,  683,  691,  701,  709,  719,  727,
        733,  739,  743,  751,  757,  761,  769,  773,
        787,  797,  809,  811,  821,  823,  827,  829,
        839,  853,  857,  859,  863,  877,  881,  883,
        887,  907,  911,  919,  929,  937,  941,  947,
        953,  967,  971,  977,  983,  991,  997, -103
    };

    int32 BNIsPrime( BigNum *X, int32 (*fpRng)(void *), void *pRng )
    {
        int32  nRet, i, j, n, s, xs;
        BigNum W, R, T, A, RR;
        byte *p;

        if( BNCmpInt( X, 0 ) == 0 )
            return( 0 );

        BNInit( &W, &R, &T, &A, &RR, nullptr );

        xs = X->s;
        X->s = 1;

        /*
        * test trivial factors first
        */
        if( ( X->p[0] & 1 ) == 0 )
            return( SDCRYPTO_BNERR_NOT_ACCEPTABLE );

        for( i = 0; small_prime[i] > 0; i++ )
        {
            uint32 r;

            if( BNCmpInt( X, small_prime[i] ) <= 0 )
                return( 0 );

            BN_CHK( BNModInt( &r, X, small_prime[i] ) );

            if( r == 0 )
                return( SDCRYPTO_BNERR_NOT_ACCEPTABLE );
        }

        /*
        * W = |X| - 1
        * R = W >> lsb( W )
        */
        s = BNGetLSB( &W );
        BN_CHK( BNSubInt( &W, X, 1 ) );
        BN_CHK( BNCopy( &R, &W ) );
        BN_CHK( BNShiftRight( &R, s ) );

        i = BNGetMSB( X );
        /*
        * HAC, table 4.4
        */
        n = ( ( i >= 1300 ) ?  2 : ( i >=  850 ) ?  3 :
              ( i >=  650 ) ?  4 : ( i >=  350 ) ?  8 :
              ( i >=  250 ) ? 12 : ( i >=  150 ) ? 18 : 27 );

        for ( i = 0; i < n; i++ )
        {
            /*
            * pick a random A, 1 < A < |X| - 1
            */
            BN_CHK( BNGrow( &A, X->n ) );

            p = (byte *) A.p;
            for ( j = 0; j < (int32)(A.n * ciL); j++ )
            {
                *p++ = (byte) fpRng( pRng );
            }

            j = BNGetMSB( &A ) - BNGetMSB( &W );
            BN_CHK( BNShiftRight( &A, j + 1 ) );
            A.p[0] |= 3;

            /*
            * A = A^R mod |X|
            */
            BN_CHK( BNExpMod( &A, &A, &R, X, &RR ) );

            if ( BNCmpMpi( &A, &W ) == 0 ||
                    BNCmpInt( &A,  1 ) == 0 )
                continue;

            j = 1;
            while ( j < s && BNCmpMpi( &A, &W ) != 0 )
            {
                /*
                * A = A * A mod |X|
                */
                BN_CHK( BNMulMpi( &T, &A, &A ) );
                BN_CHK( BNModMpi( &A, &T, X  ) );

                if ( BNCmpInt( &A, 1 ) == 0 )
                    break;

                j++;
            }

            /*
            * not prime if A != |X| - 1 or A == 1
            */
            if ( BNCmpMpi( &A, &W ) != 0 ||
                    BNCmpInt( &A,  1 ) == 0 )
            {
                nRet = SDCRYPTO_BNERR_NOT_ACCEPTABLE;
                break;
            }
        }

cleanup:

        X->s = xs;

        BNFree( &RR, &A, &T, &R, &W, nullptr );

        return( nRet );
    }



    int32 BNGenPrime( BigNum *X, int32 nBits, int32 nDhFlag,
                      int32 (*fpRng)(void *), void *pRng )
    {
        int32 nRet, k, n;
        byte *p;
        BigNum Y;

        if ( nBits < 3 )
            return( SDCRYPTO_BNERR_BAD_INPUT_DATA );

        BNInit( &Y, nullptr );

        n = BITS_TO_LIMBS( nBits );

        BN_CHK( BNGrow( X, n ) );
        BN_CHK( BNSetValue( X, 0 ) );

        p = (byte *) X->p;
        for( k = 0; k < (int32)(X->n * ciL); k++ )
            *p++ = (byte) fpRng( pRng );

        k = BNGetMSB( X );
        if ( k < nBits ) BN_CHK( BNShiftLeft( X, nBits - k ) );
        if ( k > nBits ) BN_CHK( BNShiftRight( X, k - nBits ) );

        X->p[0] |= 3;

        if ( nDhFlag == 0 )
        {
            while ( ( nRet = BNIsPrime( X, fpRng, pRng ) ) != 0 )
            {
                if ( nRet != SDCRYPTO_BNERR_NOT_ACCEPTABLE )
                    goto cleanup;

                BN_CHK( BNAddInt( X, X, 2 ) );
            }
        }
        else
        {
            BN_CHK( BNSubInt( &Y, X, 1 ) );
            BN_CHK( BNShiftRight( &Y, 1 ) );

            while ( 1 )
            {
                if ( ( nRet = BNIsPrime( X, fpRng, pRng ) ) == 0 )
                {
                    if ( ( nRet = BNIsPrime( &Y, fpRng, pRng ) ) == 0 )
                        break;

                    if ( nRet != SDCRYPTO_BNERR_NOT_ACCEPTABLE )
                        goto cleanup;
                }

                if ( nRet != SDCRYPTO_BNERR_NOT_ACCEPTABLE )
                    goto cleanup;

                BN_CHK( BNAddInt( &Y, X, 1 ) );
                BN_CHK( BNAddInt(  X, X, 2 ) );
                BN_CHK( BNShiftRight( &Y, 1 ) );
            }
        }

cleanup:

        BNFree( &Y, nullptr );

        return( nRet );
    }


    int32 BNSelfTest( int32 nVerbose )
    {
        int32  nRet;
        BigNum A, E, N, X, Y, U, V;

        BNInit( &A, &E, &N, &X, &Y, &U, &V, nullptr );

        BN_CHK( BNReadString( &A, 16,
                              "EFE021C2645FD1DC586E69184AF4A31E" \
                              "D5F53E93B5F123FA41680867BA110131" \
                              "944FE7952E2517337780CB0DB80E61AA" \
                              "E7C8DDC6C5C6AADEB34EB38A2F40D5E6" ) );

        BN_CHK( BNReadString( &E, 16,
                              "B2E7EFD37075B9F03FF989C7C5051C20" \
                              "34D2A323810251127E7BF8625A4F49A5" \
                              "F3E27F4DA8BD59C47D6DAABA4C8127BD" \
                              "5B5C25763222FEFCCFC38B832366C29E" ) );

        BN_CHK( BNReadString( &N, 16,
                              "0066A198186C18C10B2F5ED9B522752A" \
                              "9830B69916E535C8F047518A889A43A5" \
                              "94B6BED27A168D31D4A52F88925AA8F5" ) );

        BN_CHK( BNMulMpi( &X, &A, &N ) );

        BN_CHK( BNReadString( &U, 16,
                              "602AB7ECA597A3D6B56FF9829A5E8B85" \
                              "9E857EA95A03512E2BAE7391688D264A" \
                              "A5663B0341DB9CCFD2C4C5F421FEC814" \
                              "8001B72E848A38CAE1C65F78E56ABDEF" \
                              "E12D3C039B8A02D6BE593F0BBBDA56F1" \
                              "ECF677152EF804370C1A305CAF3B5BF1" \
                              "30879B56C61DE584A0F53A2447A51E" ) );

        if ( nVerbose != 0 )
            printf( "  MPI test #1 (mul_mpi): " );

        if ( BNCmpMpi( &X, &U ) != 0 )
        {
            if( nVerbose != 0 )
                printf( "failed\n" );

            return( 1 );
        }

        if ( nVerbose != 0 )
            printf( "passed\n" );

        BN_CHK( BNDivMpi( &X, &Y, &A, &N ) );

        BN_CHK( BNReadString( &U, 16,
                              "256567336059E52CAE22925474705F39A94" ) );

        BN_CHK( BNReadString( &V, 16,
                              "6613F26162223DF488E9CD48CC132C7A" \
                              "0AC93C701B001B092E4E5B9F73BCD27B" \
                              "9EE50D0657C77F374E903CDFA4C642" ) );

        if ( nVerbose != 0 )
            printf( "  MPI test #2 (div_mpi): " );

        if ( BNCmpMpi( &X, &U ) != 0 ||
                BNCmpMpi( &Y, &V ) != 0 )
        {
            if ( nVerbose != 0 )
                printf( "failed\n" );

            return( 1 );
        }

        if ( nVerbose != 0 )
            printf( "passed\n" );

        BN_CHK( BNExpMod( &X, &A, &E, &N, nullptr ) );

        BN_CHK( BNReadString( &U, 16,
                              "36E139AEA55215609D2816998ED020BB" \
                              "BD96C37890F65171D948E9BC7CBAA4D9" \
                              "325D24D6A3C12710F10A09FA08AB87" ) );

        if ( nVerbose != 0 )
            printf( "  MPI test #3 (exp_mod): " );

        if ( BNCmpMpi( &X, &U ) != 0 )
        {
            if ( nVerbose != 0 )
                printf( "failed\n" );

            return( 1 );
        }

        if ( nVerbose != 0 )
            printf( "passed\n" );

        BN_CHK( BNInvMod( &X, &A, &N ) );

        BN_CHK( BNReadString( &U, 16,
                              "003A0AAEDD7E784FC07D8F9EC6E3BFD5" \
                              "C3DBA76456363A10869622EAC2DD84EC" \
                              "C5B8A74DAC4D09E03B5E0BE779F2DF61" ) );

        if ( nVerbose != 0 )
            printf( "  MPI test #4 (inv_mod): " );

        if ( BNCmpMpi( &X, &U ) != 0 )
        {
            if( nVerbose != 0 )
                printf( "failed\n" );

            return( 1 );
        }

        if ( nVerbose != 0 )
            printf( "passed\n" );

cleanup:

        if ( nRet != 0 && nVerbose != 0 )
            printf( "Unexpected error, return code = %08X\n", nRet );

        BNFree( &V, &U, &Y, &X, &N, &E, &A, nullptr );

        if ( nVerbose != 0 )
            printf( "\n" );

        return( nRet );
    }

}

#endif 

