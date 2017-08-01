#ifndef _STRING_SPLITTER_H
#define _STRING_SPLITTER_H

#include "packet_splitter.h"


//! @class String_Splitter
//! @brief 文本行协议拆包器
//!
//! 每行文本为一个包, CR LF 字符已经去掉
class String_Splitter : public Packet_Splitter
{
public:
	virtual ~String_Splitter();

	//! 拆包
	//! @param buf 数据缓存
	//! @param len 数据长度
	//! @param packet_begin 返回参数 包起始位置
	//! @param packet_len 返回参数 包长度
	//! @return -1:数据格式错误, 0:缓存中没有完整包, 1:得到一个包
	virtual int split(const char *buf, int len, int& packet_begin, int& packet_len);
};

#endif  /* _STRING_SPLITTER_H */
