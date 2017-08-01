#include "SDuPCH.h"
#include "sdcryptorsa.h"
#ifndef WIN64 

namespace Crown
{

    void RSAInit( SRSAContext *ctx,
                  int32 padding,
                  int32 hash_id,
                  int32 (*f_rng)(void *),
                  void *p_rng )
    {
        memset( ctx, 0, sizeof( SRSAContext ) );

        ctx->padding = padding;
        ctx->hash_id = hash_id;

        ctx->f_rng = f_rng;
        ctx->p_rng = p_rng;
    }

    int32 RSAGenKey( SRSAContext *ctx, int32 nbits, int32 exponent )
    {
        int32 nRet;
        BigNum P1, Q1, H, G;

        if ( ctx->f_rng == nullptr || nbits < 128 || exponent < 3 )
        {
            return SDCRYPTO_ERR_RSA_BAD_INPUT_DATA;
        }

        BNInit( &P1, &Q1, &H, &G, nullptr );

        /*
        * find primes P and Q with Q < P so that:
        * GCD( E, (P-1)*(Q-1) ) == 1
        */
        BN_CHK( BNSetValue( &ctx->E, exponent ) );

        nbits >>= 1;

        do
        {
            BN_CHK( BNGenPrime( &ctx->P, nbits, 0,
                                ctx->f_rng, ctx->p_rng ) );

            BN_CHK( BNGenPrime( &ctx->Q, nbits, 0,
                                ctx->f_rng, ctx->p_rng ) );

            if ( BNCmpMpi( &ctx->P, &ctx->Q ) < 0 )
                BNSwap( &ctx->P, &ctx->Q );

            if ( BNCmpMpi( &ctx->P, &ctx->Q ) == 0 )
                continue;

            BN_CHK( BNMulMpi( &ctx->N, &ctx->P, &ctx->Q ) );
            BN_CHK( BNSubInt( &P1, &ctx->P, 1 ) );
            BN_CHK( BNSubInt( &Q1, &ctx->Q, 1 ) );
            BN_CHK( BNMulMpi( &H, &P1, &Q1 ) );
            BN_CHK( BNGcd( &G, &ctx->E, &H  ) );
        }
        while ( BNCmpInt( &G, 1 ) != 0 );

        /*
        * D  = E^-1 mod ((P-1)*(Q-1))
        * DP = D mod (P - 1)
        * DQ = D mod (Q - 1)
        * QP = Q^-1 mod P
        */
        BN_CHK( BNInvMod( &ctx->D , &ctx->E, &H  ) );
        BN_CHK( BNModMpi( &ctx->DP, &ctx->D, &P1 ) );
        BN_CHK( BNModMpi( &ctx->DQ, &ctx->D, &Q1 ) );
        BN_CHK( BNInvMod( &ctx->QP, &ctx->Q, &ctx->P ) );

        ctx->len = ( BNGetMSB( &ctx->N ) + 7 ) >> 3;

cleanup:

        BNFree( &G, &H, &Q1, &P1, nullptr );

        if ( nRet != 0 )
        {
            RSAFree( ctx );
            return( SDCRYPTO_ERR_RSA_KEY_GEN_FAILED | nRet );
        }

        return 0;
    }

    int32 RSACheckPubKey( SRSAContext *ctx )
    {
        if ( ( ctx->N.p[0] & 1 ) == 0 ||
                ( ctx->E.p[0] & 1 ) == 0 )
        {
            return SDCRYPTO_ERR_RSA_KEY_CHECK_FAILED;
        }

        if ( BNGetMSB( &ctx->N ) < 128 ||
                BNGetMSB( &ctx->N ) > 4096 )
        {
            return SDCRYPTO_ERR_RSA_KEY_CHECK_FAILED;
        }

        if ( BNGetMSB( &ctx->E ) < 2 ||
                BNGetMSB( &ctx->E ) > 64 )
        {
            return SDCRYPTO_ERR_RSA_KEY_CHECK_FAILED;
        }

        return 0;
    }

