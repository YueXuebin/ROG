#include "SDuPCH.h"
#include "sdrsa.h"
#include "sdcryptorsa.h"

#ifndef WIN64 

namespace Crown
{

////////////////////////////////////////////////////////////////////////////////
// typedef check return macro
#define BN_SERIAL_CHK(f)  if ( ( nRet = f ) != 0 ) \
	{						   \
	goto ERROR_HANDLE;	   \
}


////////////////////////////////////////////////////////////////////////////////

    static int RandWrapper( void *rng_state )
    {
        if( rng_state != nullptr )
            rng_state  = nullptr;

        srand((unsigned)time(nullptr) + rand());
        return( rand() );
    }

////////////////////////////////////////////////////////////////////////////////

    void CSDRsa::RSAContextInit( SRSAContext *pCtx )
    {
        RSAInit( pCtx, 0, RSA_PKCS_V15, RandWrapper, nullptr );
    }

    void CSDRsa::RSAContextFree( SRSAContext *pCtx )
    {
        RSAFree(pCtx);
    }

    int32 CSDRsa::RSANSize(SRSAContext *pCtx)
    {
        return pCtx->len;
    }

    SRSAContext* SDAPI CSDRsa::RSANew()
    {
        SRSAContext* pRSAContext = ( SRSAContext* )malloc( sizeof(SRSAContext) );
        if ( nullptr != pRSAContext )
        {
            RSAContextInit( pRSAContext );
            return pRSAContext;
        }
        else
        {
            return nullptr;
        }
    }

    bool SDAPI CSDRsa::RSAGenKeyPair( SRSAContext *pCtx, int32 nBits, int32 nExponent )
    {
        int32 nRet = RSAGenKey( pCtx, nBits, nExponent );
        if ( 0 == nRet )
        {
            return true;
        }
        else
        {
            // write logger here!nRet was the error code
            return false;
        }
    }

    int32 CSDRsa::RSAGetPubKeyLen(SRSAContext *pPubKey)
    {
        return RSAGetPubKeyLenSub(pPubKey) + 3*sizeof(int32);
    }

    int32 CSDRsa::RSAGetPubKeyLenSub(SRSAContext *pPubKey)
    {
        int32 nLen = BNGetTotalBytes( &(pPubKey->N) );
        nLen += BNGetTotalBytes( &(pPubKey->E) );
        return nLen;
    }

    int32 SDAPI CSDRsa::RSAGetPrivKeyLen(SRSAContext* pPrivKey)
    {
        return RSAGetPrivKeyLenSub(pPrivKey) + 9*sizeof(int32);
    }

    int32 CSDRsa::RSAGetPrivKeyLenSub(SRSAContext *pPrivKey)
    {
        int32 nLen = BNGetTotalBytes( &(pPrivKey->N) );
        nLen += BNGetTotalBytes( &(pPrivKey->E) );
        nLen += BNGetTotalBytes( &(pPrivKey->D) );
        nLen += BNGetTotalBytes( &(pPrivKey->P) );
        nLen += BNGetTotalBytes( &(pPrivKey->Q) );
        nLen += BNGetTotalBytes( &(pPrivKey->DP) );
        nLen += BNGetTotalBytes( &(pPrivKey->DQ) );
        nLen += BNGetTotalBytes( &(pPrivKey->QP) );
        return nLen;
    }

    bool CSDRsa::RSAReadPubKeySub(SRSAContext* pPubKey, byte* pBuf, int32 nRet)
    {
        int32 nLenBNUM = *(int32*)(pBuf);
        pBuf += sizeof(int32);
        BN_SERIAL_CHK( BNReadBinary(&pPubKey->N, pBuf, nLenBNUM) );
        pBuf += nLenBNUM;

        nLenBNUM = *(int32*)(pBuf);
        pBuf += sizeof(int32);
        BN_SERIAL_CHK( BNReadBinary(&pPubKey->E, pBuf, nLenBNUM) );
        pBuf += nLenBNUM;

        pPubKey->len = ( BNGetMSB( &pPubKey->N ) + 7 ) >> 3;

        return true;

ERROR_HANDLE:
        nRet = SDCRYPTO_BNERR_BAD_INPUT_DATA;
        return  false;
    }

