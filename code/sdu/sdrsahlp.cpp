#include "SDuPCH.h"
#include <string.h>
#include <assert.h>
#include "sdrsahlp.h"
#if SDU_WITH_OPENSSL
namespace Crown
{
    static BIGNUM* NewBigNum();
    static void FreeBigNum(BIGNUM* &pBigNum);

    static void CopyBigNum(BIGNUM* pstTo, const BIGNUM* pstFrom);
    static uint32 SerializeBigNum(const BIGNUM* pKey, byte* pBuf);
    static void ReadBigNum(BIGNUM* &pBigNum, byte* &p);

    bool GenerateRsaKey(int32 nBit, RSA *pstPublicKey, RSA *pstPrivateKey)
    {
        RSA* pstRsa = RSA_generate_key(nBit, 65537, nullptr, nullptr);
        if(nullptr == pstRsa)
            return false;

        //Copy Public Key
        pstPublicKey->n = NewBigNum();
        CopyBigNum(pstPublicKey->n, pstRsa->n);
        pstPublicKey->e = NewBigNum();
        CopyBigNum(pstPublicKey->e, pstRsa->e);
        pstPublicKey->d = nullptr;
        pstPublicKey->p = nullptr;
        pstPublicKey->q = nullptr;
        pstPublicKey->dmp1 = NewBigNum();
        CopyBigNum(pstPublicKey->dmp1, pstRsa->dmp1);
        pstPublicKey->dmq1 = NewBigNum();
        CopyBigNum(pstPublicKey->dmq1, pstRsa->dmq1);

        //Copy Private Key
        pstPrivateKey->n = NewBigNum();
        CopyBigNum(pstPrivateKey->n, pstRsa->n);
        pstPrivateKey->e = NewBigNum();
        CopyBigNum(pstPrivateKey->e, pstRsa->e);
        pstPrivateKey->d = NewBigNum();
        CopyBigNum(pstPrivateKey->d, pstRsa->d);
        pstPrivateKey->p = NewBigNum();
        CopyBigNum(pstPrivateKey->p, pstRsa->p);
        pstPrivateKey->q = NewBigNum();
        CopyBigNum(pstPrivateKey->q, pstRsa->q);
        pstPrivateKey->dmp1 = NewBigNum();
        CopyBigNum(pstPrivateKey->dmp1, pstRsa->dmp1);
        pstPrivateKey->dmq1 = NewBigNum();
        CopyBigNum(pstPrivateKey->dmq1, pstRsa->dmq1);

        RSA_free(pstRsa);

        return true;
    }

    void FreePrivateKey(RSA*& pstPrivateKey)
    {
        CnVerify(pstPrivateKey != nullptr);
        FreeBigNum(pstPrivateKey->n);
        FreeBigNum(pstPrivateKey->e);
        FreeBigNum(pstPrivateKey->d);
        FreeBigNum(pstPrivateKey->p);
        FreeBigNum(pstPrivateKey->q);
        FreeBigNum(pstPrivateKey->dmp1);
        FreeBigNum(pstPrivateKey->dmq1);

        RSA_free(pstPrivateKey);

        pstPrivateKey = nullptr;
    }

    void FreePublicKey(RSA*& pstPublicKey)
    {
        CnVerify(pstPublicKey != nullptr);
        FreeBigNum(pstPublicKey->n);
        FreeBigNum(pstPublicKey->e);
        FreeBigNum(pstPublicKey->dmp1);
        FreeBigNum(pstPublicKey->dmq1);

        RSA_free(pstPublicKey);

        pstPublicKey = nullptr;
    }

    bool SavePublicKey(const RSA* pstPublicKey, const char* pszFile)
    {
        FILE* fp = fopen(pszFile, "wb");
        if(fp == nullptr)
            return false;

        byte pBuf[65535] = {0};
        byte* p = pBuf;
        uint32 nTotalLen = 0;

        uint32 nDataLen = SerializeBigNum(pstPublicKey->n, p);
        p += nDataLen;
        nTotalLen += nDataLen;

        nDataLen = SerializeBigNum(pstPublicKey->e, p);
        p += nDataLen;
        nTotalLen += nDataLen;

        nDataLen = SerializeBigNum(pstPublicKey->dmp1, p);
        p += nDataLen;
        nTotalLen += nDataLen;

        nDataLen = SerializeBigNum(pstPublicKey->dmq1, p);
        p += nDataLen;
        nTotalLen += nDataLen;

        if(fwrite(pBuf, sizeof(byte), nTotalLen, fp) < (size_t)nDataLen)
        {
            fclose(fp);
            return false;
        }
        fclose(fp);
        return true;
    }

