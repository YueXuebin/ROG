//! @file packet_splitter.h


#ifndef _PACKET_SPLITTER_H_
#define _PACKET_SPLITTER_H_




//! @class Packet_Splitter
//! @brief tcp拆包接口
class Packet_Splitter
{
public:
    Packet_Splitter();
	virtual ~Packet_Splitter();

	//! 拆包
	//! @param buf 数据缓存
	//! @param len 数据长度
	//! @param packet_begin 返回参数 包起始位置
	//! @param packet_len 返回参数 包长度
	//! @return -1:数据格式错误, 0:缓存中没有完整包, 1:得到一个包
	virtual int split(const char *buf, int len, int& packet_begin, int& packet_len) = 0;
};


#endif // _PACKET_SPLITTER_H_
