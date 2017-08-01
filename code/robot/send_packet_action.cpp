#include "robot_pch.h"
#include "robot.h"
#include "net_packet.h"
#include "send_packet_action.h"


Send_Packet_Action::Send_Packet_Action( Robot* pRobot, int32 msgId, int32 mode) :
ActionBase(pRobot)
{
    m_Name = "SendPacket";
    m_msgId = msgId;
    m_msgMode = mode;
}

Send_Packet_Action::~Send_Packet_Action( void )
{

}

uint16 Send_Packet_Action::CRC16(const char* ba, uint32 len)
{
    CRC crc;
    crc.crc16 = 0xffff;
    for (uint32 i=0; i<len; i++)
    {
        uint8 ch = ba[i];
        crc.by[0] = crc.by[0] ^ ch;
        for(uint32 l=0; l<8; l++)
        {
            if (crc.by[0] & 0x01)
            {
                crc.crc16 = crc.crc16 >> 1;
                crc.crc16 = crc.crc16 ^ 0xa001;
            }
            else
            {
                crc.crc16 = crc.crc16 >> 1;
            }
        }
    }
    unsigned char swap = crc.by[0];
    crc.by[0] = crc.by[1];
    crc.by[1] = swap;
    return crc.crc16;
}

void Send_Packet_Action::OnStart()
{
    ActionBase::OnStart();

    Net_Packet* packet = new Net_Packet;
    MakePacket(packet, m_msgId);

    if (!m_Robot->SendPacket(packet))
    {
        CnDbg("Action: %s, Msg: %d, Failed\n", m_Name.c_str(), m_msgId);
        delete packet;
    }
    else
    {
        //CnDbg("Action: %s, Msg: %d,  Success\n", m_Name.c_str(), m_msgId);
    }

    End();
}

void Send_Packet_Action::OnRecv( uint32 msgID, void* data )
{
    ActionBase::OnRecv(msgID, data);

    switch(msgID)
    {
    case  MSG_INST_ACHIVE_ACK:
        {
            break;
        }
    default:
        break;
    }
}

void Send_Packet_Action::MakePacket(Net_Packet* packet, int32 msgId)
{
    packet->write_uint16(0x4744);  // DG
    packet->write_uint8(CMessageEncoder::m_TimeCounter);  // time stamp
    packet->write_uint8(0);  // reserve
    Bookmark bmLen = packet->getcurpos();
    packet->write_uint32(0);   // all packet length
    packet->write_uint64(0);
    packet->write_uint32(msgId);

    // write message
    if(m_msgMode == 0)
    {   // 全填1
        memset(crcBuf, 1, crcBufLen);
    }
    else
    {
        for(int32 i=0; i<crcBufLen/sizeof(uint32); i++)
        {
            uint32 v = Crown::SDRandom(0xffffffff);
            memset(crcBuf+sizeof(uint32)*i, sizeof(uint32), v);
        }
    }

    packet->write_data(crcBuf, 200000);

    // CRC
    int32 msg_len = packet->getdatalen() - NET_MSG_HEADER_LEN;
    if(msg_len > crcBufLen)
    {
        CnAssert(false);
        return;
    }
    packet->movetobegin();
    packet->move_rptr(NET_MSG_HEADER_LEN);
    memset(crcBuf, 0, crcBufLen);
    packet->readdata(crcBuf, msg_len);
    uint16 crc = CRC16(crcBuf, msg_len);
    packet->write_uint16(crc);

    // check tail
    packet->write_uint16(0x4641);  // AF
    packet->write_uint32(packet->getdatalen(), bmLen);
}
