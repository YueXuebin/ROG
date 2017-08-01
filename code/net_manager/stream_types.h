//! @file stream_types.h


#ifndef _STREAM_TYPES_H_
#define _STREAM_TYPES_H_



//! @class String
//! @brief 字符串
class String
{
public:
	//! 构造函数
	String();

	//! 构造函数
	//! @param buf 字符串指针
	//! @param len 长度
	//!
	//! 写入流时 将buf内的字符串写入流中, 同时会写入结束0
	//! 读取流时 如果buf为NULL 将buf指向流中的字符串,
	//!	         如果buf不为NULL并且len的长度足够 将流中的字符串拷贝到buf中
	String(const char *buf, int len);

public:
	const char *buf;	//!< 字符串指针
	int len;			//!< 字符串长度(不包括结束0)
};


//! @class Binary
//! @brief 二进制数据块
class Binary
{
public:
	//! 构造函数
	Binary();

	//! 构造函数
	//! @param buf 数据块指针
	//! @param len 长度
	//!
	//! 写入流时 将buf内的数据块写入流中
	//! 读取流时 将buf指向流中的数据块
	Binary(const char *buf, int len);

public:
	const char *buf;	//!< 数据块指针
	int len;			//!< 数据块长度
};

#endif // _STREAM_TYPES_H_

