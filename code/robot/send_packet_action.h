#ifndef send_packet_action_h
#define send_packet_action_h

#include "action_base.h"


//typedef union _CRC
//{
//    uint16 crc16;
//    uint8 by[2];
//} CRC;

class Send_Packet_Action : public ActionBase
{
public:
    Send_Packet_Action(Robot* pRobot, int32 msgId, int32 mode);
    virtual ~Send_Packet_Action(void);

    uint16 CRC16(const char* ba, uint32 len);

    virtual void OnStart();
    virtual void OnRecv(uint32 msgID, void* data);

    void MakePacket(Net_Packet* packet, int32 msgId);
private:
    static const uint32 crcBufLen = 204800; // 200K
    char crcBuf[crcBufLen];

    int32 m_msgId;
    int32 m_msgMode;            // 协议模式(0:全1, 1:全随机)
};


#endif
