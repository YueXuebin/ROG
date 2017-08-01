#include "net_manager_pch.h"
#include "afpacket_splitter.h"
#include "os.h"
#include "config.h"


// class Packet_Splitter
AfPacket_Splitter::AfPacket_Splitter(PACKLEN_TYPE packlentype)
: m_packlentype(packlentype)
{

}

AfPacket_Splitter::~AfPacket_Splitter()
{

}

int AfPacket_Splitter::split(const char *buf, int len, int& packet_begin, int& packet_len)
{
    // 检查是否已经包含头部的长度
    uint8_t nMinLen = m_packlentype == PACKLENTYPE_DWORD ? (sizeof(uint32_t) + sizeof(uint32_t)): (sizeof(uint32_t) + sizeof(uint16_t));
    if (len < nMinLen)
    {
        return 0;
    }

    // 检查包的起始验证码
    if(ntohs(*(uint16_t*)buf) != START_CHECK_CODE)
    {
        return -2;
    }

    // 获得包的长度
    int pack_len = m_packlentype == PACKLENTYPE_DWORD ? ntohl(*(uint32_t*)(buf + sizeof(uint32_t))) : ntohs(*(uint16_t*)(buf + sizeof(uint32_t)));
    if (pack_len < nMinLen) 
    {
        return -1;
    }

    if (len >= pack_len) 
    { // 已经有了一个完整的包
        // 检查包的结束验证码
        if(ntohs(*(uint16_t*)(buf + pack_len - sizeof(uint16_t))) != END_CHECK_CODE)
        {
            return -2;
        }

        packet_begin = 0;
        packet_len = pack_len;
        return 1;       // 获得一个数据包
    }

    if(len < pack_len) // 缓冲区中的数据长度，不足一个包的长度
    {
        packet_begin = 0;
        packet_len = pack_len;
    }

    return 0;
}
