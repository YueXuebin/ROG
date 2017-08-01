

#include "os.h"


#define BS_MAKE64(lo, hi) ((uint64_t)((uint64_t)(lo) | (((uint64_t)(hi)) << 32)))
#define BS_LO32(x)  ((uint32_t)((x) & 0x00000000ffffffffULL))
#define BS_HI32(x)  ((uint32_t)(((x) & 0xffffffff00000000ULL) >> 32))

		
// class Binary_Read_Stream

template<typename HEAD>
Binary_Read_Stream<HEAD>::Binary_Read_Stream(const char *buf, int len)
{
	m_buf = buf;
	m_len = len;
	m_pos = 0;

	if ((int)sizeof(HEAD) > m_len)		// 包头长度超过buf长度
	{
		m_good = false;
		m_eof = false;
	}
	else
	{
		m_pos += sizeof(HEAD);
		m_good = true;
		m_eof = false;
	}
}

template<typename HEAD>
Binary_Read_Stream<HEAD>::~Binary_Read_Stream()
{
}

template<typename HEAD>
HEAD& Binary_Read_Stream<HEAD>::head()
{
	return *(HEAD*)m_buf;
}

template<typename HEAD>
Binary_Read_Stream<HEAD>& Binary_Read_Stream<HEAD>::operator >>(uint8_t &data)
{
	data = read_uint8();
	return *this;
}

template<typename HEAD>
Binary_Read_Stream<HEAD>& Binary_Read_Stream<HEAD>::operator >>(uint16_t &data)
{
	data = ntohs(read_uint16());
	return *this;
}

template<typename HEAD>
Binary_Read_Stream<HEAD>& Binary_Read_Stream<HEAD>::operator >>(uint32_t &data)
{
	data = ntohl(read_uint32());
	return *this;
}

template<typename HEAD>
Binary_Read_Stream<HEAD>& Binary_Read_Stream<HEAD>::operator >>(uint64_t &data)
{
	uint32_t hi, lo;
	(*this) >> hi >> lo;

	data = BS_MAKE64(lo, hi);
	return *this;
}

template<typename HEAD>
Binary_Read_Stream<HEAD>& Binary_Read_Stream<HEAD>::operator >>(String &data)
{
	String d = read_string();

	if ((data.buf != NULL))
	{
		//深拷贝
		if (data.len >= (d.len + 1)) {
			char *buf = const_cast<char*>(data.buf);
			memcpy((void*)buf, (void*)d.buf, d.len);
			data.len = d.len;
			buf[data.len] = 0;
		}
		else
		{
			m_good = false;
		}
	}
	else
	{
		//浅拷贝
		data.buf = d.buf;
		data.len = d.len;
	}

	return *this;
}
/*
template<typename HEAD>
Binary_Read_Stream<HEAD>& Binary_Read_Stream<HEAD>::operator >>(const String &data)
{
	String d = read_string();

	if (data.buf != NULL)
	{
        if (data.len >= (d.len + 1)) {
    		//深拷贝
    		char *buf = const_cast<char*>(data.buf);
    		memcpy((void*)buf, (void*)d.buf, d.len);
	     
    		buf[d.len] = 0;
        } else {
            m_good = false;
        }
	}
	else
	{
		//浅拷贝
		//NOTICE此处data为const, 浅拷贝无意义
	}

	return *this;
}
*/
template<typename HEAD>
Binary_Read_Stream<HEAD>& Binary_Read_Stream<HEAD>::operator >>(Binary &data)
{
	Binary b = read_binary();

	// TODO 此处是否也实现深拷贝

	data = b;

	return *this;
}

template<typename HEAD>
bool Binary_Read_Stream<HEAD>::eof() const
{
	return m_eof;
}

template<typename HEAD>
bool Binary_Read_Stream<HEAD>::good() const
{
	return m_good;
}

template<typename HEAD>
uint8_t Binary_Read_Stream<HEAD>::read_uint8()
{
	if (false == m_good) {
		return 0;
	}

	if (true == m_eof) {
		m_good = false;
		return 0;
	}

	if ((m_len - m_pos) < 1) {
		m_good = false;
		return 0;
	}

	uint8_t data = *(uint8_t*)(m_buf + m_pos);

	m_pos += 1;
	if (m_pos == m_len) {
		m_eof = true;
	}

	return data;
}