    bool SavePrivateKey(const RSA* pstPrivateKey, const char* pszFile)
    {
        FILE* fp = fopen(pszFile, "wb");
        if(fp == nullptr)
            return false;

        byte pBuf[65535] = {0};
        byte* p = pBuf;
        uint32 nTotalLen = 0;

        uint32 nDataLen = SerializeBigNum(pstPrivateKey->n, p);
        p += nDataLen;
        nTotalLen += nDataLen;

        nDataLen = SerializeBigNum(pstPrivateKey->e, p);
        p += nDataLen;
        nTotalLen += nDataLen;

        nDataLen = SerializeBigNum(pstPrivateKey->d, p);
        p += nDataLen;
        nTotalLen += nDataLen;

        nDataLen = SerializeBigNum(pstPrivateKey->p, p);
        p += nDataLen;
        nTotalLen += nDataLen;

        nDataLen = SerializeBigNum(pstPrivateKey->q, p);
        p += nDataLen;
        nTotalLen += nDataLen;

        nDataLen = SerializeBigNum(pstPrivateKey->dmp1, p);
        p += nDataLen;
        nTotalLen += nDataLen;

        nDataLen = SerializeBigNum(pstPrivateKey->dmq1, p);
        p += nDataLen;
        nTotalLen += nDataLen;

        if(fwrite(pBuf, sizeof(byte), nTotalLen, fp) < (size_t)nDataLen)
        {
            fclose(fp);
            return false;
        }
        fclose(fp);
        return true;
    }

    bool LoadPublicKey(RSA*& pstPublicKey, const char* pszFile)
    {
        CnVerify(pstPublicKey == nullptr);

        FILE* fp = fopen(pszFile, "rb");
        if(fp == nullptr)
            return false;

        byte pBuf[65535] = {0};
        byte* p = pBuf;

        fread(pBuf, sizeof(byte), sizeof(pBuf), fp);

        pstPublicKey = RSA_new();

        ReadBigNum(pstPublicKey->n, p);
        ReadBigNum(pstPublicKey->e, p);
        ReadBigNum(pstPublicKey->dmp1, p);
        ReadBigNum(pstPublicKey->dmq1, p);

        fclose(fp);

        return true;
    }

    bool LoadPrivateKey(RSA*& pstPrivateKey, const char *pszFile)
    {
        CnVerify(pstPrivateKey == nullptr);

        FILE* fp = fopen(pszFile, "rb");
        if(fp == nullptr)
            return false;
        byte pBuf[65535] = {0};
        byte* p = pBuf;

        fread(pBuf, sizeof(byte), sizeof(pBuf), fp);

        pstPrivateKey = RSA_new();

        ReadBigNum(pstPrivateKey->n, p);
        ReadBigNum(pstPrivateKey->e, p);
        ReadBigNum(pstPrivateKey->d, p);
        ReadBigNum(pstPrivateKey->p, p);
        ReadBigNum(pstPrivateKey->q, p);
        ReadBigNum(pstPrivateKey->dmp1, p);
        ReadBigNum(pstPrivateKey->dmq1, p);

        fclose(fp);

        return true;
    }

    void LoadPublicKeyFromMem(RSA*& pstKey, byte* pbyKeyBuf, uint16 wKeyLen)
    {
        CnVerify(pstKey == nullptr);

        byte* p = pbyKeyBuf;
        pstKey = RSA_new();

        ReadBigNum(pstKey->n, p);
        ReadBigNum(pstKey->e, p);
        ReadBigNum(pstKey->dmp1, p);
        ReadBigNum(pstKey->dmq1, p);
    }

    void LoadPrivateKeyFromMem(RSA*& pstKey, byte* pbyKeyBuf, uint16 wKeyLen)
    {
        CnVerify(pstKey == nullptr);

        byte* p = pbyKeyBuf;
        pstKey = RSA_new();

        ReadBigNum(pstKey->n, p);
        ReadBigNum(pstKey->e, p);
        ReadBigNum(pstKey->d, p);
        ReadBigNum(pstKey->p, p);
        ReadBigNum(pstKey->q, p);
        ReadBigNum(pstKey->dmp1, p);
        ReadBigNum(pstKey->dmq1, p);
    }

    BIGNUM * NewBigNum()
    {
        BIGNUM *pstRet = new BIGNUM;
        pstRet->d = nullptr;
        pstRet->top = 0;
        pstRet->dmax = 0;
        pstRet->neg = 0;
        pstRet->flags = 1;

        return pstRet;
    }

