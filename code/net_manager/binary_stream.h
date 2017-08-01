
//=============================================================================
/**
 *  @file binary_stream.h
 *  本程序实现一个二进制流读取类和写入类
 */
//=============================================================================


#ifndef _BINARY_STREAM_H_
#define _BINARY_STREAM_H_


#include "stream_types.h"
#include "os.h"
#include <string.h>


//! 空包头(对于某些没有包头的协议使用此包头)
class NULL_HEAD;


//! @class Binary_Read_Stream
//! @brief 二进制读取流
template<typename HEAD = NULL_HEAD>
class Binary_Read_Stream
{
public:
	//! 构造函数
	//! @param buf 缓冲区指针
	//! @param len 数据长度
	Binary_Read_Stream(const char *buf, int len);

	//! 析构函数
	~Binary_Read_Stream();

public:
	//! 取包头
	//! @return 包头引用
	HEAD& head();

public:
	//! 读取uint8
	//! @param data 读取数据
	//! @return 二进制读取流
	Binary_Read_Stream& operator >>(uint8_t &data);

	//! 读取uint16(转为网络字节序)
	//! @param data 读取数据
	//! @return 二进制读取流
	Binary_Read_Stream& operator >>(uint16_t &data);

	//! 读取uint32(转为网络字节序)
	//! @param data 读取数据
	//! @return 二进制读取流
	Binary_Read_Stream& operator >>(uint32_t &data);

	//! 读取uint64(转为网络字节序)
	//! @param data 读取数据
	//! @return 二进制读取流
	Binary_Read_Stream& operator >>(uint64_t &data);

	//! 读取字符串
	//! @param data 读取数据
	//! @return 二进制读取流
	Binary_Read_Stream& operator >>(String &data);

	//! 读取字符串
	//! @param data 读取数据
	//! @return 二进制读取流
	//Binary_Read_Stream& operator >>(const String &data);

	//! 读取数据块
	//! @param data 读取数据
	//! @return 二进制读取流
	Binary_Read_Stream& operator >>(Binary &data);

public:
	//! 取流状态
	//! @return 流状态
	bool good() const;

	//! 取流结束标志
	//! @return 流结束标志
	bool eof() const;

public:
	//! 读取uint8
	uint8_t read_uint8();

	//! 读取uint16
	uint16_t read_uint16();

	//! 读取uint32
	uint32_t read_uint32();

	//! 读取String
	String read_string();

	//! 读取Binary
	Binary read_binary();

private:
	const char *m_buf;
	int m_len;
	int m_pos;
	bool m_good;
	bool m_eof;
};

//! 特化无包头版本的构造函数
//! @param buf 缓冲区指针
//! @param len 数据长度
template<>
Binary_Read_Stream<NULL_HEAD>::Binary_Read_Stream(const char *buf, int len);




template<typename HEAD>
class Binary_Write_Stream;


//! @fn Binary_Write_Stream<HEAD>& Binary_Begin(Binary_Write_Stream<HEAD>& write_stream)
//! @brief 二进制写入流操作符, 开始一个数据块
template<typename HEAD>
Binary_Write_Stream<HEAD>& Binary_Begin(Binary_Write_Stream<HEAD>& write_stream);


//! @fn Binary_Write_Stream<HEAD>& Binary_End(Binary_Write_Stream<HEAD>& write_stream)
//! @brief 二进制写入流操作符, 结束一个数据块
template<typename HEAD>
Binary_Write_Stream<HEAD>& Binary_End(Binary_Write_Stream<HEAD>& write_stream);


//! @class Binary_Write_Stream
//! @brief 二进制写入流
template<typename HEAD = NULL_HEAD>
class Binary_Write_Stream
{
public:
	//! 构造函数
	//! @param buf 缓冲区指针
	//! @param len 缓冲区长度
	Binary_Write_Stream(char *buf, int len);
	
	Binary_Write_Stream();

	//! 析构函数
	~Binary_Write_Stream();

public:
	//! 流操作符友元, 开始一个数据块
	friend Binary_Write_Stream& Binary_Begin<>(Binary_Write_Stream& write_stream);

	//! 流操作符友元, 结束一个数据块
	friend Binary_Write_Stream& Binary_End<>(Binary_Write_Stream& write_stream);

public:
	//! 取包头
	//! @return 包头引用
	HEAD& head();
	
	void reset();
	void detach();
	void attach(char *buf, int len);

public:

	//! 写入uint8
	//! @param data 写入数据
	//! @return 二进制写入流
	Binary_Write_Stream& operator <<(uint8_t data);

	//! 写入uint16(转为本地字节序)
	//! @param data 写入数据
	//! @return 二进制写入流
	Binary_Write_Stream& operator <<(uint16_t data);

	//! 写入uint32(转为本地字节序)
	//! @param data 写入数据
	//! @return 二进制写入流
	Binary_Write_Stream& operator <<(uint32_t data);

	//! 写入uint64(转为本地字节序)
	//! @param data 写入数据
	//! @return 二进制写入流
	Binary_Write_Stream& operator <<(uint64_t data);

	//! 写入字符串
	//! @param data 写入数据
	//! @return 二进制写入流
	Binary_Write_Stream& operator <<(const String& data);

	//! 写入字符串
	//! @param data 写入数据
	//! @return 二进制写入流
	Binary_Write_Stream& operator <<(const char *data);

	//! 写入数据块
	//! @param data 写入数据
	//! @return 二进制写入流
	Binary_Write_Stream& operator <<(const Binary& data);

	//! 执行流操作符
	//! @param pfn 流操作符函数指针
	//! @return 二进制写入流
	Binary_Write_Stream& operator <<(Binary_Write_Stream& (*pfn)(Binary_Write_Stream&));

public:
	//! 取流状态
	//! @return 流状态
	bool eof() const;

	//! 取流结束标志
	//! @return 流结束标志
	bool good() const;

	//! 取已写入的数据长度
	//! @return 数据长度
	int length() const;

public:
	//! 写入uint8
	void write_uint8(uint8_t data);

	//! 写入uint16
	void write_uint16(uint16_t data);

	//! 写入uint32
	void write_uint32(uint32_t data);

	//! 写入string
	void write_string(const String& data);

	//! 开始二进制数据块
	void begin_binary();

	//! 结束二进制数据块
	void end_binary();

	//! 写入二进制数据块
	void write_binary(const Binary& data);
	//! 写入不带长度的数据块
	void write_raw(char* data, int len);


private:
	char *m_buf;
	int m_len;
	int m_pos;
	bool m_good;
	bool m_eof;
	bool m_begin_binary;
	int m_binary_len_pos;
	uint16_t m_binary_len;
};

//! 特化无包头版本构造函数
//! @param buf 缓冲区指针
//! @param len 缓冲区长度
template<>
Binary_Write_Stream<NULL_HEAD>::Binary_Write_Stream(char *buf, int len);

template<>
void Binary_Write_Stream<NULL_HEAD>::reset();
template<>
void Binary_Write_Stream<NULL_HEAD>::detach();
template<>
void Binary_Write_Stream<NULL_HEAD>::attach(char *buf, int len);


#include "binary_stream.inl"	// 模板实现代码


#endif // _BINARY_STREAM_H_
