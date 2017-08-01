#include "SDuPCH.h"
#include "sdnetutils.h"
#include <string.h>
#include "sddatastream.h"
#include "sdstring.h"

namespace Crown
{

    CSDDataStream::CSDDataStream()
    {
        m_pBuf	= nullptr;
        m_bufLen = 0;
        m_pos	= 0;
    }

    CSDDataStream::~CSDDataStream()
    {
    }

    void SDAPI CSDDataStream::Prepare(char *pNetData, int32 iSize)
    {
        m_pBuf	= pNetData;
        m_bufLen	= iSize;
        m_pos	= 0;
    }

    void SDAPI CSDDataStream::Reset()
    {
        m_pos = 0;
    }

    int32 SDAPI CSDDataStream::AddByte(byte byByte)
    {
        if(m_pos + (int32)sizeof(byByte) > m_bufLen)
            return -1;

        *(byte*)(m_pBuf + m_pos) = byByte;
        m_pos += sizeof(byByte);

        return m_pos;
    }

    int32 SDAPI CSDDataStream::DelByte(byte &byByte)
    {
        if(m_pos + (int32)sizeof(byByte) > m_bufLen)
            return -1;

        byByte = *(byte*)(m_pBuf + m_pos);
        m_pos += sizeof(byByte);

        return m_pos;
        return -1;
    }

    int32 SDAPI CSDDataStream::AddChar(char chChar)
    {
        if(m_pos + (int32)sizeof(chChar) > m_bufLen)
            return -1;

        *(char*)(m_pBuf + m_pos) = chChar;
        m_pos += sizeof(chChar);

        return m_pos;
    }

    int32 SDAPI CSDDataStream::DelChar(char &chChar)
    {
        if(m_pos + (int32)sizeof(chChar) > m_bufLen)
            return -1;

        chChar = *(char*)(m_pBuf + m_pos);
        m_pos += sizeof(chChar);

        return m_pos;
    }

    int32 SDAPI CSDDataStream::AddWord(uint16 wWord)
    {
        if(m_pos + (int32)sizeof(wWord) > m_bufLen)
            return -1;

        *(uint16*)(m_pBuf + m_pos) = htons(wWord);
        m_pos += sizeof(wWord);

        return m_pos;
    }

    int32 SDAPI CSDDataStream::DelWord(uint16 &wWord)
    {
        if(m_pos + (int32)sizeof(wWord) > m_bufLen)
            return -1;

        wWord = ntohs(*(uint16*)(m_pBuf + m_pos));
        m_pos += sizeof(wWord);

        return m_pos;
    }

    int32 SDAPI CSDDataStream::AddShort(int16 shShort)
    {
        if(m_pos + (int32)sizeof(shShort) > m_bufLen)
            return -1;

        *(int16*)(m_pBuf + m_pos) = htons(shShort);
        m_pos += sizeof(shShort);

        return m_pos;
    }

    int32 SDAPI CSDDataStream::DelShort(int16 &shShort)
    {
        if(m_pos + (int32)sizeof(shShort) > m_bufLen)
            return -1;

        shShort = ntohs(*(int16*)(m_pBuf + m_pos));
        m_pos += sizeof(shShort);

        return m_pos;
    }

    int32 SDAPI CSDDataStream::AddDword(uint32 dwDword)
    {
        if(m_pos + (int32)sizeof(dwDword) > m_bufLen)
            return -1;

        *(uint32*)(m_pBuf + m_pos) = htonl(dwDword);
        m_pos += sizeof(dwDword);

        return m_pos;
    }

    int32 SDAPI CSDDataStream::DelDword(uint32 &dwDword)
    {
        if(m_pos + (int32)sizeof(dwDword) > m_bufLen)
            return -1;

        dwDword = ntohl(*(uint32*)(m_pBuf + m_pos));
        m_pos += sizeof(dwDword);

        return m_pos;
    }

    int32 SDAPI CSDDataStream::AddInt(int32 iInt)
    {
        if(m_pos + (int32)sizeof(iInt) > m_bufLen)
            return -1;

        *(int32*)(m_pBuf + m_pos) = htonl(iInt);
        m_pos += sizeof(iInt);

        return m_pos;
    }

    int32 SDAPI CSDDataStream::DelInt(int32 &iInt)
    {
        if(m_pos + (int32)sizeof(iInt) > m_bufLen)
            return -1;

        iInt = ntohl(*(int32*)(m_pBuf + m_pos));
        m_pos += sizeof(iInt);

        return m_pos;
    }

