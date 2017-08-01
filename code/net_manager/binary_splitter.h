//! @file binary_splitter.h


#ifndef _BINARY_SPLITTER_H_
#define _BINARY_SPLITTER_H_


#include "packet_splitter.h"

//! @class Binary_Splitter
//! @brief 二进制协议tcp拆包器
//!
//! 包前两字节为网络字节序的包长
class Binary_Splitter : public Packet_Splitter
{
public:
    explicit Binary_Splitter();

    virtual ~Binary_Splitter();

    //! 拆包
    //! @param buf 数据缓存
    //! @param len 数据长度
    //! @param packet_begin 返回参数 包起始位置
    //! @param packet_len 返回参数 包长度
    //! @return -1:数据格式错误, 0:缓存中没有完整包, 1:得到一个包
    virtual int split(const char *buf, int len, int& packet_begin, int& packet_len);
};


#endif // _BINARY_SPLITTER_H_