    bool CSDRsa::RSAReadPrivKeySub(SRSAContext* pPrivKey, byte* pBuf, int32 nRet)
    {
        int32 nLenBNUM = *(int32*)(pBuf);
        pBuf += sizeof(int32);
        BN_SERIAL_CHK( BNReadBinary(&pPrivKey->N, pBuf, nLenBNUM) );
        pBuf += nLenBNUM;

        nLenBNUM = *(int32*)(pBuf);
        pBuf += sizeof(int32);
        BN_SERIAL_CHK( BNReadBinary(&pPrivKey->E, pBuf, nLenBNUM) );
        pBuf += nLenBNUM;

        nLenBNUM = *(int32*)(pBuf);
        pBuf += sizeof(int32);
        BN_SERIAL_CHK( BNReadBinary(&pPrivKey->D, pBuf, nLenBNUM) );
        pBuf += nLenBNUM;

        nLenBNUM = *(int32*)(pBuf);
        pBuf += sizeof(int32);
        BN_SERIAL_CHK( BNReadBinary(&pPrivKey->P, pBuf, nLenBNUM) );
        pBuf += nLenBNUM;

        nLenBNUM = *(int32*)(pBuf);
        pBuf += sizeof(int32);
        BN_SERIAL_CHK( BNReadBinary(&pPrivKey->Q, pBuf, nLenBNUM) );
        pBuf += nLenBNUM;

        nLenBNUM = *(int32*)(pBuf);
        pBuf += sizeof(int32);
        BN_SERIAL_CHK( BNReadBinary(&pPrivKey->DP, pBuf, nLenBNUM) );
        pBuf += nLenBNUM;

        nLenBNUM = *(int32*)(pBuf);
        pBuf += sizeof(int32);
        BN_SERIAL_CHK( BNReadBinary(&pPrivKey->DQ, pBuf, nLenBNUM) );
        pBuf += nLenBNUM;

        nLenBNUM = *(int32*)(pBuf);
        pBuf += sizeof(int32);
        BN_SERIAL_CHK( BNReadBinary(&pPrivKey->QP, pBuf, nLenBNUM) );
        pBuf += nLenBNUM;

        pPrivKey->len = ( BNGetMSB( &pPrivKey->N ) + 7 ) >> 3;

        return true;

ERROR_HANDLE:
        nRet = SDCRYPTO_BNERR_BAD_INPUT_DATA;
        return  false;
    }

    bool CSDRsa::RSAWritePubKeySub(SRSAContext* pPubKey, byte* pBuf, int32 nRet)
    {
        int32 nHeaderLen = RSAGetPubKeyLenSub(pPubKey) + 3*sizeof(int32);
        memcpy(pBuf, &nHeaderLen, sizeof(int32));
        pBuf += sizeof(int32);

        int32 nLenBNUM = BNGetTotalBytes(&pPubKey->N);
        memcpy(pBuf, &nLenBNUM, sizeof(int32));
        pBuf += sizeof(int32);
        BN_SERIAL_CHK( BNWriteBinary( &(pPubKey->N), pBuf, nLenBNUM) );
        pBuf += nLenBNUM;

        nLenBNUM = BNGetTotalBytes(&pPubKey->E);
        memcpy(pBuf, &nLenBNUM, sizeof(int32));
        pBuf += sizeof(int32);
        BN_SERIAL_CHK( BNWriteBinary( &(pPubKey->E), pBuf, nLenBNUM));
        pBuf += nLenBNUM;

        return true;

ERROR_HANDLE:
        nRet = SDCRYPTO_BNERR_BAD_INPUT_DATA;
        return  false;
    }