    int32 SDAPI CSDDataStream::AddUint64(uint64 qwUint64)
    {
        if(m_pos + (int32)sizeof(qwUint64) > m_bufLen)
            return -1;

        *(uint64*)(m_pBuf + m_pos) = SDHtonll(qwUint64);
        m_pos += sizeof(qwUint64);

        return m_pos;
    }

    int32 SDAPI CSDDataStream::DelUint64(uint64& qwUint64)
    {
        if(m_pos + (int32)sizeof(qwUint64) > m_bufLen)
            return -1;

        qwUint64 = SDNtohll(*(uint64*)(m_pBuf + m_pos));
        m_pos += sizeof(qwUint64);

        return m_pos;
    }

    int32 SDAPI CSDDataStream::AddInt64(int64 llInt64)
    {
        if(m_pos + (int32)sizeof(llInt64) > m_bufLen)
            return -1;

        *(int64*)(m_pBuf + m_pos) = SDHtonll(llInt64);
        m_pos += sizeof(llInt64);

        return m_pos;
    }

    int32 SDAPI CSDDataStream::DelInt64(int64 &llInt64)
    {
        if(m_pos + (int32)sizeof(llInt64) > m_bufLen)
            return -1;

        llInt64 = SDNtohll(*(int64*)(m_pBuf + m_pos));
        m_pos += sizeof(llInt64);

        return m_pos;
    }

    int32 SDAPI CSDDataStream::AddFloat(real fFloat)//08-10-06 liujunhui add: 增加解析real
    {
        if(m_pos + (int32)sizeof(fFloat) > m_bufLen)
            return -1;

        *(real*)(m_pBuf + m_pos) = fFloat;
        m_pos += sizeof(fFloat);

        return m_pos;
    }

    int32 SDAPI CSDDataStream::DelFloat(real &fFloat)//08-10-06 liujunhui add: 增加解析real
    {
        if(m_pos + (int32)sizeof(fFloat) > m_bufLen)
            return -1;

        fFloat = (*(real*)(m_pBuf + m_pos));
        m_pos += sizeof(fFloat);

        return m_pos;
    }


    int32 SDAPI CSDDataStream::AddDouble(double dbDouble )
    {
        if(m_pos + (int32)sizeof(dbDouble) > m_bufLen)
            return -1;

        *(double*)(m_pBuf + m_pos) = dbDouble;
        m_pos += sizeof(dbDouble);

        return m_pos;
    }

    int32 SDAPI CSDDataStream::DelDouble(double &dbDouble )
    {
        if(m_pos + (int32)sizeof(dbDouble) > m_bufLen)
            return -1;

        dbDouble = (*(double*)(m_pBuf + m_pos));
        m_pos += sizeof(dbDouble);

        return m_pos;
    }

    int32 SDAPI CSDDataStream::AddString(const char *pszString, int32 iSize)
    {
        int32 iLen = SDStrnlen(pszString, iSize-1);

        if(m_pos + (int32)sizeof(uint16) + iLen > m_bufLen)
            return -1;

        if(-1 == AddWord(iLen))
            return -1;

        memcpy(m_pBuf + m_pos, pszString, iLen);
        m_pos += iLen;

        return m_pos;
    }

    int32 SDAPI CSDDataStream::DelString(char *pszOut, int32 iSize)
    {
        uint16 wLen = 0;
        if(-1 == DelWord(wLen))
            return -1;

        if(m_pos + (int32)wLen > m_bufLen)
            return -1;

        if( int32(wLen + 1) > iSize )
            return -1;

        memcpy(pszOut, m_pBuf+m_pos, wLen);
        pszOut[wLen] = '\0';
        m_pos += wLen;

        return m_pos;
    }


    int32 SDAPI CSDDataStream::AddBuf(const byte *pbyBuf, int32 iSize )
    {
        if (iSize < 0 || nullptr == pbyBuf)
        {
            return -1;
        }

        if(m_pos + iSize > m_bufLen)
            return -1;

        memcpy(m_pBuf + m_pos, pbyBuf, iSize);
        m_pos += iSize;

        return m_pos;
    }

    int32 SDAPI CSDDataStream::DelBuf(byte *pbyBuf, int32 iSize )
    {
        if (nullptr == pbyBuf)
        {
            return -1;
        }

        if(m_pos + iSize > m_bufLen)
            return -1;

        memcpy(pbyBuf, m_pBuf+m_pos, iSize);

        m_pos += iSize;

        return m_pos;
    }

};
