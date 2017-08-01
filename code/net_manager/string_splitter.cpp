#include "net_manager_pch.h"
#include "string_splitter.h"
#include "os.h"
#include "config.h"
#include "log.h"

// class String_Splitter
String_Splitter::~String_Splitter()
{
}
#if 0
int String_Splitter::split(const char *buf, int len, int& packet_begin, int& packet_len)
{
	if (len < 4) 
	{
		return 0;
	}
	packet_begin = 0;
	packet_len = len;
	return 1;
}
#else


int String_Splitter::split(const char *buf, int len, int& packet_begin, int& packet_len)
{
	if (len < 6) 
	{
		return 0;
	}

	int pack_len = 0;
        sscanf(buf, "%6x", &pack_len);
	
//	        if((pack_len + 6)> MAX_PACKET_LENGTH)
//		{
//			LOG_DEBUG("Exceed max packet len, max=%d pack_len=%d", MAX_PACKET_LENGTH, pack_len);
//	
//			LOG_HEX_PREFIX("RECV msg:", buf, len, utils::L_ERROR);
//			return -1;
//		}

	if(len >= (pack_len+6))
	{
		packet_begin = 0;
		//LOG_DEBUG("Packet Complete, len=%d pack_len=%d, discard more bytes if exist.", len, pack_len);
		packet_len = pack_len+6;
		return 1;
	}
	packet_begin = 0;
	packet_len = pack_len+6;

	LOG_DEBUG("Packet Not Complete, len=%d pack_len=%d", len, pack_len);
	return 0;
#if 0
	int pack_len = 0;
	packet_begin = 0;
	for(int i = 0; i < 4; i++)
	{
		if(buf[i] >= '0' && buf[i] <= '9')
		{
			pack_len = pack_len*0x10+(buf[i]-'0');
		}
		else if(buf[i] >= 'A' && buf[i] <= 'F')
		{
			pack_len = pack_len*0x10+(buf[i]-'A')+10;
		}
		else if(buf[i] >= 'a' && buf[i] <= 'f') 
		{
			pack_len = pack_len*0x10+(buf[i]-'a')+10;
		}
		else
		{
			packet_begin++;
		}
	}
	if (pack_len < 2) {
		return -1;
	}
	if (pack_len+4 > MAX_PACKET_LENGTH) {
		return -1;
	}

	if (len >= pack_len+4) {
		packet_len = pack_len+4;
		if(buf[packet_len] == '\r') packet_len++;
		if(buf[packet_len] == '\n') packet_len++;
		return 1;
	}

	return 0;
#endif
}
#endif