    bool CSDRsa::RSAWritePrivKeySub(SRSAContext* pPrivKey, byte* pBuf, int32 nRet)
    {
        int32 nHeaderLen = RSAGetPrivKeyLenSub(pPrivKey) + 9*sizeof(int32);
        memcpy(pBuf, &nHeaderLen, sizeof(int32));
        pBuf += sizeof(int32);

        int32 nLenBNUM = BNGetTotalBytes(&pPrivKey->N);
        memcpy(pBuf, &nLenBNUM, sizeof(int32));
        pBuf += sizeof(int32);
        BN_SERIAL_CHK( BNWriteBinary( &(pPrivKey->N), pBuf, nLenBNUM) );
        pBuf += nLenBNUM;

        nLenBNUM = BNGetTotalBytes(&pPrivKey->E);
        memcpy(pBuf, &nLenBNUM, sizeof(int32));
        pBuf += sizeof(int32);
        BN_SERIAL_CHK( BNWriteBinary( &(pPrivKey->E), pBuf, nLenBNUM));
        pBuf += nLenBNUM;

        nLenBNUM = BNGetTotalBytes(&pPrivKey->D);
        memcpy(pBuf, &nLenBNUM, sizeof(int32));
        pBuf += sizeof(int32);
        BN_SERIAL_CHK( BNWriteBinary( &(pPrivKey->D), pBuf, nLenBNUM));
        pBuf += nLenBNUM;

        nLenBNUM = BNGetTotalBytes(&pPrivKey->P);
        memcpy(pBuf, &nLenBNUM, sizeof(int32));
        pBuf += sizeof(int32);
        BN_SERIAL_CHK( BNWriteBinary( &(pPrivKey->P), pBuf, nLenBNUM));
        pBuf += nLenBNUM;

        nLenBNUM = BNGetTotalBytes(&pPrivKey->Q);
        memcpy(pBuf, &nLenBNUM, sizeof(int32));
        pBuf += sizeof(int32);
        BN_SERIAL_CHK( BNWriteBinary( &(pPrivKey->Q), pBuf, nLenBNUM));
        pBuf += nLenBNUM;

        nLenBNUM = BNGetTotalBytes(&pPrivKey->DP);
        memcpy(pBuf, &nLenBNUM, sizeof(int32));
        pBuf += sizeof(int32);
        BN_SERIAL_CHK( BNWriteBinary( &(pPrivKey->DP), pBuf, nLenBNUM));
        pBuf += nLenBNUM;

        nLenBNUM = BNGetTotalBytes(&pPrivKey->DQ);
        memcpy(pBuf, &nLenBNUM, sizeof(int32));
        pBuf += sizeof(int32);
        BN_SERIAL_CHK( BNWriteBinary( &(pPrivKey->DQ), pBuf, nLenBNUM));
        pBuf += nLenBNUM;

        nLenBNUM = BNGetTotalBytes(&pPrivKey->QP);
        memcpy(pBuf, &nLenBNUM, sizeof(int32));
        pBuf += sizeof(int32);
        BN_SERIAL_CHK( BNWriteBinary( &(pPrivKey->QP), pBuf, nLenBNUM));
        pBuf += nLenBNUM;

        return true;

ERROR_HANDLE:
        nRet = SDCRYPTO_BNERR_BAD_INPUT_DATA;
        return  false;
    }

    bool CSDRsa::RSASavePubKey(SRSAContext *pPubKey, const char *pszFile)
    {
        int32 nRet = -1;
        byte acBuf[2048] = {0};
        byte* pBuf = acBuf;

        int32 nHeaderLen = 0;

        FILE* fp = fopen( pszFile, "wb" );
        if ( fp == nullptr )
        {
            nRet = SDCRYPTO_BNERR_BAD_INPUT_DATA;
            goto ERROR_HANDLE;
        }

        nHeaderLen = RSAGetPubKeyLenSub(pPubKey) + 3*sizeof(int32);

        if(!RSAWritePubKeySub(pPubKey, pBuf, nRet))
            goto ERROR_HANDLE;

        if( (int32)fwrite(acBuf, 1, nHeaderLen, fp) != nHeaderLen )
        {
            nRet = SDCRYPTO_BNERR_FILE_IO_ERROR;
            goto ERROR_HANDLE;
        }

        fclose( fp );
        return true;

ERROR_HANDLE:
        fclose( fp );
        // write logger here!nRet was the error code
        return false;
    }

    bool CSDRsa::RSASavePubKeyToMem(SRSAContext *pPubKey, byte* pBuffer, int32 nBufLen)
    {
        if (nullptr ==  pBuffer || nullptr == pPubKey ||
                nBufLen < RSAGetPubKeyLen(pPubKey))
        {
            return false;
        }

        int32 nRet = -1;
        return RSAWritePubKeySub(pPubKey, pBuffer, nRet);
    }