    int32 RSACheckPrivKey( SRSAContext *ctx )
    {
        int32  nRet;
        BigNum PQ, DE, P1, Q1, H, I, G;

        if ( ( nRet = RSACheckPubKey( ctx ) ) != 0 )
        {
            return nRet;
        }

        BNInit( &PQ, &DE, &P1, &Q1, &H, &I, &G, nullptr );

        BN_CHK( BNMulMpi( &PQ, &ctx->P, &ctx->Q ) );
        BN_CHK( BNMulMpi( &DE, &ctx->D, &ctx->E ) );
        BN_CHK( BNSubInt( &P1, &ctx->P, 1 ) );
        BN_CHK( BNSubInt( &Q1, &ctx->Q, 1 ) );
        BN_CHK( BNMulMpi( &H, &P1, &Q1 ) );
        BN_CHK( BNModMpi( &I, &DE, &H  ) );
        BN_CHK( BNGcd( &G, &ctx->E, &H  ) );

        if( BNCmpMpi( &PQ, &ctx->N ) == 0 &&
                BNCmpInt( &I, 1 ) == 0 &&
                BNCmpInt( &G, 1 ) == 0 )
        {
            BNFree( &G, &I, &H, &Q1, &P1, &DE, &PQ, nullptr );
            return 0;
        }

cleanup:

        BNFree( &G, &I, &H, &Q1, &P1, &DE, &PQ, nullptr );
        return( SDCRYPTO_ERR_RSA_KEY_CHECK_FAILED | nRet );
    }

    int32 RSAPublic( SRSAContext *ctx,
                     unsigned char *input,
                     unsigned char *output )
    {
        int32  nRet, olen;
        BigNum T;

        BNInit( &T, nullptr );

        BN_CHK( BNReadBinary( &T, input, ctx->len ) );

        if ( BNCmpMpi( &T, &ctx->N ) >= 0 )
        {
            BNFree( &T, nullptr );
            return SDCRYPTO_ERR_RSA_BAD_INPUT_DATA;
        }

        olen = ctx->len;
        BN_CHK( BNExpMod( &T, &T, &ctx->E, &ctx->N, &ctx->RN ) );
        BN_CHK( BNWriteBinary( &T, output, olen ) );

cleanup:

        BNFree( &T, nullptr );

        if ( nRet != 0 )
        {
            return( SDCRYPTO_ERR_RSA_PUBLIC_FAILED | nRet );
        }

        return 0;
    }

    int32 RSAPrivate( SRSAContext *ctx,
                      unsigned char *input,
                      unsigned char *output )
    {
        int32  nRet, olen;
        BigNum T, T1, T2;

        BNInit( &T, &T1, &T2, nullptr );

        BN_CHK( BNReadBinary( &T, input, ctx->len ) );

        if ( BNCmpMpi( &T, &ctx->N ) >= 0 )
        {
            BNFree( &T, nullptr );
            return SDCRYPTO_ERR_RSA_BAD_INPUT_DATA;
        }

#if 0
        BN_CHK( BNExpMod( &T, &T, &ctx->D, &ctx->N, &ctx->RN ) );
#else
        //
        // faster decryption using the CRT
        //
        // T1 = input ^ dP mod P
        // T2 = input ^ dQ mod Q
        //
        BN_CHK( BNExpMod( &T1, &T, &ctx->DP, &ctx->P, &ctx->RP ) );
        BN_CHK( BNExpMod( &T2, &T, &ctx->DQ, &ctx->Q, &ctx->RQ ) );

        //
        // T = (T1 - T2) * (Q^-1 mod P) mod P
        //
        BN_CHK( BNSubMpi( &T, &T1, &T2 ) );
        BN_CHK( BNMulMpi( &T1, &T, &ctx->QP ) );
        BN_CHK( BNModMpi( &T, &T1, &ctx->P ) );

        //
        // output = T2 + T * Q
        //
        BN_CHK( BNMulMpi( &T1, &T, &ctx->Q ) );
        BN_CHK( BNAddMpi( &T, &T2, &T1 ) );
#endif

        olen = ctx->len;
        BN_CHK( BNWriteBinary( &T, output, olen ) );

cleanup:

        BNFree( &T, &T1, &T2, nullptr );

        if ( nRet != 0 )
        {
            return( SDCRYPTO_ERR_RSA_PRIVATE_FAILED | nRet );
        }

        return 0;

    }

    int32 RSAPkcs1Encrypt( SRSAContext *ctx,
                           int32 mode,
                           int32  ilen,
                           unsigned char *input,
                           unsigned char *output )
    {
        int32 nb_pad, olen;
        unsigned char *p = output;

        olen = ctx->len;

        switch ( ctx->padding )
        {
        case RSA_PKCS_V15:

            if ( ilen < 0 || olen < ilen + 11 )
            {
                return SDCRYPTO_ERR_RSA_BAD_INPUT_DATA;
            }

            nb_pad = olen - 3 - ilen;

            *p++ = 0;
            *p++ = RSA_CRYPT;

            while ( nb_pad-- > 0 )
            {
                do
                {
                    *p = (unsigned char) rand();
                }
                while( *p == 0 );
                p++;
            }
            *p++ = 0;
            memcpy( p, input, ilen );
            break;

        default:

            return SDCRYPTO_ERR_RSA_INVALID_PADDING;
        }

        return( ( mode == RSA_PUBLIC )
                ? RSAPublic(  ctx, output, output )
                : RSAPrivate( ctx, output, output ) );
    }

