#ifndef NETHELP_H
#define NETHELP_H

#include <fcntl.h>

void SetNonBlock(int32 hSock, bool bNonBlock)
{
	int32 nFlags = fcntl(hSock, F_GETFL, 0);
	if(bNonBlock)
		nFlags |= O_NONBLOCK;
	else
		nFlags &= ~O_NONBLOCK;

	fcntl(hSock, F_SETFL, nFlags);
}

#endif
