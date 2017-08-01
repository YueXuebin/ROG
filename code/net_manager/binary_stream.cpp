#include "net_manager_pch.h"

#include "binary_stream.h"


// class Binary_Read_Stream
template<>
Binary_Read_Stream<NULL_HEAD>::Binary_Read_Stream(const char *buf, int len)
{
	m_buf = buf;
	m_len = len;
	m_pos = 0;	// 模板特化, 对于没有包头的包此处为0
	m_good = true;
	m_eof = false;
}


// class Binary_Write_Stream
template<>
Binary_Write_Stream<NULL_HEAD>::Binary_Write_Stream(char *buf, int len)
{
	m_buf = buf;
	m_len = len;
	m_pos = 0;	// 模板特化, 对于没有包头的包此处为0
	m_good = true;
	m_eof = false;
	m_begin_binary = false;
	m_binary_len_pos = 0;
	m_binary_len = 0;
}


template<>
void Binary_Write_Stream<NULL_HEAD>::reset()
{
    m_pos = 0;
	m_eof = false;

    m_begin_binary = false;
    m_binary_len_pos = 0;
    m_binary_len = 0;
}

template<>
void Binary_Write_Stream<NULL_HEAD>::detach()
{
	m_buf = NULL;
	m_len = 0;
	m_pos = 0;	
	m_good = false;
	m_eof = false;
	m_begin_binary = false;
	m_binary_len_pos = 0;
	m_binary_len = 0;
}


template<>
void Binary_Write_Stream<NULL_HEAD>::attach(char *buf, int len)
{
        m_buf = buf;
        m_len = len;
        m_pos = 0;

		m_good = true;
		m_eof = false;

        m_begin_binary = false;
        m_binary_len_pos = 0;
        m_binary_len = 0;
}