    int32 RSAPkcs1Decrypt( SRSAContext *ctx,
                           int32 mode,
                           int32 *olen,
                           unsigned char *input,
                           unsigned char *output )
    {
        int32 nRet, ilen;
        unsigned char *p;
        unsigned char buf[512];

        ilen = ctx->len;

        if ( ilen < 16 || ilen > (int) sizeof( buf ) )
        {
            return SDCRYPTO_ERR_RSA_BAD_INPUT_DATA;
        }

        nRet = ( mode == RSA_PUBLIC )
               ? RSAPublic(  ctx, input, buf )
               : RSAPrivate( ctx, input, buf );

        if ( nRet != 0 )
            return nRet;

        p = buf;

        switch ( ctx->padding )
        {
        case RSA_PKCS_V15:

            if ( *p++ != 0 || *p++ != RSA_CRYPT )
            {
                return SDCRYPTO_ERR_RSA_INVALID_PADDING;
            }

            while ( *p != 0 )
            {
                if ( p >= buf + ilen - 1 )
                {
                    return SDCRYPTO_ERR_RSA_INVALID_PADDING;
                }
                p++;
            }
            p++;
            break;

        default:

            return SDCRYPTO_ERR_RSA_INVALID_PADDING;
        }

        *olen = ilen - (int)(p - buf);
        memcpy( output, p, *olen );

        return( 0 );
    }

    int32 RSAPkcs1Sign( SRSAContext *ctx,
                        int32 mode,
                        int32 hash_id,
                        int32 hashlen,
                        unsigned char *hash,
                        unsigned char *sig )
    {
        int32 nb_pad, olen;
        unsigned char *p = sig;

        olen = ctx->len;

        switch ( ctx->padding )
        {
        case RSA_PKCS_V15:

            switch ( hash_id )
            {
            case RSA_RAW:
                nb_pad = olen - 3 - hashlen;
                break;

            case RSA_MD2:
            case RSA_MD4:
            case RSA_MD5:
                nb_pad = olen - 3 - 34;
                break;

            case RSA_SHA1:
                nb_pad = olen - 3 - 35;
                break;

            default:
                return SDCRYPTO_ERR_RSA_BAD_INPUT_DATA;
            }

            if ( nb_pad < 8 )
            {
                return SDCRYPTO_ERR_RSA_BAD_INPUT_DATA;
            }

            *p++ = 0;
            *p++ = RSA_SIGN;
            memset( p, 0xFF, nb_pad );
            p += nb_pad;
            *p++ = 0;
            break;

        default:

            return SDCRYPTO_ERR_RSA_INVALID_PADDING;
        }

        switch ( hash_id )
        {
        case RSA_RAW:
            memcpy( p, hash, hashlen );
            break;

        case RSA_MD2:
            memcpy( p, ASN1_HASH_MDX, 18 );
            memcpy( p + 18, hash, 16 );
            p[13] = 2;
            break;

        case RSA_MD4:
            memcpy( p, ASN1_HASH_MDX, 18 );
            memcpy( p + 18, hash, 16 );
            p[13] = 4;
            break;

        case RSA_MD5:
            memcpy( p, ASN1_HASH_MDX, 18 );
            memcpy( p + 18, hash, 16 );
            p[13] = 5;
            break;

        case RSA_SHA1:
            memcpy( p, ASN1_HASH_SHA1, 15 );
            memcpy( p + 15, hash, 20 );
            break;

        default:
            return SDCRYPTO_ERR_RSA_BAD_INPUT_DATA;
        }

        return ( ( mode == RSA_PUBLIC )
                 ? RSAPublic(  ctx, sig, sig )
                 : RSAPrivate( ctx, sig, sig ) );
    }