    bool CSDRsa::RSALoadPubKey(SRSAContext *&pPubKey, const char *pszFile)
    {
        int32 nRet = -1;

        int32 nHeaderLen = 0;

        byte  keyBuf[2048] = {0};
        byte* pBuf = keyBuf;

        int32 nFileLen = 0;

        FILE* fp = fopen( pszFile, "rb" );
        if (fp == nullptr)
        {
            nRet = SDCRYPTO_BNERR_BAD_INPUT_DATA;
            goto ERROR_HANDLE;
        }

        pPubKey = RSANew();



        fseek(fp, 0, SEEK_END);
        nFileLen = ftell(fp);
        fseek(fp, 0, SEEK_SET);
        if( (int32)fread(pBuf, 1, nFileLen, fp) != nFileLen )
        {
            nRet = SDCRYPTO_BNERR_FILE_IO_ERROR;
            goto ERROR_HANDLE;
        }

        fclose( fp );

        nHeaderLen = *(int32*)(pBuf);
        pBuf += sizeof(int32);
        if(nFileLen != nHeaderLen)
        {
            nRet = SDCRYPTO_BNERR_BAD_INPUT_DATA;
            goto ERROR_HANDLE;
        }

        if(!RSAReadPubKeySub(pPubKey, pBuf, nRet))
            goto ERROR_HANDLE;

        return true;

ERROR_HANDLE:

        fclose( fp );
        // write logger here!nRet was the error code
        return false;
    }

    bool CSDRsa::RSALoadPubKeyFromMem(SRSAContext *&pPubKey, const byte* pBuffer, int32 nBufLen)
    {
        int32 nRet = -1;

        byte* pBuf = (byte*)pBuffer;

        int32 nHeaderLen = 0;

        if( pBuffer == nullptr )
        {
            nRet = SDCRYPTO_BNERR_BAD_INPUT_DATA;
            goto ERROR_HANDLE;
        }

        pPubKey = RSANew();

        nHeaderLen = *(int32*)(pBuf);
        pBuf += sizeof(int32);
        if(nBufLen < nHeaderLen)
        {
            nRet = SDCRYPTO_BNERR_BAD_INPUT_DATA;
            goto ERROR_HANDLE;
        }

        if(!RSAReadPubKeySub(pPubKey, pBuf, nRet))
            goto ERROR_HANDLE;

        return true;

ERROR_HANDLE:
        return false;
    }


    bool CSDRsa::RSASavePrivKey(SRSAContext *pPrivKey, const char *pszFile)
    {
        FILE* fp = fopen( pszFile, "wb" );
        if (fp == nullptr)
        {
            return false;
        }

        int32 nRet = -1;

        byte acBuf[2048] = {0};
        byte* pBuf = acBuf;

        int32 nHeaderLen = RSAGetPrivKeyLenSub(pPrivKey) + 9*sizeof(int32);

        if(!RSAWritePrivKeySub(pPrivKey, pBuf, nRet))
        {
            goto ERROR_HANDLE;
        }

        if( (int32)fwrite(acBuf, 1, nHeaderLen, fp) != nHeaderLen )
        {
            nRet = SDCRYPTO_BNERR_FILE_IO_ERROR;
            goto ERROR_HANDLE;
        }

        fclose( fp );
        return true;


ERROR_HANDLE:

        fclose(fp);
        // write logger here!nRet was the error code

        return false;
    }

    bool CSDRsa::RSASavePrivKeyToMem(SRSAContext *pPrivKey, byte* pBuffer, int32 nBufLen)
    {
        int32 nRet = -1;
        int32 nHeaderLen = 0;
        byte* pBuf = pBuffer;

        if( pBuffer == nullptr )
        {
            nRet = SDCRYPTO_BNERR_BAD_INPUT_DATA;
            goto ERROR_HANDLE;
        }

        pPrivKey = RSANew();

        nHeaderLen = *(int32*)(pBuf);
        pBuf += sizeof(int32);
        if(nBufLen < nHeaderLen)
        {
            nRet = SDCRYPTO_BNERR_BAD_INPUT_DATA;
            goto ERROR_HANDLE;
        }

        if(!RSAReadPrivKeySub(pPrivKey, pBuf, nRet))
            goto ERROR_HANDLE;

        return true;

ERROR_HANDLE:
        return false;
    }

