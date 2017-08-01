#include "gateserver_pch.h"
#include "player.h"
#include "af_encrypt.h"
#include "config_loader.h"
#include "config.h"
#include "zlib.h"
#include "net_packet.h"


// ==================================================
// 角色构造
// ==================================================
Player::Player()
{
    Clear();
    m_encrypt = CnNew AfEncrypt();

    m_SendCount = m_RecvCount = 0;
    m_RecvCountPerSecond = 0;
    m_SecondTimer = 0;
    m_bCompress = false;
}

// ==================================================
// 角色析构
// ==================================================
Player::~Player()
{
    Clear();
    CnDelete m_encrypt;
    m_encrypt = 0;
}

void Player::Update()
{
    m_SecondTimer += g_ServerUpdateTime;
    if(m_SecondTimer < 1000)
        return;

    m_SecondTimer -= g_ServerUpdateTime;

    m_RecvCountPerSecond = 0;
}

// ==================================================
// 加密数据包
// ==================================================
void Player::EncryptPacket(Net_Packet* packet)
{
    if(ConfigLoader::Instance()->GetEncrypt())
    {
        m_encrypt->EncodeByPubKey(packet);
    }
}

// ==================================================
// 解密数据包
// ==================================================
void Player::DecryptPacket(Net_Packet* packet)
{
    if(ConfigLoader::Instance()->GetEncrypt())
    {
        m_encrypt->DecodeByPriKey(packet);
    }
}

bool Player::CompressPacket(Net_Packet* packet)
{
    if (!CanCompress())
        return true;

    static const uint32 CompressLimitSize = 256;
    static const uint32 CompressBuffCapacity = 204800; //200k
    static char srcCompressBuff[CompressBuffCapacity];
    static char desCompressBuff[CompressBuffCapacity];

    packet->movetobegin();
    uint16 markcode = packet->read_uint16();        // DG
    uint8 timeStamp = packet->read_uint8();         // 时间戳
    uint8 countCmprsFlag = packet->read_uint8();    // 计数器
    uint32 dataLen = packet->read_uint32();         // 数据包长(整包)
    uint64 playerId = packet->read_uint64();        // PlayerID
    uint32 msgId = packet->read_uint32();           // msgID

    if (dataLen > CompressBuffCapacity)
    {
        CnAssert(false);
        return false;
    }

    unsigned long srcMsgLen = dataLen - (NET_MSG_HEADER_LEN + 2 + 2);
    unsigned long desMsgLen = CompressBuffCapacity;

    if (srcMsgLen < CompressLimitSize) //不用压缩
        return true;

    packet->readdata(srcCompressBuff, srcMsgLen);

    int ret = compress((unsigned char*)desCompressBuff, &desMsgLen, (unsigned char*)srcCompressBuff, srcMsgLen);
    if (ret != Z_OK)
    {
        CnWarn("compress packet error.code:%d\n", ret);
        return false;
    }

    packet->clear();
    packet->movetobegin();
    packet->write_uint16(markcode);  // DG
    packet->write_uint8(timeStamp);  // time stamp
    packet->write_uint8(countCmprsFlag |= 0x80);  // 7bit计数器 1bit是否压缩
    packet->write_uint32(NET_MSG_HEADER_LEN + desMsgLen + 2 + 2);   // all packet length
    packet->write_uint64(playerId);
    packet->write_uint32(msgId);

    packet->write_data(desCompressBuff, desMsgLen);
    uint16 crc = CRC16(desCompressBuff, desMsgLen);
    packet->write_uint16(crc);
    packet->write_uint16(0x4641);
    packet->crunch();
    return true;
}

// ==================================================
// 设置加密私匙
// ==================================================
void Player::SetEncryptPrivateKey(uint32 val)
{
    m_encrypt->SetPrivateKey(val);
}

// ==================================================
// 获得加密私匙
// ==================================================
uint32 Player::GetEncryptPrivateKey() const
{
    return m_encrypt->GetPrivateKey();
}

// ==================================================
// 设置加密公匙
// ==================================================
void Player::SetEncryptPublicKey(uint32 val)
{
    m_encrypt->SetPublicKey(val);
}

// ==================================================
// 获得加密公匙
// ==================================================
uint32 Player::GetEncryptPublicKey() const
{
    return m_encrypt->GetPublicKey();
}

// ==================================================
// 发送私有Key 给客户端
// ==================================================
void Player::SendEncryptPublicKeyNtf()
{
    ENCRYPT_TOKEN sendMsg;
    sendMsg.key = m_encrypt->GetPublicKey() ^ ENCRYPT_MAGIC_NUMBER;
    Send2Client(MSG_ENCRYPT_PUBLIC_KEY_NTF, &sendMsg);
}

// ==================================================
// 发送数据给客户端
// ==================================================
bool Player::Send2Client(uint32 msgID, void* data)
{
    if(TransferSend(m_sessionID, msgID, data, m_playerID))
    {
        return false;
    }

    return true;
}

void Player::OnSendMessage(uint32 msgID)
{
    m_SendCount++;
}

void Player::OnRecvMessage(uint32 msgID)
{
    m_RecvCount++;
    m_RecvCountPerSecond++;
}