    int32 RSAPkcs1Verify( SRSAContext *ctx,
                          int32 mode,
                          int32 hash_id,
                          int32 hashlen,
                          unsigned char *hash,
                          unsigned char *sig )
    {
        int32 nRet, len, siglen;
        unsigned char *p, c;
        unsigned char buf[512];

        siglen = ctx->len;

        if ( siglen < 16 || siglen > (int) sizeof( buf ) )
        {
            return SDCRYPTO_ERR_RSA_BAD_INPUT_DATA;
        }

        nRet = ( mode == RSA_PUBLIC )
               ? RSAPublic(  ctx, sig, buf )
               : RSAPrivate( ctx, sig, buf );

        if( nRet != 0 )
            return( nRet );

        p = buf;

        switch ( ctx->padding )
        {
        case RSA_PKCS_V15:

            if ( *p++ != 0 || *p++ != RSA_SIGN )
            {
                return SDCRYPTO_ERR_RSA_INVALID_PADDING;
            }

            while ( *p != 0 )
            {
                if ( p >= buf + siglen - 1 || *p != 0xFF )
                {
                    return SDCRYPTO_ERR_RSA_INVALID_PADDING;
                }
                p++;
            }
            p++;
            break;

        default:

            return SDCRYPTO_ERR_RSA_INVALID_PADDING;
        }

        len = siglen - (int)( p - buf );

        if ( len == 34 )
        {
            c = p[13];
            p[13] = 0;

            if ( memcmp( p, ASN1_HASH_MDX, 18 ) != 0 )
            {
                return SDCRYPTO_ERR_RSA_VERIFY_FAILED;
            }

            if ( ( c == 2 && hash_id == RSA_MD2 ) ||
                    ( c == 4 && hash_id == RSA_MD4 ) ||
                    ( c == 5 && hash_id == RSA_MD5 ) )
            {
                if ( memcmp( p + 18, hash, 16 ) == 0 )
                {
                    return 0;
                }
                else
                {
                    return SDCRYPTO_ERR_RSA_VERIFY_FAILED;
                }
            }
        }

        if ( len == 35 && hash_id == RSA_SHA1 )
        {
            if ( memcmp( p, ASN1_HASH_SHA1, 15 ) == 0 &&
                    memcmp( p + 15, hash, 20 ) == 0 )
            {
                return 0;
            }
            else
            {
                return SDCRYPTO_ERR_RSA_VERIFY_FAILED;
            }
        }

        if ( len == hashlen && hash_id == RSA_RAW )
        {
            if ( memcmp( p, hash, hashlen ) == 0 )
            {
                return 0;
            }
            else
            {
                return SDCRYPTO_ERR_RSA_VERIFY_FAILED;
            }
        }

        return SDCRYPTO_ERR_RSA_INVALID_PADDING;
    }

    void RSAFree( SRSAContext *ctx )
    {
        BNFree( &ctx->RQ, &ctx->RP, &ctx->RN,
                &ctx->QP, &ctx->DQ, &ctx->DP,
                &ctx->Q,  &ctx->P,  &ctx->D,
                &ctx->E,  &ctx->N,  nullptr );
    }