template<typename HEAD>
uint16_t Binary_Read_Stream<HEAD>::read_uint16()
{
	if (false == m_good) {
		return 0;
	}

	if (true == m_eof) {
		m_good = false;
		return 0;
	}

	if ((m_len - m_pos) < 2) {
		m_good = false;
		return 0;
	}

	uint16_t data = *(uint16_t*)(m_buf + m_pos);

	m_pos += 2;
	if (m_pos == m_len) {
		m_eof = true;
	}

	return data;
}

template<typename HEAD>
uint32_t Binary_Read_Stream<HEAD>::read_uint32()
{
	if (false == m_good) {
		return 0;
	}

	if (true == m_eof) {
		m_good = false;
		return 0;
	}

	if ((m_len - m_pos) < 4) {
		m_good = false;
		return 0;
	}

	uint32_t data = *(uint32_t*)(m_buf + m_pos);

	m_pos += 4;
	if (m_pos == m_len) {
		m_eof = true;
	}

	return data;
}

template<typename HEAD>
String Binary_Read_Stream<HEAD>::read_string()
{
	String str;

	if (false == m_good) {
		return str;
	}

	if (true == m_eof) {
		m_good = false;
		return str;
	}

	// read len
	if ((m_len - m_pos) < 2) {
		m_good = false;
		return str;
	}

	uint16_t len = *(uint16_t*)(m_buf + m_pos);
	len = ntohs(len);

	// read str
	if ((len == 0) || ((m_len - m_pos - 2) < len) || (m_buf[m_pos + 2 + len - 1] != 0)) {
		m_good = false;
		return str;
	}

	str.buf = m_buf + m_pos + 2;
	str.len = len - 1;

	m_pos += (2 + len);
	if (m_pos == m_len) {
		m_eof = true;
	}

	return str;
}

template<typename HEAD>
Binary Binary_Read_Stream<HEAD>::read_binary()
{
	Binary bin;

	if (false == m_good) {
		return bin;
	}

	if (true == m_eof) {
		m_good = false;
		return bin;
	}

	// read len
	if ((m_len - m_pos) < 2) {
		m_good = false;
		return bin;
	}

	uint16_t len = *(uint16_t*)(m_buf + m_pos);
	len = ntohs(len);

	// read str
	if ((len == 0) || ((m_len - m_pos - 2) < len)) {
		m_good = false;
		return bin;
	}

	bin.buf = m_buf + m_pos + 2;
	bin.len = len;

	m_pos += (2 + len);
	if (m_pos == m_len) {
		m_eof = true;
	}

	return bin;
}




// class Binary_Write_Stream
template<typename HEAD>
Binary_Write_Stream<HEAD>::Binary_Write_Stream()
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

template<typename HEAD>
Binary_Write_Stream<HEAD>::Binary_Write_Stream(char *buf, int len)
{
	m_buf = buf;
	m_len = len;
	m_pos = 0;
	
	

	if ((int)sizeof(HEAD) > m_len)			// 包头长度超过buf长度
	{
		m_good = false;
		m_eof = false;
	}
	else
	{
		m_pos += sizeof(HEAD);
		m_good = true;
		m_eof = false;
	}

	m_begin_binary = false;
	m_binary_len_pos = 0;
	m_binary_len = 0;
}

template<typename HEAD>
Binary_Write_Stream<HEAD>::~Binary_Write_Stream()
{
}

template<typename HEAD>
void Binary_Write_Stream<HEAD>::reset()
{
    m_pos = 0;

    if ((int)sizeof(HEAD) > m_len)                  // °üí·3¤?è3?1ybuf3¤?è
    {
            m_good = false;
            m_eof = false;
    }
    else
    {
            m_pos += sizeof(HEAD);
            m_good = true;
            m_eof = false;
    }

	m_eof = false;

    m_begin_binary = false;
    m_binary_len_pos = 0;
    m_binary_len = 0;
}

template<typename HEAD>
void Binary_Write_Stream<HEAD>::detach()
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


