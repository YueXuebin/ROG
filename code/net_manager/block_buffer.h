//! @file block_buffer.h


#ifndef _BLOCK_BUFFER_H_
#define _BLOCK_BUFFER_H_




#include "log.h"


//! @class Block_Buffer_T
//! @brief 数据块缓冲
//!
//! 模板参数 BUFFER_LENGTH 是缓冲区长度
template <int BUFFER_LENGTH>
class Block_Buffer_T
{
public:
	Block_Buffer_T();
	~Block_Buffer_T();

	//! 取读指针
	char* rd_ptr();

	//! 读指针前移
	void rd_ptr(int n);

	//! 取写指针
	char* wr_ptr();

	//! 写指针前移
	void wr_ptr(int n);

	//! 缓存长度
	int buffer_length();

	//! 可用数据长度
	int data_length();

	//! 剩余空间长度
	int space_length();
	
	//! 实际长度
	int length();

	//! 回收空间
	void recycle();

	//! 扩展buffer空间
	void extend(int new_length);
	
private:
	//! 缓冲区
	char* m_buffer;

	//! 读位置
	int m_read_index;

	//! 写位置
	int m_write_index;

	//! 当前buffer实际大小
	int _length;
};


template <int BUFFER_LENGTH>
Block_Buffer_T<BUFFER_LENGTH>::Block_Buffer_T()
{
	m_read_index = 0;
	m_write_index = 0;
	m_buffer = CnNew char[BUFFER_LENGTH];
	_length = BUFFER_LENGTH;
}

template <int BUFFER_LENGTH>
Block_Buffer_T<BUFFER_LENGTH>::~Block_Buffer_T()
{
	CnDelete[] m_buffer;
}

template <int BUFFER_LENGTH>
char* Block_Buffer_T<BUFFER_LENGTH>::rd_ptr()
{
	return m_buffer + m_read_index;
}

template <int BUFFER_LENGTH>
void Block_Buffer_T<BUFFER_LENGTH>::rd_ptr(int n)
{
	assert((m_write_index - m_read_index) >= n);
	m_read_index += n;
    assert(m_read_index <= BUFFER_LENGTH);      // 不可超出缓冲区
}

template <int BUFFER_LENGTH>
char* Block_Buffer_T<BUFFER_LENGTH>::wr_ptr()
{
	return m_buffer + m_write_index;
}

template <int BUFFER_LENGTH>
void Block_Buffer_T<BUFFER_LENGTH>::wr_ptr(int n)
{
	assert((_length - m_write_index) >= n);
	m_write_index += n;
    assert(m_write_index <= BUFFER_LENGTH);     // 不可超出缓冲区
}

template <int BUFFER_LENGTH>
int Block_Buffer_T<BUFFER_LENGTH>::buffer_length()
{
	return _length;
}

template <int BUFFER_LENGTH>
int Block_Buffer_T<BUFFER_LENGTH>::data_length()
{
	return (m_write_index - m_read_index);
}

template <int BUFFER_LENGTH>
int Block_Buffer_T<BUFFER_LENGTH>::space_length()
{
	return (_length - m_write_index);
}

template <int BUFFER_LENGTH>
int Block_Buffer_T<BUFFER_LENGTH>::length()
{
	return _length;
}

template <int BUFFER_LENGTH>
void Block_Buffer_T<BUFFER_LENGTH>::recycle()
{
	int data_len = m_write_index - m_read_index;
	if (0 == data_len) {
		m_write_index = 0;
		m_read_index = 0;
	}
	else {
		memmove(m_buffer, m_buffer + m_read_index, data_len);
		m_read_index = 0;
		m_write_index = data_len;
	}
}

template <int BUFFER_LENGTH>
void Block_Buffer_T<BUFFER_LENGTH>::extend(int new_length)
{
	int data_len = m_write_index - m_read_index;
	char* ext_buffer = CnNew char[new_length+data_len];

	LOG_INFO("Block_Buffer_T extend from %d to %d for datalen:%d newlength:%d",
					_length, new_length+data_len,
					data_len, new_length);
	
	if(data_len == 0)
	{
		m_write_index = 0;
		m_read_index = 0;		
	}
	else
	{
		memmove(ext_buffer, m_buffer + m_read_index, data_len);
		m_read_index = 0;
		m_write_index = data_len;
	}
	
	CnDelete[] m_buffer;
	m_buffer = ext_buffer;
	_length = new_length+data_len;
}



#endif // _BLOCK_BUFFER_H_