    void FreeBigNum(BIGNUM*& pBigNum)
    {
        CnVerify(pBigNum != nullptr);
        if(pBigNum->d != nullptr)
            delete[] pBigNum->d;
        pBigNum->d = nullptr;

        delete pBigNum;
        pBigNum = nullptr;
    }

    void CopyBigNum(BIGNUM *pstTo, const BIGNUM *pstFrom)
    {
        pstTo->d = new BN_ULONG[pstFrom->dmax];
        memset(pstTo->d, 0, sizeof(BN_ULONG) * pstFrom->dmax);
        memcpy(pstTo->d, pstFrom->d, sizeof(BN_ULONG)*pstFrom->top);
        pstTo->top = pstFrom->top;
        pstTo->dmax = pstFrom->dmax;
        pstTo->neg = pstFrom->neg;
        pstTo->flags = pstFrom->flags;
    }

    uint32 SerializeBigNum(const BIGNUM* pKey, byte* pBuf)
    {
        memset(pBuf, 0, sizeof(pBuf));

        byte* p = pBuf;

        *(int32*)p = pKey->dmax;
        p += sizeof(int32);

        memcpy(p, pKey->d, pKey->dmax * sizeof(BN_ULONG));
        p += pKey->dmax * sizeof(BN_ULONG);

        *(int32*)p = pKey->top;
        p += sizeof(int32);

        *(int32*)p = pKey->dmax;
        p += sizeof(int32);

        *(int32*)p = pKey->neg;
        p += sizeof(int32);

        *(int32*)p = pKey->flags;
        p += sizeof(int32);

        return (p-pBuf);
    }

    void ReadBigNum(BIGNUM*& pBigNum, byte*& p)
    {
        CnVerify(pBigNum == nullptr);

        pBigNum = NewBigNum();

        uint32 nBigNumSize = *(uint32*)p;
        p += sizeof(uint32);

        pBigNum->d = new BN_ULONG[nBigNumSize];
        memset(pBigNum->d, 0, sizeof(BN_ULONG) * nBigNumSize);


        memcpy(pBigNum->d, p, nBigNumSize * sizeof(BN_ULONG));
        p += nBigNumSize * sizeof(BN_ULONG);

        pBigNum->top = *(int32*)p;
        p += sizeof(int32);

        pBigNum->dmax = *(int32*)p;
        p += sizeof(int32);

        pBigNum->neg = *(int32*)p;
        p += sizeof(int32);

        pBigNum->flags = *(int32*)p;
        p += sizeof(int32);
    }

    uint32 PubEncrypt(byte* from, size_t nFromSize, byte* to, RSA* pPubKey)
    {
        uint32 nRSA_size = RSA_size(pPubKey);
        uint32 nResultBytes = 0;
        uint32 nRemain = (uint32)nFromSize;
        byte* pCurPos = from;

        do
        {
            uint32 nCurSrcSize = nRSA_size - 16 < nRemain ? nRSA_size-16 : nRemain;
            uint32 nCurEnc = RSA_public_encrypt(nCurSrcSize, pCurPos, to+nResultBytes, pPubKey, RSA_PKCS1_PADDING);
            pCurPos += nCurSrcSize;
            nRemain -= nCurSrcSize;
            nResultBytes += nCurEnc;
        }
        while(nRemain > 0);

        return nResultBytes;


        //	int32 nStep1 = RSA_public_encrypt(nRSA_size, from, to, pPubKey, RSA_NO_PADDING);
        //	int32 nStep2 = RSA_public_encrypt(nRSA_size, from + nStep1, to + nStep1, pPubKey, RSA_NO_PADDING);

    }
    uint32 PriDecrypt(byte* from, size_t nFromSize, byte* to, RSA* pPriKey)
    {
        uint32 nRSA_size = RSA_size(pPriKey);
        uint32 nResultBytes = 0;
        uint32 nRemain = (uint32)nFromSize;
        byte* pCurPos = from;

        do
        {
            uint32 nCurSrcSize = nRSA_size < nRemain ? nRSA_size : nRemain;
            uint32 nCurEnc = RSA_private_decrypt(nCurSrcSize, pCurPos, to+nResultBytes, pPriKey, RSA_PKCS1_PADDING);
            pCurPos += nCurSrcSize;
            nRemain -= nCurSrcSize;
            nResultBytes += nCurEnc;
        }
        while(nRemain > 0);

        return nResultBytes;
    }

	void FreeCrypto()
	{
		CRYPTO_cleanup_all_ex_data() ;
	}
}
#endif // #if SDU_WITH_OPENSSL 