template<typename HEAD>
void Binary_Write_Stream<HEAD>::attach(char *buf, int len)
{
        m_buf = buf;
        m_len = len;
        m_pos = 0;

        if ((int)sizeof(HEAD) > m_len)                  // °üí·3¤?è3?1ybuf3¤?è
        {
                m_good = false;
                m_eof = false;
        }
        else
        {
                m_pos += sizeof(HEAD);
                m_good = true;
                m_eof = false;
        }

        m_begin_binary = false;
        m_binary_len_pos = 0;
        m_binary_len = 0;
}



template<typename HEAD>
HEAD& Binary_Write_Stream<HEAD>::head()
{
	return *(HEAD*)m_buf;
}

template<typename HEAD>
Binary_Write_Stream<HEAD>& Binary_Write_Stream<HEAD>::operator <<(uint8_t data)
{
	write_uint8(data);
	return *this;
}

template<typename HEAD>
Binary_Write_Stream<HEAD>& Binary_Write_Stream<HEAD>::operator <<(uint16_t data)
{
	write_uint16(htons(data));
	return *this;
}

template<typename HEAD>
Binary_Write_Stream<HEAD>& Binary_Write_Stream<HEAD>::operator <<(uint32_t data)
{
	write_uint32(htonl(data));
	return *this;
}

template<typename HEAD>
Binary_Write_Stream<HEAD>& Binary_Write_Stream<HEAD>::operator <<(uint64_t data)
{
	(*this) << BS_HI32(data) << BS_LO32(data);
	return *this;
}

template<typename HEAD>
Binary_Write_Stream<HEAD>& Binary_Write_Stream<HEAD>::operator <<(const String& data)
{
	if (data.buf == NULL) {
		m_good = false;
		return *this;
	}

	write_string(data);
	return *this;
}

template<typename HEAD>
Binary_Write_Stream<HEAD>& Binary_Write_Stream<HEAD>::operator <<(const char *data)
{
	if (data == NULL) {
		m_good = false;
		return *this;
	}

	write_string(String(data, strlen(data)));
	return *this;
}

template<typename HEAD>
Binary_Write_Stream<HEAD>& Binary_Write_Stream<HEAD>::operator <<(const Binary& data)
{
	if (data.buf == NULL) {
		m_good = false;
		return *this;
	}

	write_binary(data);
	return *this;
}

template<typename HEAD>
Binary_Write_Stream<HEAD>& Binary_Write_Stream<HEAD>::operator <<(Binary_Write_Stream& (*pfn)(Binary_Write_Stream&))
{
	return pfn(*this);
}

template<typename HEAD>
bool Binary_Write_Stream<HEAD>::eof() const
{
	return m_eof;
}

template<typename HEAD>
bool Binary_Write_Stream<HEAD>::good() const
{
	return m_good;
}

template<typename HEAD>
int Binary_Write_Stream<HEAD>::length() const
{
	return m_pos;
}

template<typename HEAD>
void Binary_Write_Stream<HEAD>::write_uint8(uint8_t data)
{
	if (false == m_good) {
		return;
	}

	if (true == m_eof) {
		m_good = false;
		return;
	}

	if (true == m_begin_binary) {
		m_good = false;
		return;
	}

	if ((m_len - m_pos) < 1) {
		m_good = false;
		return;
	}

	*(uint8_t*)(m_buf + m_pos) = data;

	m_pos += 1;
	if (m_pos == m_len) {
		m_eof = true;
	}

	return;
}

template<typename HEAD>
void Binary_Write_Stream<HEAD>::write_uint16(uint16_t data)
{
	if (false == m_good) {
		return;
	}

	if (true == m_eof) {
		m_good = false;
		return;
	}

	if (true == m_begin_binary) {
		m_good = false;
		return;
	}

	if ((m_len - m_pos) < 2) {
		m_good = false;
		return;
	}

	*(uint16_t*)(m_buf + m_pos) = data;

	m_pos += 2;
	if (m_pos == m_len) {
		m_eof = true;
	}

	return;
}

