#pragma once

/*
 *	a NetWork Pipe for Fake Transfer
 */
#ifndef FakeNetWorkPipe_h__
#define FakeNetWorkPipe_h__

#include "protocol_define.h"

typedef struct NetPack
{
	NetPack() : len(0), RevcTime(0) { memset(pData, 0, sizeof(pData)); }
	uint32	len;
	uint32	RevcTime;
	byte	pData[MAX_PACK_LEN];
} NetPack;

class FakeNetWorkPipe : private Crown::CSDObjectFifo<NetPack>
{
public:
	FakeNetWorkPipe();
	virtual ~FakeNetWorkPipe();

	void	Send(void* pData, uint32 len);
	uint32	Recv(void* pData, uint32 len);			// return data length received
private:
	uint32 m_PushThreadId, m_PopThreadId;

};

#endif // FakeNetWorkPipe_h__
