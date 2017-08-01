//! @file afpacket_splitter.h


#ifndef _AFPACKET_SPLITTER_H_
#define _AFPACKET_SPLITTER_H_

#include "packet_splitter.h"

#define START_CHECK_CODE 0x4744     ///< "DG"
#define END_CHECK_CODE 0x4641       ///< "AF"

//! @class Packet_Splitter
//! @brief tcp拆包接口
class AfPacket_Splitter : public Packet_Splitter
{
public:
    enum PACKLEN_TYPE {PACKLENTYPE_WORD, PACKLENTYPE_DWORD};

    explicit AfPacket_Splitter(PACKLEN_TYPE packlentype = PACKLENTYPE_DWORD);

    virtual ~AfPacket_Splitter();

    //! 拆包
    //! @param buf 数据缓存
    //! @param len 数据长度
    //! @param packet_begin 返回参数 包起始位置
    //! @param packet_len 返回参数 包长度
    //! @return -1:数据格式错误, 0:缓存中没有完整包, 1:得到一个包
    virtual int split(const char *buf, int len, int& packet_begin, int& packet_len);

private:
    PACKLEN_TYPE m_packlentype;     // 是用word还是dword来表示包长
};


#endif // _AFPACKET_SPLITTER_H_