template<typename HEAD>
void Binary_Write_Stream<HEAD>::write_uint32(uint32_t data)
{
	if (false == m_good) {
		return;
	}

	if (true == m_eof) {
		m_good = false;
		return;
	}

	if (true == m_begin_binary) {
		m_good = false;
		return;
	}

	if ((m_len - m_pos) < 4) {
		m_good = false;
		return;
	}

	*(uint32_t*)(m_buf + m_pos) = data;

	m_pos += 4;
	if (m_pos == m_len) {
		m_eof = true;
	}

	return;
}

template<typename HEAD>
void Binary_Write_Stream<HEAD>::write_string(const String& data)
{
	if (false == m_good) {
		return;
	}

	if (true == m_eof) {
		m_good = false;
		return;
	}

	if (true == m_begin_binary) {
		m_good = false;
		return;
	}

	uint16_t len = data.len;
	if ((m_len - m_pos) < (2 + len + 1)) {
		m_good = false;
		return;
	}

	*(uint16_t*)(m_buf + m_pos) = htons(len + 1);
	memcpy(m_buf + m_pos + 2, data.buf, len);
	m_buf[m_pos + 2 + len] = 0;

	m_pos += (2 + len + 1);
	if (m_pos == m_len) {
		m_eof = true;
	}

	return;
}

template<typename HEAD>
void Binary_Write_Stream<HEAD>::begin_binary()
{
	if (false == m_good) {
		return;
	}

	if (true == m_eof) {
		m_good = false;
		return;
	}

	if (true == m_begin_binary) {
		m_good = false;
		return;
	}

	if ((m_len - m_pos) < 2) {
		m_good = false;
		return;
	}

	m_begin_binary = true;
	m_binary_len_pos = m_pos;
	m_binary_len = 0;

	*(uint16_t*)(m_buf + m_binary_len_pos) = htons(m_binary_len);

	m_pos += 2;
	if (m_pos == m_len) {
		m_eof = true;
	}

	return;
}

template<typename HEAD>
void Binary_Write_Stream<HEAD>::end_binary()
{
	if (false == m_good) {
		return;
	}

	if (true == m_eof) {
		m_good = false;
		return;
	}

	if (false == m_begin_binary) {
		m_good = false;
		return;
	}

	m_begin_binary = false;
	m_binary_len_pos = 0;
	m_binary_len = 0;
	return;
}

template<typename HEAD>
void Binary_Write_Stream<HEAD>::write_binary(const Binary& data)
{
	if (false == m_good) {
		return;
	}

	if (true == m_eof) {
		m_good = false;
		return;
	}

	if (false == m_begin_binary)
	{
		// 写入一个完整数据块
		if ((m_len - m_pos) < (2 + data.len)) {
			m_good = false;
			return;
		}

		*(uint16_t*)(m_buf + m_pos) = htons(data.len);
		memcpy(m_buf + m_pos + 2, data.buf, data.len);

		m_pos += (2 + data.len);
		if (m_pos == m_len) {
			m_eof = true;
		}
	}
	else
	{
		// 写入部分数据块
		if ((m_len - m_pos) < data.len) {
			m_good = false;
			return;
		}

		memcpy(m_buf + m_pos, data.buf, data.len);
		m_binary_len += data.len;
		*(uint16_t*)(m_buf + m_binary_len_pos) = htons(m_binary_len);

		m_pos += data.len;
		if (m_pos == m_len) {
			m_eof = true;
		}
	}

	return;
}

template<typename HEAD>
void Binary_Write_Stream<HEAD>::write_raw(char* data, int length)
{
	if (false == m_good) {
		return;
	}

	if (true == m_eof) {
		m_good = false;
		return;
	}

	if ((m_len - m_pos) < length) {
		m_good = false;
		return;
	}

	memcpy(m_buf + m_pos, data, length);

	m_pos += length;
	if (m_pos == m_len) {
		m_eof = true;
	}

	return;
}

template<typename HEAD>
Binary_Write_Stream<HEAD>& Binary_Begin(Binary_Write_Stream<HEAD>& write_stream)
{
	write_stream.begin_binary();
	return write_stream;
}

template<typename HEAD>
Binary_Write_Stream<HEAD>& Binary_End(Binary_Write_Stream<HEAD>& write_stream)
{
	write_stream.end_binary();
	return write_stream;
}

