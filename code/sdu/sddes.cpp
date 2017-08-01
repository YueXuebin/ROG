#include "SDuPCH.h"
#include "sddes.h"
#include "sdcryptodes.h"


namespace Crown
{
    void CSDDes::DESGenKey( SDESBlock* pIV )
    {
        DESRandomKey( pIV );
    }

    void CSDDes::DESGenKey( SDES3Block* pIV )
    {
        GenDES3Key( pIV );
    }

    void CSDDes::DESGenEncKeySche( SDESContext *pCtx, SDESBlock& oKey )
    {
        DESSetEncKey( pCtx, oKey.acBlock );
    }

    void CSDDes::DESGenEncKeySche( SDES3Context *pCtx, SDES3Block& oKey )
    {
        DES3SetEncKey( pCtx, &oKey );
    }


    void CSDDes::DESGenDecKeySche( SDESContext *pCtx, SDESBlock& oKey )
    {
        DESSetDecKey( pCtx, oKey.acBlock );
    }

    void CSDDes::DESGenDecKeySche( SDES3Context *pCtx, SDES3Block& oKey )
    {
        DES3SetDecKey( pCtx, &oKey );
    }

    void CSDDes::DESEncryptNCBC( SDESContext    *pCtx,
                                 SDESBlock		 *pIV,
                                 int32		 nLength,
                                 unsigned char *pInput,
                                 unsigned char *pOutput )
    {
        SDESBlock IVBak;
        memcpy(&IVBak, (const void*)pIV, sizeof(SDESBlock));
        DESCryptCBC( pCtx, DES_ENCRYPT, nLength, pIV, pInput, pOutput );
        memcpy(pIV, (const void*)&IVBak, sizeof(SDESBlock));
    }

    void CSDDes::DESEncryptNCBC( SDES3Context   *pCtx,
                                 SDES3Block	 *pIV,
                                 int32		 nLength,
                                 unsigned char *pInput,
                                 unsigned char *pOutput )
    {
        SDES3Block IVBak;
        memcpy(&IVBak, (const void*)pIV, sizeof(SDES3Block));
        DES3CryptCBC( pCtx, DES_ENCRYPT, nLength, pIV, pInput, pOutput );
        memcpy(pIV, (const void*)&IVBak, sizeof(SDES3Block));
    }

    void CSDDes::DESDecryptNCBC( SDESContext	 *pCtx,
                                 SDESBlock		 *pIV,
                                 int32		 nLength,
                                 unsigned char *pInput,
                                 unsigned char *pOutput )
    {
        SDESBlock IVBak;
        memcpy(&IVBak, (const void*)pIV, sizeof(SDESBlock));
        DESCryptCBC( pCtx, DES_DECRYPT, nLength, pIV, pInput, pOutput );
        memcpy(pIV, (const void*)&IVBak, sizeof(SDESBlock));
    }

    void CSDDes::DESDecryptNCBC( SDES3Context	 *pCtx,
                                 SDES3Block	 *pIV,
                                 int32		 nLength,
                                 unsigned char *pInput,
                                 unsigned char *pOutput )
    {
        SDES3Block IVBak;
        memcpy(&IVBak, (const void*)pIV, sizeof(SDES3Block));
        DES3CryptCBC( pCtx, DES_DECRYPT, nLength, pIV, pInput, pOutput );
        memcpy(pIV, (const void*)&IVBak, sizeof(SDES3Block));
    }


}

