#include "net_manager_pch.h"

#include "stream_types.h"


// class String
String::String()
{
	buf = NULL;
	len = 0;
}

String::String(const char *buf, int len)
{
	this->buf = buf;
	this->len = len;
}


// class Binary
Binary::Binary()
{
	buf = NULL;
	len = 0;
}

Binary::Binary(const char *buf, int len)
{
	this->buf = buf;
	this->len = len;
}