    bool CSDRsa::RSALoadPrivKey(SRSAContext *&pPrivKey, const char *pszFile)
    {
        int32 nRet = -1;

        FILE* fp = fopen( pszFile, "rb" );
        if (fp == nullptr)
        {
            return false;
        }

        pPrivKey = RSANew();

        byte keyBuf[2048] = {0};
        byte* pBuf = keyBuf;

        int32 nHeaderLen = 0;

        fseek(fp, 0, SEEK_END);
        int32 nFileLen = ftell(fp);
        fseek(fp, 0, SEEK_SET);
        if( (int32)fread(pBuf, 1, nFileLen, fp) != nFileLen )
        {
            nRet = SDCRYPTO_BNERR_FILE_IO_ERROR;
            fclose(fp);
            goto ERROR_HANDLE;
        }

        fclose( fp );

        nHeaderLen = *(int32*)(pBuf);
        pBuf += sizeof(int32);
        if(nFileLen != nHeaderLen)
        {
            nRet = SDCRYPTO_BNERR_BAD_INPUT_DATA;
            goto ERROR_HANDLE;
        }

        RSAReadPrivKeySub(pPrivKey, pBuf, nRet);

        return true;
ERROR_HANDLE:
        fclose( fp );
        // write logger here!nRet was the error code

        return false;
    }

    bool CSDRsa::RSALoadPrivKeyFromMem(SRSAContext *&pPrivKey, const byte* pBuffer, int32 nBufLen)
    {
        int32 nRet = -1;

        byte* pBuf = (byte*)pBuffer;
        int32 nHeaderLen = 0;

        if( pBuffer == nullptr )
        {
            nRet = SDCRYPTO_BNERR_BAD_INPUT_DATA;
            goto ERROR_HANDLE;
        }

        pPrivKey = RSANew();

        nHeaderLen = *(int32*)(pBuf);
        pBuf += sizeof(int32);
        if(nBufLen < nHeaderLen)
        {
            nRet = SDCRYPTO_BNERR_BAD_INPUT_DATA;
            goto ERROR_HANDLE;
        }

        if(RSAReadPrivKeySub(pPrivKey, (byte*)pBuf, nRet))
            goto ERROR_HANDLE;

ERROR_HANDLE:
        return false;
    }

    int32 SDAPI CSDRsa::RSAPubEncrypt( SRSAContext	*pCtx,
                                 int32			nInLen,
                                 byte *pInput,
                                 byte *pOutput )
    {
        uint32 nRemain = (uint32)nInLen;
        uint32 nNSize  = RSANSize(pCtx);
        byte* pCurInPos  = pInput;
        byte* pCurOutPos = pOutput;
        int32 nOutLen = 0;

        do
        {
            uint32 nCurEncSize = ( nNSize - 16 ) < nRemain ? ( nNSize - 16 ) : nRemain;

            int32 nRet = RSAPkcs1Encrypt( pCtx, RSA_PUBLIC, nCurEncSize, pCurInPos, pCurOutPos );
            if ( 0 != nRet )
            {
                // write logger here! nRet was the error code
                return -1;
            }
            else
            {
                nRemain	   -= nCurEncSize;
                pCurInPos  += nCurEncSize;
                pCurOutPos += nNSize;
                nOutLen    += nNSize;
            }
        }
        while ( nRemain > 0 );

        return nOutLen;
    }

    int32 SDAPI CSDRsa::RSAPrivDecrypt( SRSAContext	   *pCtx,
                                  int32         nInLen,
                                  byte *pInput,
                                  byte *pOutput )
    {
        uint32 nRemain = (uint32)nInLen;
        uint32 nNSize = RSANSize(pCtx);
        byte* pCurInPos  = pInput;
        byte* pCurOutPos = pOutput;

        int32 nOutLenSingle = 0;
        int32 nOutLenTotal  = 0;
        do
        {
            uint32 nCurDecSize = nNSize < nRemain ? nNSize : nRemain;

            int32 nRet = RSAPkcs1Decrypt( pCtx, RSA_PRIVATE, &nOutLenSingle, pCurInPos, pCurOutPos);
            if ( 0 != nRet )
            {
                // write logger here! nRet was the error code
                return -1;
            }
            else
            {
                nRemain      -= nCurDecSize;
                pCurInPos    += nCurDecSize;
                pCurOutPos   += nOutLenSingle;
                nOutLenTotal += nOutLenSingle;
            }
        }
        while(  nRemain > 0 );

        return nOutLenTotal;
    }

}
#endif //#ifndef WIN64 
