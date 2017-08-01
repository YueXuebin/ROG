#include "SDuPCH.h"
#include "sdstrencrypt.h"

using namespace Crown;

//对每个字节高低4bit互换，与KEY进行异或运算
void SDAPI CSDStrEncrypt::Encode(std::string& sContent)
{
    Encode( (byte*)sContent.c_str(), sContent.size());
}

void SDAPI CSDStrEncrypt::Encode(byte* pBuf, int32 nLen)
{
    if(nLen > 2)
    {
        for (int i = 2; i < nLen; ++i)
        {
            pBuf[i] = (pBuf[i]<<4) | (pBuf[i]>>4);
            pBuf[i] ^= CODE_KEY;
        }
    }
}

void SDAPI CSDStrEncrypt::Decode(std::string& sContent)
{
    Decode((byte*)sContent.c_str(), sContent.size());
}

void SDAPI CSDStrEncrypt::Decode(byte* pBuf, int32 nLen)
{
    if(nLen > 2)
    {
        for (int i = 2; i < nLen; ++i)
        {
            pBuf[i] ^= CODE_KEY;
            pBuf[i] = (pBuf[i]<<4) | (pBuf[i]>>4);
        }
    }
}

