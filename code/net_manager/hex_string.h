//! @file hex_string.h
//! @brief ʮ�������ַ���


#ifndef _HEX_STRING_H_
#define _HEX_STRING_H_


namespace utils {


//! ʮ�������ַ���������
class HexString
{
public:
	//! ����������תʮ�������ַ���
	//! @param in ��������
	//! @param in_len �������ݳ���
	//! @param out ���������
	//! @param out_buf_len �������������
	//! @param out_len ������ݳ���
	//!
	//! in �� out ����ָ��ͬһ������
	static void bin2asc(const char *in, int in_len, char *out, int out_buf_len, int &out_len);

	//! ʮ�������ַ���ת����������
	//! @param in ��������
	//! @param in_len �������ݳ���
	//! @param out ���������
	//! @param out_buf_len �������������
	//! @param out_len ������ݳ���
	//!
	//! in �� out ����ָ��ͬһ������
	static void asc2bin(const char *in, int in_len, char *out, int out_buf_len, int &out_len);

private:
	static const char* asc_map[256];
	static unsigned char bin_map[256];
};


} // namepsace utils


#endif // _HEX_STRING_H_
