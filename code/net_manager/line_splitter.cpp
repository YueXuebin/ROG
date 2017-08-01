#include "net_manager_pch.h"

#include "line_splitter.h"
#include "os.h"
#include "config.h"


// class Line_Splitter
Line_Splitter::~Line_Splitter()
{
}

int Line_Splitter::split(const char *buf, int len, int& packet_begin, int& packet_len)
{
	int p1 = -1;
	int p2 = -1;

	// 找到本行的起始字符(第一个非/r/n的字符)
	for (int i = 0; i < len; i++)
	{
		if ((buf[i] != '\r') && (buf[i] != '\n'))
		{
			p1 = i;
			break;
		}
	}
	// 没找到第一个非/r/n的字符
	if (-1 == p1)
	{
		if (len > MAX_PACKET_LENGTH) {
			return -1;
		}
		else {
			return 0;
		}
	}

	// 找到本行的结束字符(/r/n)
	for (int i = p1; i < len; i++)
	{
		if ((buf[i] == '\r') || (buf[i] == '\n'))
		{
			p2 = i - 1;
			break;
		}
	}
	// 没找到本行的结束字符(/r/n)
	if (-1 == p2)
	{
		if (len > MAX_PACKET_LENGTH) {
			return -1;
		}
		else {
			return 0;
		}
	}

	packet_begin = p1;
	packet_len = p2 - p1 + 1;
	if (packet_len > MAX_PACKET_LENGTH) {
		return -1;
	}

	return 1;
}

