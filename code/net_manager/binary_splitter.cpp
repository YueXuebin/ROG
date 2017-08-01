#include "net_manager_pch.h"
#include "binary_splitter.h"
#include "os.h"
#include "config.h"


// class Binary_Splitter

Binary_Splitter::Binary_Splitter()
{

}

Binary_Splitter::~Binary_Splitter()
{

}

int Binary_Splitter::split(const char *buf, int len, int& packet_begin, int& packet_len)
{
    if(len < 1)
    {
        return -2;
    }

    packet_begin = 0;
    packet_len = len;

    return 1;
}

