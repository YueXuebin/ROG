#include "transfer_pch.h"
#include "af_encrypt.h"
#include "net_packet.h"
#include "message_encoder.h"

AfEncrypt::AfEncrypt()
{
    memset(m_private_key_byte, 0, sizeof(m_private_key_byte));
    m_private_key_dword = 0;

    memset(m_public_key_byte, 0, sizeof(m_public_key_byte));
    m_public_key_dword = 0;
}

AfEncrypt::~AfEncrypt()
{
    
}

void AfEncrypt::SetPrivateKey(uint32_t key)
{
    m_private_key_byte[0] = key & 0xFF;
    m_private_key_byte[1] = (key >> 8) & 0xFF;
    m_private_key_byte[2] = (key >> 16) & 0xFF;
    m_private_key_byte[3] = (key >> 24) & 0xFF;

    m_private_key_dword = key;
}

void AfEncrypt::SetPublicKey(uint32_t key)
{
    m_public_key_byte[0] = key & 0xFF;
    m_public_key_byte[1] = (key >> 8) & 0xFF;
    m_public_key_byte[2] = (key >> 16) & 0xFF;
    m_public_key_byte[3] = (key >> 24) & 0xFF;

    m_public_key_dword = key;
}

void AfEncrypt::EncodeByPubKey(Net_Packet* packet)
{
    Encode(packet, m_public_key_byte);
}

void AfEncrypt::DecodeByPriKey(Net_Packet* packet)
{
    Decode(packet, m_private_key_byte);
}

void AfEncrypt::EncodeByPriKey( Net_Packet* packet )
{
    Encode(packet, m_private_key_byte);
}

void AfEncrypt::DecodeByPubKey(Net_Packet* packet)
{
    Decode(packet, m_public_key_byte);
}

void AfEncrypt::Encode(Net_Packet* packet, uint8 keys[4])
{
    if(!packet) return;

    // | 2byte DG | 2byte time | 4byte PacketLen | 8byte PlayerID | 4byte MsgID | 2byte AF |
    if(packet->get_datalength() <= (NET_MSG_HEADER_LEN+2))
        return;

    list_head* pos;
    list_head* head = packet->get_bufhead();

    // 提取时间戳
    Net_Buff1* pFirstBuf = list_entry(head->next, Net_Buff1, item);
    uint8 timeKey = pFirstBuf->m_packet[2];
    uint8 timeKeys[4];
    timeKeys[0] = (keys[0] + timeKey) & 0xff;
    timeKeys[1] = (keys[1] + timeKey) & 0xff;
    timeKeys[2] = (keys[2] + timeKey) & 0xff;
    timeKeys[3] = (keys[3] + timeKey) & 0xff;

    Net_Buff1* pNetBuf = NULL;

    // 去掉末尾的 | 2byte AF |
    uint32_t nLeftLen = packet->get_datalength() - 2;
    uint32_t nReadLen = 0;
    uint32_t nBuffLen = 0;
    uint32_t nTokenIdx = 0;

    list_for_each(pos, head)
    {
        pNetBuf = list_entry(pos, Net_Buff1, item);

        // 跳过消息头 | 2byte DG | 2byte time | 4byte PacketLen | 8byte PlayerID | 4byte MsgID |
        if(nReadLen < NET_MSG_HEADER_LEN)
        {
            nLeftLen -= NET_MSG_HEADER_LEN;
            nReadLen = NET_MSG_HEADER_LEN;
            nBuffLen = NET_MSG_HEADER_LEN;
        }
        else
        {
            nBuffLen = 0;
        }

        for(; (nBuffLen < MAX_PACKET_LENGTH) && (nLeftLen > 0); ++nBuffLen, --nLeftLen)
        {
            pNetBuf->m_packet[nBuffLen] ^= timeKeys[nTokenIdx];
            nTokenIdx = (nTokenIdx + 1) % 4;
        }

        if(nLeftLen <= 0)
            break;
    }
}

void AfEncrypt::Decode(Net_Packet* packet, uint8 keys[4])
{
    if(!packet) return;

    // | 2byte DG | 2byte time | 4byte PacketLen | 8byte PlayerID | 4byte MsgID | 2byte AF |
    if(packet->get_datalength() <= (NET_MSG_HEADER_LEN+2))
        return;

    list_head* pos;
    list_head* head = packet->get_bufhead();

    // 提取时间戳
    Net_Buff1* pFirstBuf = list_entry(head->next, Net_Buff1, item);
    uint8 timeKey = pFirstBuf->m_packet[2];
    uint8 timeKeys[4];
    timeKeys[0] = (keys[0] + timeKey) & 0xff;
    timeKeys[1] = (keys[1] + timeKey) & 0xff;
    timeKeys[2] = (keys[2] + timeKey) & 0xff;
    timeKeys[3] = (keys[3] + timeKey) & 0xff;

    Net_Buff1* pNetBuf = NULL;

    // 去掉末尾的 | 2byte AF |
    uint32_t nLeftLen = packet->get_datalength() - 2;
    uint32_t nReadLen = 0;
    uint32_t nBuffLen = 0;
    uint32_t nTokenIdx = 0;

    list_for_each(pos, head)
    {
        pNetBuf = list_entry(pos, Net_Buff1, item);

        // 跳过消息头 | 2byte DG | 2byte time | 4byte PacketLen | 8byte PlayerID | 4byte MsgID |
        if(nReadLen < NET_MSG_HEADER_LEN)
        {
            nLeftLen -= NET_MSG_HEADER_LEN;
            nReadLen = NET_MSG_HEADER_LEN;
            nBuffLen = NET_MSG_HEADER_LEN;
        }
        else
        {
            nBuffLen = 0;
        }

        for(; (nBuffLen < MAX_PACKET_LENGTH) && (nLeftLen > 0); ++nBuffLen, --nLeftLen)
        {
            pNetBuf->m_packet[nBuffLen] ^= timeKeys[nTokenIdx];
            nTokenIdx = (nTokenIdx + 1) % 4;
        }

        if(nLeftLen <= 0)
            break;
    }
}