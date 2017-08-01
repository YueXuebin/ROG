#pragma once

#ifndef CODER_H_INCLUDE
#define CODER_H_INCLUDE

#include <string>
#include <vector>

#ifndef SWIG
#include "sdu.h"
#endif

#ifdef WIN32
#include <WinSock2.h>
#elif defined(LINUX) || defined(APPLE)
#include <arpa/inet.h>
#else
#pragma message("unknown system")
#endif

#define NET_DATA_TEXT_LEN 8192

inline unsigned long long htonll(unsigned long long number)
{
    return ( htonl( (unsigned long)((number >> 32) & 0xFFFFFFFF)) |
        ((unsigned long long) (htonl((unsigned long)(number & 0xFFFFFFFF)))  << 32));
}

inline unsigned long long ntohll(unsigned long long number)
{
    return ( ntohl( (unsigned long)((number >> 32) & 0xFFFFFFFF) ) |
        ((unsigned long long) (ntohl((unsigned long)(number & 0xFFFFFFFF)))  << 32));
}

class CNetData
{
public:
    CNetData();
    virtual ~CNetData();

    void Prepare(char *pNetData, int iSize);
    void Reset();

    char* GetData(){ return m_pBuf; }
    int GetDataLen(){ return m_iPos; }

    int AddByte(unsigned char byByte);
    int DelByte(unsigned char &byByte);

    int AddChar(char chChar);
    int DelChar(char &chChar);

    int AddWord(unsigned short wWord);
    int DelWord(unsigned short &wWord);

    int AddShort(short shShort);
    int DelShort(short &shShort);

    int AddDword(unsigned int dwDword);
    int DelDword(unsigned int &dwDword);
    int SetDword(int pos, unsigned int dwDword);

    int AddInt(int iInt);
    int DelInt(int &iInt);

    int AddUint64(unsigned long long qwUint64);
    int DelUint64(unsigned long long& qwUint64);

    int AddInt64(long long llInt64);
    int DelInt64(long long &llInt64);

    int AddFloat(float fFloat); 
    int DelFloat(float &fFloat);

    // 这两个没有实现
#ifndef SWIG
    int AddDouble(double dbDouble);
    int DelDouble(double &dbDouble);
#endif

    int AddString(const char *pszString, int iSize);
    int DelString(char *pszOut, int iSize);

    int AddString(const std::string& str);
    int DelString(std::string& str);

    int Strnlen(const char *pszString, int iSize);

    int AddBuf(const unsigned char* pbyBuf, int iSize);
    int DelBuf(unsigned char* pbyBuf, int iSize);

protected:
    static char m_text[NET_DATA_TEXT_LEN];
    char*  m_pBuf;
    int    m_iSize;
    int    m_iPos;
};

class CSDProtocol 
{
public:
    virtual ~CSDProtocol() {}

    virtual int         Encode(uint32 msgID, void* data) = 0;
    virtual int         Encode(uint32 sessionID, uint32 msgID, void* data) = 0;
    virtual int         Decode(uint32 sessionID, uint32 msgID, char *pNet, int iNetSize) = 0;

    std::string         GetName ();
    void                SetName(const std::string strName);

    std::vector<int>*   GetMsgList();
private:
    std::string         m_strName ;
};
#endif