    int32 RSASelfTest( int32 verbose )
    {
        const int32 KEY_LEN = 128;

        const char*RSA_N = "9292758453063D803DD603D5E777D788"
                           "8ED1D5BF35786190FA2F23EBC0848AEA"
                           "DDA92CA6C3D80B32C4D109BE0F36D6AE"
                           "7130B9CED7ACDF54CFC7555AC14EEBAB"
                           "93A89813FBF3C4F8066D2D800F7C38A8"
                           "1AE31942917403FF4946B0A83D3D3E05"
                           "EE57C6F5F5606FB5D4BC6CD34EE0801A"
                           "5E94BB77B07507233A0BC7BAC8F90F79";

        const char*RSA_E  = "10001";

        const char*RSA_D  = "24BF6185468786FDD303083D25E64EFC"
                            "66CA472BC44D253102F8B4A9D3BFA750"
                            "91386C0077937FE33FA3252D28855837"
                            "AE1B484A8A9A45F7EE8C0C634F99E8CD"
                            "DF79C5CE07EE72C7F123142198164234"
                            "CABB724CF78B8173B9F880FC86322407"
                            "AF1FEDFDDE2BEB674CA15F3E81A1521E"
                            "071513A1E85B5DFA031F21ECAE91A34D";

        const char*RSA_P  = "C36D0EB7FCD285223CFB5AABA5BDA3D8"
                            "2C01CAD19EA484A87EA4377637E75500"
                            "FCB2005C5C7DD6EC4AC023CDA285D796"
                            "C3D9E75E1EFC42488BB4F1D13AC30A57";

        const char*RSA_Q = "C000DF51A7C77AE8D7C7370C1FF55B69"
                           "E211C2B9E5DB1ED0BF61D0D9899620F4"
                           "910E4168387E3C30AA1E00C339A79508"
                           "8452DD96A9A5EA5D9DCA68DA636032AF";

        const char*RSA_DP = "C1ACF567564274FB07A0BBAD5D26E298"
                            "3C94D22288ACD763FD8E5600ED4A702D"
                            "F84198A5F06C2E72236AE490C93F07F8"
                            "3CC559CD27BC2D1CA488811730BB5725";

        const char*RSA_DQ = "4959CBF6F8FEF750AEE6977C155579C7"
                            "D8AAEA56749EA28623272E4F7D0592AF"
                            "7C1F1313CAC9471B5C523BFE592F517B"
                            "407A1BD76C164B93DA2D32A383E58357";

        const char*RSA_QP = "9AE7FBC99546432DF71896FC239EADAE"
                            "F38D18D2B2F0E2DD275AA977E2BF4411"
                            "F5A3B2A5D33605AEBBCCBA7FEB9F2D2F"
                            "A74206CEC169D74BF5A8C50D6F48EA08";

        const int32 PT_LEN = 24;

        const char*RSA_PT = "\xAA\xBB\xCC\x03\x02\x01\x00\xFF\xFF\xFF\xFF\xFF"
                            "\x11\x22\x33\x0A\x0B\x0C\xCC\xDD\xDD\xDD\xDD\xDD";

        int32 len;
        SRSAContext rsa;
        unsigned char sha1sum[20];
        unsigned char rsa_plaintext[PT_LEN];
        unsigned char rsa_decrypted[PT_LEN];
        unsigned char rsa_ciphertext[KEY_LEN];

        memset( &rsa, 0, sizeof( SRSAContext ) );

        rsa.len = KEY_LEN;
	BNReadString( &rsa.N , 16, RSA_N  );
	BNReadString( &rsa.E , 16, RSA_E  );
	BNReadString( &rsa.D , 16, RSA_D  );
	BNReadString( &rsa.P , 16, RSA_P  );
	BNReadString( &rsa.Q , 16, RSA_Q  );
	BNReadString( &rsa.DP, 16, RSA_DP );
	BNReadString( &rsa.DQ, 16, RSA_DQ );
	BNReadString( &rsa.QP, 16, RSA_QP );

        if( verbose != 0 )
            printf( "  RSA key validation: " );

        if( RSACheckPubKey(  &rsa ) != 0 ||
                RSACheckPrivKey( &rsa ) != 0 )
        {
            if( verbose != 0 )
                printf( "failed\n" );

            return( 1 );
        }

        if( verbose != 0 )
            printf( "passed\n  PKCS#1 encryption : " );

        memcpy( rsa_plaintext, RSA_PT, PT_LEN );

        if( RSAPkcs1Encrypt( &rsa, RSA_PUBLIC, PT_LEN,
                             rsa_plaintext, rsa_ciphertext ) != 0 )
        {
            if( verbose != 0 )
                printf( "failed\n" );

            return( 1 );
        }

        if( verbose != 0 )
            printf( "passed\n  PKCS#1 decryption : " );

        if( RSAPkcs1Decrypt( &rsa, RSA_PRIVATE, &len,
                             rsa_ciphertext, rsa_decrypted ) != 0 )
        {
            if( verbose != 0 )
                printf( "failed\n" );

            return( 1 );
        }

        if( memcmp( rsa_decrypted, rsa_plaintext, len ) != 0 )
        {
            if( verbose != 0 )
                printf( "failed\n" );

            return( 1 );
        }

        if( verbose != 0 )
            printf( "passed\n  PKCS#1 data sign  : " );

#if 0
        sha1( rsa_plaintext, PT_LEN, sha1sum );
#endif
        strncpy((char*)sha1sum, (const char*)rsa_plaintext, 20);

        if( RSAPkcs1Sign( &rsa, RSA_PRIVATE, RSA_SHA1, 20,
                          sha1sum, rsa_ciphertext ) != 0 )
        {
            if( verbose != 0 )
                printf( "failed\n" );

            return( 1 );
        }

        if( verbose != 0 )
            printf( "passed\n  PKCS#1 sig. verify: " );

        if( RSAPkcs1Verify( &rsa, RSA_PUBLIC, RSA_SHA1, 20,
                            sha1sum, rsa_ciphertext ) != 0 )
        {
            if( verbose != 0 )
                printf( "failed\n" );

            return( 1 );
        }

        if( verbose != 0 )
            printf( "passed\n\n" );

        RSAFree( &rsa );

        return 0;
    }

}

#endif //#ifndef WIN64 
