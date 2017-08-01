#include "SDnetPCH.h"
#include "sdnetconfig.h"
#include "sdnet.h"
#include "stdarg.h"

int32 MAX_CONNECTION			= 10000;
int32 RECV_LOOP_BUF_SIZE		= (10*1024*1024);	//10M
int32 SEND_LOOP_BUF_SIZE		= (10*1024*1024);	//10M
int32 MAX_NET_EVENT			= (1024*1024);		//1M
uint32 DELAY_RELEASE_CPSOCK_TIME= 3;

int32 MAX_NET_REQUEST			= (1024*1024);		//1M
int32 POP_TO_BUF_COUNT			= 10;
int32 VAL_SO_SNDLOWAT			= 128*1024;
